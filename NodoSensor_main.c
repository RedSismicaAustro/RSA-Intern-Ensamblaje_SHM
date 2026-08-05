/*-------------------------------------------------------------------------------------------------------------------------
Nodo Sensor A - Sistema de Monitorizacion de Salud Estructural V1.5
Autor: David Timbi

Descripcion:
  Firmware de nodo sensor, capta el pulso de sincronizacion INT_SINC, lee el
  acelerometro ADXL355 y gestiona mensajes RS485 hacia/desde el concentrador.

Hardware:
  dsPIC33EP256MC202, XT=80MHz, MAX485 U5 (entrada de sync permanente) y
  MAX485 U9 (datos bidireccional).

---------------------------------------------------------------------------------------------------------------------------*/

////////////////////////////////////////////////////         Librerias         /////////////////////////////////////////////////////////////

#include <ADXL355_SPI.c>          // Driver del acelerometro triaxial (SPI2)
#include <TIEMPO_RPI.c>           // Utilidades para interpretar la trama de tiempo recibida por RS485
#include <RS485.c>                // Modulo compartido con el Concentrador (EnviarTramaRS485, etc.)

#include <spiSD.h>
#include <sdcard.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Credenciales del nodo
#define IDNODO 1                                                                // ID RS485 del nodo: 1 = Nodo A
#define SIZESD 8                                                                // Capacidad de la SD en GB
#define DELTASECTOR 97952                                                       // Offset de inicio de datos desde PSF

// Depuracion / validacion hardware
#define DEBUG_TOGGLE_EN_SYNC 1                                                  // Si es 1, TEST conmuta con INT_SINC

//////////////////////////////////////////////  Variables globales  /////////////////////////////////////////////////////////////

// Constantes de sistema
#define FP 80000000                                                             // Frecuencia del reloj del dsPIC

// Indices generales
unsigned int i, j, x, y;

// Pines y perifericos
struct sdflags sdflags;                                                         // Estructura usada por sdcard.c
sbit TEST at LATA2_bit;                                                         // LED TEST1
sbit TEST_Direction at TRISA2_bit;

sbit CsADXL at LATA3_bit;                                                       // Chip select del ADXL355 (SPI2)
sbit CsADXL_Direction at TRISA3_bit;

sbit sd_CS_lat at LATB0_bit;                                                    // Chip select de la MicroSD
sbit sd_CS_tris at TRISB0_bit;
sbit sd_detect_port at LATA4_bit;                                               // Deteccion de tarjeta SD
sbit sd_detect_tris at TRISA4_bit;

sbit MSRS485 at LATB12_bit;                                                     // Control DE/RE del MAX485 de datos
sbit MSRS485_Direction at TRISB12_bit;

// El MAX485 U5 esta en modo recepcion permanente; su salida INT_SINC se conecta
// directamente a la interrupcion externa INT1.

// Estado general del nodo
unsigned short inicioSistema;

// Tiempo del sistema
unsigned short tiempo[6] = {0, 0, 0, 0, 0, 0};                                  // Vector de tiempo recibido por RS485
unsigned short banSetReloj;                                                     // 1 = hora inicial cargada
unsigned short fuenteReloj;                                                     // 1=GPS, 2=RTC, 3=RPi
unsigned long horaSistema, fechaSistema;

// Control del acelerometro
unsigned short banCiclo, banInicioMuestreo;
unsigned char datosLeidos[9] = {0};
unsigned char datosFIFO[243];                                                   // FIFO del ADXL355: 27 muestras x 3 ejes
unsigned char tramaAceleracion[2500];
unsigned short numFIFO, numSetsFIFO;
unsigned short contTimer1;                                                      // Desbordamientos de TMR1 dentro del segundo
unsigned short contMuestras;
unsigned short contCiclos;
unsigned int contFIFO;
short tasaMuestreo;
short numTMR1;

// Protocolo RS485
unsigned short banRSI, banRSC;                                                  // Control de recepcion de trama
unsigned char byteRS485;
unsigned int i_rs485;
unsigned char tramaCabeceraRS485[10];                                           // [0x3A, Direccion, Funcion, numDatos LSB, numDatos MSB]
unsigned char inputPyloadRS485[15];
unsigned char outputPyloadRS485[15];
unsigned int numDatosRS485;
unsigned char *ptrnumDatosRS485;
unsigned short funcionRS485;                                                    // 0xF1=Tiempo, 0xF2=Muestreo, 0xF3=Lectura
unsigned short subFuncionRS485;
unsigned char *ptrsectorReq;
unsigned long sectorReq;
unsigned short contTMR2;                                                        // Timeout de recuperacion del UART1

// Almacenamiento SD
unsigned long PSF, PSE, USF, PSEC;
unsigned long sectorSD, sectorLec;
const unsigned int clusterSizeSD = 512;
unsigned long infoPrimerSector, infoUltimoSector;
unsigned char cabeceraSD[6] = {255, 253, 251, 10, 0, 250};                      // [Cte1, Cte2, Cte3, #Bytes/Muestra, MSB_fSample, LSB_fSample]
unsigned char bufferSD[clusterSizeSD];
unsigned char checkEscSD, checkLecSD;
unsigned short banInsSec;

/////////////////////////////////////////////////////////  Declaracion de funciones  /////////////////////////////////////////////////////////
void ConfiguracionPrincipal();
void Muestrear();
void GuardarBufferSD(unsigned char* bufferLleno, unsigned long sector);
void GuardarTramaSD(unsigned char* tiempoSD, unsigned char* aceleracionSD);
void GuardarInfoSector(unsigned long sector, unsigned long localizacionSector);
unsigned long UbicarPrimerSectorEscrito();
unsigned long UbicarUltimoSectorEscrito(unsigned short sobrescribirSD);
void InformacionSectores();
void InspeccionarSector(unsigned short estadoMuestreo, unsigned long sectorReq);
void RecuperarTramaAceleracion(unsigned long sectorReq);

//////////////////////////////////////////////////////////////  MAIN  ////////////////////////////////////////////////////////////////
void main() {

     ConfiguracionPrincipal();
     TEST = 0;

     tasaMuestreo = 1;                                                          //1=250Hz, 2=125Hz, 4=62.5Hz, 8=31.25Hz
     ADXL355_init(tasaMuestreo);
     numTMR1 = (tasaMuestreo*10)-1;

     //Inicializacion de variables:
     i = 0; j = 0; x = 0; y = 0;
     inicioSistema = 0;

     banSetReloj = 0;
     horaSistema = 0;
     fechaSistema = 0;
     fuenteReloj = 2;                                                           //Por defecto se asume fuente RTC hasta que el Concentrador informe otra cosa

     banCiclo = 0;
     banInicioMuestreo = 0;
     numFIFO = 0; numSetsFIFO = 0;
     contTimer1 = 0; contMuestras = 0; contCiclos = 0; contFIFO = 0;

     banRSI = 0; banRSC = 0;
     byteRS485 = 0; i_rs485 = 0;
     funcionRS485 = 0; subFuncionRS485 = 0;
     numDatosRS485 = 0;
     ptrnumDatosRS485 = (unsigned char *) &numDatosRS485;
     ptrsectorReq = (unsigned char *) &sectorReq;
     contTMR2 = 0;

     PSEC = 0; sectorSD = 0; sectorLec = 0;
     checkEscSD = 0; checkLecSD = 0;
     MSRS485 = 0;                                                               //MAX485 de datos en modo lectura (escucha)
     banInsSec = 0;

     //Determina el ultimo sector fisico en funcion de la capacidad de la SD:
     switch (SIZESD){
            case 2:  PSF = 2048; USF = 3911679;  break;
            case 4:  PSF = 2048; USF = 7772160;  break;
            case 8:  PSF = 2048; USF = 16779263; break;
            case 16: PSF = 2048; USF = 31115263; break;
     }
     infoPrimerSector = PSF+DELTASECTOR-2;
     infoUltimoSector = PSF+DELTASECTOR-1;
     PSE = PSF+DELTASECTOR;

     //Espera a que la SD este conectada:
     while (1) {
           if (SD_Detect() == DETECTED) {
              sdflags.detected = true;
              break;
           } else {
              sdflags.detected = false;
              sdflags.init_ok = false;
           }
           Delay_ms(100);
     }

     //Inicializa la SD:
     if (sdflags.detected && !sdflags.init_ok) {
          if (SD_Init_Try(10) == SUCCESSFUL_INIT) {
              sdflags.init_ok = true;
              inicioSistema = 1;
              TEST = 1;
           } else {
              sdflags.init_ok = false;
              INT1IE_bit = 0;                                                   //Sin SD valida, no tiene sentido reaccionar al pulso de sync
              U1MODE.UARTEN = 0;
              inicioSistema = 0;
              TEST = 0;
           }
     }
     Delay_ms(2000);

     while(1){
            asm CLRWDT;
            Delay_ms(100);
     }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////  FUNCIONES  ////////////////////////////////////////////////////////////////

//*****************************************************************************************************************************************
// Configuracion principal del nodo
void ConfiguracionPrincipal(){

     //Configuracion del oscilador:                                             //FPLLO = FIN*(M/(N1+N2)) = 80.017MHz
     CLKDIVbits.FRCDIV = 0;
     CLKDIVbits.PLLPOST = 0;
     CLKDIVbits.PLLPRE = 5;
     PLLFBDbits.PLLDIV = 150;

     //Configuracion de puertos:
     ANSELA = 0;
     ANSELB = 0;
     TEST_Direction = 0;
     CsADXL_Direction = 0;
     sd_CS_tris = 0;
     MSRS485_Direction = 0;
     sd_detect_tris = 1;
     TRISB14_bit = 1;                                                           //RB14/RPI46 = INT_SINC, entrada desde el MAX485 U5

     INTCON2.GIE = 1;                                                           //Habilita interrupciones globales

     //Configuracion del UART1 (bus RS485 principal, MAX485 U9):
     RPINR18bits.U1RXR = 0x2F;                                                  //Rx1 en RB15/RPI47   [VERIFICAR]
     RPOR1bits.RP36R = 0x01;                                                    //Tx1 en RB4/RP36     [VERIFICAR]
     U1RXIE_bit = 1;
     U1STAbits.URXISEL = 0x00;
     U1RXIF_bit = 0;
     IPC2bits.U1RXIP = 0x04;
     UART1_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED);

     //Configuracion del SPI2 en modo Maestro (interfaz con el ADXL355):
     RPINR22bits.SDI2R = 0x21;                                                  //SDI2 en RB1/RPI33   [VERIFICAR]
     RPOR2bits.RP38R = 0x08;                                                    //SDO2 en RB6/RP38    [VERIFICAR]
     RPOR1bits.RP37R = 0x09;                                                    //SCK2 en RB5/RP37    [VERIFICAR]
     SPI2STAT.SPIEN = 1;
     SPI2_Init();

     //Configuracion de la interrupcion externa INT1 <- INT_SINC (MAX485 U5, escucha permanente):
     //Este pulso llega 1 vez por segundo desde el Concentrador (Timer3, PERIODO_SYNC_MS=1000
     //en NodoConcentrador/main.c) y cumple doble funcion:
     //   1) Disparar el muestreo/volcado de datos de forma sincronizada en todos los nodos.
     //   2) Servir de base de tiempo local (el nodo no tiene RTC/GPS propio; el "reloj de
     //      pared" se ajusta una unica vez via RS485 funcion 0xF1/D1 y luego se incrementa
     //      un segundo por cada pulso INT_SINC capturado).
     RPINR0 = 0x2E00;                                                           //INT1 <- RB14/RPI46 (INT_SINC)   [VERIFICAR]
     INT1IE_bit = 1;
     INT1IF_bit = 0;
     IPC5bits.INT1IP = 0x01;

     //Configuracion del TMR1: interpolacion del vector FIFO dentro del segundo entre pulsos INT_SINC
     T1CON = 0x0020;
     T1CON.TON = 0;
     T1IE_bit = 1;
     T1IF_bit = 0;
     PR1 = 62500;                                                               //100ms
     IPC0bits.T1IP = 0x02;

     //Configuracion del TMR2: recuperacion del UART1 tras una trama RS485 no dirigida a este nodo
     T2CON = 0x0030;
     T2CON.TON = 0;
     T2IE_bit = 1;
     T2IF_bit = 0;
     PR2 = 46875;                                                               //300ms
     IPC1bits.T2IP = 0x02;

     //Configuracion del acelerometro (modo reposo hasta iniciar muestreo):
     ADXL355_write_byte(POWER_CTL, DRDY_OFF|STANDBY);

     sdflags.detected = false;
     sdflags.init_ok = false;
     sdflags.saving = false;

     Delay_ms(200);

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// Muestreo (cierre de ciclo de 1s): recupera lo que quede en el FIFO, arma la trama de
// aceleracion con marca de tiempo y la guarda en la SD.
void Muestrear(){

     if (banCiclo==0){

         ADXL355_write_byte(POWER_CTL, DRDY_OFF|MEASURING);
         T1CON.TON = 1;
         TMR1 = 0;

     } else if (banCiclo==1) {

         banCiclo = 2;

         tramaAceleracion[0] = fuenteReloj;                                     //Marca de tiempo: fuente de reloj del ciclo
         numFIFO = ADXL355_read_byte(FIFO_ENTRIES);
         numSetsFIFO = (numFIFO)/3;

         for (x=0;x<numSetsFIFO;x++){
             ADXL355_read_FIFO(datosLeidos);
             for (y=0;y<9;y++){
                 datosFIFO[y+(x*9)] = datosLeidos[y];
             }
         }

         for (x=0;x<(numSetsFIFO*9);x++){
             if ((x==0)||(x%9==0)){
                tramaAceleracion[contFIFO+contMuestras+x] = contMuestras;
                tramaAceleracion[contFIFO+contMuestras+x+1] = datosFIFO[x];
                contMuestras++;
             } else {
                tramaAceleracion[contFIFO+contMuestras+x] = datosFIFO[x];
             }
         }

         contMuestras = 0;
         contFIFO = 0;
         T1CON.TON = 1;
         TMR1 = 0;

         //Empaqueta y guarda la trama [cabecera + marca de tiempo + datos de aceleracion]:
         GuardarTramaSD(tiempo, tramaAceleracion);

         if (banInsSec==1){
            InspeccionarSector(1, sectorReq);
         }

     }

     contCiclos++;

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
void GuardarBufferSD(unsigned char* bufferLleno, unsigned long sector){
     for (x=0;x<5;x++){
         checkEscSD = SD_Write_Block(bufferLleno,sector);
         if (checkEscSD == DATA_ACCEPTED){
             break;
         }
         Delay_us(10);
     }
}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// [Cabecera(6) + tiempo(6) + aceleracion(2500)] = 2512 bytes = 5 sectores SD (4 completos + 1 con relleno)
void GuardarTramaSD(unsigned char* tiempoSD, unsigned char* aceleracionSD){

        for (x=0;x<6;x++) bufferSD[x] = cabeceraSD[x];
        for (x=0;x<6;x++) bufferSD[6+x] = tiempoSD[x];
        for (x=0;x<500;x++) bufferSD[12+x] = aceleracionSD[x];
        GuardarBufferSD(bufferSD, sectorSD); sectorSD++;

        for (x=0;x<512;x++) bufferSD[x] = aceleracionSD[x+500];
        GuardarBufferSD(bufferSD, sectorSD); sectorSD++;

        for (x=0;x<512;x++) bufferSD[x] = aceleracionSD[x+1012];
        GuardarBufferSD(bufferSD, sectorSD); sectorSD++;

        for (x=0;x<512;x++) bufferSD[x] = aceleracionSD[x+1524];
        GuardarBufferSD(bufferSD, sectorSD); sectorSD++;

        for (x=0;x<512;x++){
            if (x<464) bufferSD[x] = aceleracionSD[x+2036];
            else bufferSD[x] = 0;
        }
        GuardarBufferSD(bufferSD, sectorSD); sectorSD++;

        if (horaSistema%300==0){
           GuardarInfoSector(sectorSD, infoUltimoSector);
        }

        TEST = 0;

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
void GuardarInfoSector(unsigned long datoSector, unsigned long localizacionSector){

     unsigned char bufferSectores[512];
     bufferSectores[0] = (datoSector>>24)&0xFF;
     bufferSectores[1] = (datoSector>>16)&0xFF;
     bufferSectores[2] = (datoSector>>8)&0xFF;
     bufferSectores[3] = (datoSector)&0xFF;
     for (x=4;x<512;x++) bufferSectores[x] = 0;

     for (x=0;x<5;x++){
         checkEscSD = SD_Write_Block(bufferSectores,localizacionSector);
         if (checkEscSD == DATA_ACCEPTED) break;
         Delay_us(10);
     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
unsigned long UbicarPrimerSectorEscrito(){

     unsigned char bufferSectorInicio[512];
     unsigned long primerSectorSD;
     unsigned char *ptrPrimerSectorSD;
     ptrPrimerSectorSD = (unsigned char *) &primerSectorSD;

     checkLecSD = 1;
     for (x=0;x<5;x++){
         checkLecSD = SD_Read_Block(bufferSectorInicio, infoPrimerSector);
         if (checkLecSD==0) {
            *ptrPrimerSectorSD = bufferSectorInicio[3];
            *(ptrPrimerSectorSD+1) = bufferSectorInicio[2];
            *(ptrPrimerSectorSD+2) = bufferSectorInicio[1];
            *(ptrPrimerSectorSD+3) = bufferSectorInicio[0];
            break;
         } else {
            primerSectorSD = PSE;
         }
     }
     return primerSectorSD;

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
unsigned long UbicarUltimoSectorEscrito(unsigned short sobrescribirSD){

     unsigned char bufferSectorFinal[512];
     unsigned long sectorInicioSD;
     unsigned char *ptrSectorInicioSD;
     ptrSectorInicioSD = (unsigned char *) &sectorInicioSD;

     if (sobrescribirSD==1){
         sectorInicioSD = PSE;
     } else {
         checkLecSD = 1;
         for (x=0;x<5;x++){
             checkLecSD = SD_Read_Block(bufferSectorFinal, infoUltimoSector);
             if (checkLecSD==0) {
                *ptrSectorInicioSD = bufferSectorFinal[3];
                *(ptrSectorInicioSD+1) = bufferSectorFinal[2];
                *(ptrSectorInicioSD+2) = bufferSectorFinal[1];
                *(ptrSectorInicioSD+3) = bufferSectorFinal[0];
                break;
             } else {
                sectorInicioSD = PSE;
             }
         }
     }
     return sectorInicioSD;

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
void InformacionSectores(){

     unsigned char tramaInfoSec[20];
     unsigned long infoPSF, infoPSE, infoPSEC, infoSA;
     unsigned char *ptrPSF, *ptrPSE, *ptrPSEC, *ptrSA;

     infoPSF = PSF; infoPSE = PSE;
     ptrPSF = (unsigned char *) &infoPSF;
     ptrPSE = (unsigned char *) &infoPSE;
     ptrPSEC = (unsigned char *) &infoPSEC;
     ptrSA = (unsigned char *) &infoSA;

     if (banInicioMuestreo==0){
        infoPSEC = UbicarPrimerSectorEscrito();
        infoSA = UbicarUltimoSectorEscrito(0);
     } else {
        infoSA = sectorSD - 1;
        infoPSEC = PSEC;
     }

     tramaInfoSec[0] = 0xD1;
     tramaInfoSec[1] = *ptrPSF; tramaInfoSec[2] = *(ptrPSF+1);
     tramaInfoSec[3] = *(ptrPSF+2); tramaInfoSec[4] = *(ptrPSF+3);
     tramaInfoSec[5] = *ptrPSE; tramaInfoSec[6] = *(ptrPSE+1);
     tramaInfoSec[7] = *(ptrPSE+2); tramaInfoSec[8] = *(ptrPSE+3);
     tramaInfoSec[9] = *ptrPSEC; tramaInfoSec[10] = *(ptrPSEC+1);
     tramaInfoSec[11] = *(ptrPSEC+2); tramaInfoSec[12] = *(ptrPSEC+3);
     tramaInfoSec[13] = *ptrSA; tramaInfoSec[14] = *(ptrSA+1);
     tramaInfoSec[15] = *(ptrSA+2); tramaInfoSec[16] = *(ptrSA+3);

     delay_ms(10);
     EnviarTramaRS485(RS485_PUERTO_UART1, IDNODO, 0xF3, 17, tramaInfoSec);

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
void InspeccionarSector(unsigned short estadoMuestreo, unsigned long sectorReq){

     unsigned char tramaDatosSec[15];
     unsigned char bufferSectorReq[512];
     unsigned int numDatosSec;
     unsigned long USE;

     if (estadoMuestreo==0) USE = UbicarUltimoSectorEscrito(0);
     else USE = sectorSD - 1;

     tramaDatosSec[0] = 0xD2;

     if ((sectorReq>=PSE)&&(sectorReq<USF)){
         if (sectorReq<USE){
             checkLecSD = 1;
             for (x=0;x<5;x++){
                 checkLecSD = SD_Read_Block(bufferSectorReq, sectorReq);
                 if (checkLecSD==0) {
                    numDatosSec = 14;
                    for (y=0;y<numDatosSec;y++) tramaDatosSec[y+1] = bufferSectorReq[y];
                    break;
                } else {
                    numDatosSec = 3;
                    tramaDatosSec[1] = 0xEE; tramaDatosSec[2] = 0xE3;
                }
                Delay_us(10);
            }
        } else {
            numDatosSec = 3;
            tramaDatosSec[1] = 0xEE; tramaDatosSec[2] = 0xE2;
        }
    } else {
        numDatosSec = 3;
        tramaDatosSec[1] = 0xEE; tramaDatosSec[2] = 0xE1;
    }

    banInsSec = 0;
    delay_ms(10);
    EnviarTramaRS485(RS485_PUERTO_UART1, IDNODO, 0xF3, numDatosSec, tramaDatosSec);

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
void RecuperarTramaAceleracion(unsigned long sectorReq){

    unsigned char tramaAcelSeg[2515];
    unsigned char bufferSectorReq[512];
    unsigned short tiempoAcel[6];
    unsigned long contSector;
    unsigned int numDatosTramaAcel;
    unsigned short banLecturaCorrecta;

    tramaAcelSeg[0] = 0xD3;
    contSector = 0;
    banLecturaCorrecta = 0;
    numDatosTramaAcel = 2513;

    checkLecSD = 1;
    for (x=0;x<5;x++){
        checkLecSD = SD_Read_Block(bufferSectorReq, (sectorReq+contSector));
        if (checkLecSD==0) {
            for (y=0;y<6;y++) tiempoAcel[y] = bufferSectorReq[y+6];
            for (y=0;y<6;y++) tramaAcelSeg[y+1] = bufferSectorReq[y];
            for (y=0;y<500;y++) tramaAcelSeg[y+7] = bufferSectorReq[y+12];
            banLecturaCorrecta = 1; contSector++;
            break;
        } else {
            tramaAcelSeg[1] = 0xEE; tramaAcelSeg[2] = 0xE3;
            numDatosTramaAcel = 3; banLecturaCorrecta = 2;
        }
        Delay_us(10);
    }

    if (banLecturaCorrecta==1){
        checkLecSD = 1;
        for (x=0;x<5;x++){
            checkLecSD = SD_Read_Block(bufferSectorReq, (sectorReq+contSector));
            if (checkLecSD==0) {
                for (y=0;y<512;y++) tramaAcelSeg[y+507] = bufferSectorReq[y];
                banLecturaCorrecta = 1; contSector++;
                break;
            } else {
                tramaAcelSeg[1] = 0xEE; tramaAcelSeg[2] = 0xE3;
                numDatosTramaAcel = 3; banLecturaCorrecta = 2;
            }
            Delay_us(10);
        }
    }

    if (banLecturaCorrecta==1){
        checkLecSD = 1;
        for (x=0;x<5;x++){
            checkLecSD = SD_Read_Block(bufferSectorReq, (sectorReq+contSector));
            if (checkLecSD==0) {
                for (y=0;y<512;y++) tramaAcelSeg[y+1019] = bufferSectorReq[y];
                banLecturaCorrecta = 1; contSector++;
                break;
            } else {
                tramaAcelSeg[1] = 0xEE; tramaAcelSeg[2] = 0xE3;
                numDatosTramaAcel = 3; banLecturaCorrecta = 2;
            }
            Delay_us(10);
        }
    }

    if (banLecturaCorrecta==1){
        checkLecSD = 1;
        for (x=0;x<5;x++){
            checkLecSD = SD_Read_Block(bufferSectorReq, (sectorReq+contSector));
            if (checkLecSD==0) {
                for (y=0;y<512;y++) tramaAcelSeg[y+1531] = bufferSectorReq[y];
                banLecturaCorrecta = 1; contSector++;
                break;
            } else {
                tramaAcelSeg[1] = 0xEE; tramaAcelSeg[2] = 0xE3;
                numDatosTramaAcel = 3; banLecturaCorrecta = 2;
            }
            Delay_us(10);
        }
    }

    if (banLecturaCorrecta==1){
        checkLecSD = 1;
        for (x=0;x<5;x++){
            checkLecSD = SD_Read_Block(bufferSectorReq, (sectorReq+contSector));
            if (checkLecSD==0) {
                for (y=0;y<464;y++) tramaAcelSeg[y+2043] = bufferSectorReq[y];
                banLecturaCorrecta = 1;
                break;
            } else {
                tramaAcelSeg[1] = 0xEE; tramaAcelSeg[2] = 0xE3;
                numDatosTramaAcel = 3; banLecturaCorrecta = 2;
            }
            Delay_us(10);
        }
    }

    if (banLecturaCorrecta==1){
        for (x=0;x<6;x++) tramaAcelSeg[2507+x] = tiempoAcel[x];
        TEST = ~TEST;
    }

    delay_ms(10);
    EnviarTramaRS485(RS485_PUERTO_UART1, IDNODO, 0xF3, numDatosTramaAcel, tramaAcelSeg);

}
//*****************************************************************************************************************************************

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////  INTERRUPCIONES  /////////////////////////////////////////////////////////////

//*****************************************************************************************************************************************
// INT1 <- INT_SINC: pulso de sincronizacion recibido del Concentrador via MAX485 U5.
// Fase 2 - "Configuracion de la interrupcion externa para la captura inmediata del pulso
// de sincronizacion" + base de tiempo local del nodo.
void int_1() org IVT_ADDR_INT1INTERRUPT {

     INT1IF_bit = 0;

     #if DEBUG_TOGGLE_EN_SYNC
        TEST = ~TEST;                                                           //Testigo para osciloscopio (Fase 3): mide retardo vs. INT_SINC_1 del Concentrador
     #endif

     //Sobrescribe el PSEC al cambio de dia (0 horas):
     if ((horaSistema==0)&&(banInicioMuestreo==1)){
        PSEC = sectorSD;
        GuardarInfoSector(PSEC, infoPrimerSector);
     }

     //Incrementa el reloj local 1 segundo por cada pulso INT_SINC (asume PERIODO_SYNC_MS=1000
     //en el Concentrador). Solo corre una vez que el Concentrador ya establecio hora/fecha.
     if (banSetReloj==1){
        horaSistema++;
        if (horaSistema==86400){
           horaSistema = 0;
           fechaSistema = IncrementarFecha(fechaSistema);
        }
        AjustarTiempoSistema(horaSistema, fechaSistema, tiempo);
     }

     if (banInicioMuestreo==1){
        Muestrear();
     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// TMR1: interpolacion del vector FIFO del ADXL355 entre pulsos INT_SINC (100ms x tasaMuestreo)
void Timer1Int() org IVT_ADDR_T1INTERRUPT{

     T1IF_bit = 0;

     numFIFO = ADXL355_read_byte(FIFO_ENTRIES);
     numSetsFIFO = (numFIFO)/3;

     for (x=0;x<numSetsFIFO;x++){
         ADXL355_read_FIFO(datosLeidos);
         for (y=0;y<9;y++){
             datosFIFO[y+(x*9)] = datosLeidos[y];
         }
     }

     for (x=0;x<(numSetsFIFO*9);x++){
         if ((x==0)||(x%9==0)){
            tramaAceleracion[contFIFO+contMuestras+x] = contMuestras;
            tramaAceleracion[contFIFO+contMuestras+x+1] = datosFIFO[x];
            contMuestras++;
         } else {
            tramaAceleracion[contFIFO+contMuestras+x] = datosFIFO[x];
         }
     }

     contFIFO = (contMuestras*9);
     contTimer1++;

     if (contTimer1==numTMR1){
        T1CON.TON = 0;
        banCiclo = 1;
        contTimer1 = 0;
     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// TMR2: si una trama RS485 no iba dirigida a este nodo, reactiva el UART1 tras 1200ms
// (evita que el nodo quede sordo al bus por una trama ajena).
void Timer2Int() org IVT_ADDR_T2INTERRUPT{

     T2IF_bit = 0;
     contTMR2++;

     if (contTMR2==4){
         T2CON.TON = 0;
         TMR2 = 0;
         contTMR2 = 0;
         banRSI = 0;
         banRSC = 0;
         i_rs485 = 0;
         UART1_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED);
     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// UART1: recepcion de tramas RS485 desde el Concentrador (protocolo compartido, ver RS485.c).
// Fase 2 - "Control de mensajeria y protocolo RS485 con direccion de nodo para evitar
// colisiones": el nodo solo procesa/responde si la cabecera trae su IDNODO o broadcast (255);
// en caso contrario se apaga el UART temporalmente para no interferir con la trama en curso.
void urx_1() org  IVT_ADDR_U1RXINTERRUPT {

     U1RXIF_bit = 0;
     byteRS485 = U1RXREG;
     OERR_bit = 0;

     if (banRSI==2){
        if (i_rs485<(numDatosRS485)){
           inputPyloadRS485[i_rs485] = byteRS485;
           i_rs485++;
        } else {
           banRSI = 0;
           banRSC = 1;
        }
     }

     if ((banRSI==0)&&(banRSC==0)){
        if (byteRS485==RS485_BYTE_INICIO){
           banRSI = 1;
           i_rs485 = 0;
        }
     }
     if ((banRSI==1)&&(i_rs485<5)){
        tramaCabeceraRS485[i_rs485] = byteRS485;
        i_rs485++;
     }
     if ((banRSI==1)&&(i_rs485==5)){
        if ((tramaCabeceraRS485[1]==IDNODO)||(tramaCabeceraRS485[1]==RS485_DIR_BROADCAST)){
           funcionRS485 = tramaCabeceraRS485[2];
           *(ptrnumDatosRS485) = tramaCabeceraRS485[3];
           *(ptrnumDatosRS485+1) = tramaCabeceraRS485[4];
           banRSI = 2;
           i_rs485 = 0;
        } else {
           //Trama dirigida a otro nodo: se descarta y se silencia el UART brevemente
           //(evita colisionar con la respuesta que el nodo destinatario pueda emitir).
           banRSI = 0;
           banRSC = 0;
           i_rs485 = 0;
           T2CON.TON = 1;
           TMR2 = 0;
           contTMR2 = 0;
           U1MODE.UARTEN = 0;
        }
     }

     if (banRSC==1){
        subFuncionRS485 = inputPyloadRS485[0];
        switch (funcionRS485){

               case 0xF1:
                    //Funcion de tiempo (recibida del Concentrador):
                    if (subFuncionRS485==0xD1){
                        for (x=0;x<6;x++) tiempo[x] = inputPyloadRS485[x+1];
                        horaSistema = RecuperarHoraRPI(tiempo);
                        fechaSistema = RecuperarFechaRPI(tiempo);
                        fuenteReloj = inputPyloadRS485[7];
                        banSetReloj = 1;                                        //A partir de aqui INT_SINC ya incrementa el reloj local
                    }
                    if (subFuncionRS485==0xD2){
                        outputPyloadRS485[0] = 0xD2;
                        for (x=0;x<6;x++) outputPyloadRS485[x+1] = tiempo[x];
                        outputPyloadRS485[7] = fuenteReloj;
                        delay_ms(10);
                        EnviarTramaRS485(RS485_PUERTO_UART1, IDNODO, 0xF1, 8, outputPyloadRS485);
                    }
                    break;

               case 0xF2:
                    //Funcion de muestreo:
                    if ((subFuncionRS485==0xD1)&&(banInicioMuestreo==0)){
                        sectorSD = UbicarUltimoSectorEscrito(inputPyloadRS485[1]); //[1]=sobrescribir (0=no,1=si)
                        PSEC = sectorSD;
                        GuardarInfoSector(PSEC, infoPrimerSector);
                        banInicioMuestreo = 1;
                    }
                    if ((subFuncionRS485==0xD2)&&(banInicioMuestreo==1)){
                       GuardarInfoSector(sectorSD, infoUltimoSector);
                       banInicioMuestreo = 0;
                    }
                    break;

               case 0xF3:
                    //Funcion de analisis y lectura:
                    *ptrsectorReq = inputPyloadRS485[1];
                    *(ptrsectorReq+1) = inputPyloadRS485[2];
                    *(ptrsectorReq+2) = inputPyloadRS485[3];
                    *(ptrsectorReq+3) = inputPyloadRS485[4];

                    if (subFuncionRS485==0xD1){
                       InformacionSectores();
                    }
                    if (subFuncionRS485==0xD2){
                       if (banInicioMuestreo==0){
                          InspeccionarSector(0, sectorReq);
                       } else {
                          banInsSec = 1;                                        //Se atiende al cerrar el ciclo de muestreo en curso
                       }
                    }
                    if (subFuncionRS485==0xD3){
                       if (banInicioMuestreo==0){
                          RecuperarTramaAceleracion(sectorReq);
                       }
                    }
                    break;

        }

        banRSC = 0;
        banRSI = 0;

     }

}
//*****************************************************************************************************************************************
