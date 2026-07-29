                                                                 /*-------------------------------------------------------------------------------------------------------------------------
Autor: Milton Munoz, email: miltonrodrigomunoz@gmail.com
Fecha de creacion: 16/01/2020
Configuracion: dsPIC33EP256MC202, XT=80MHz
---------------------------------------------------------------------------------------------------------------------------*/

////////////////////////////////////////////////////         Librerias         /////////////////////////////////////////////////////////////

#include <ADXL355_SPI.c>
#include <TIEMPO_RTC.c>
#include <RS485.c>
#include <TIEMPO_RPI.c>

#include <spiSD.h>
#include <sdcard.h>
#include <stdbool.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Credenciales:
#define IDNODO 1                                                                //Direccion del nodo
#define SIZESD 8                                                                //Capacidad de la SD (GB)
#define DELTASECTOR 97952                                                       //Desface donde empiezan los datos del sector a partir del PSF (sector 100000)

////////////////////////////////////////////// Declaracion de variables y costantes ///////////////////////////////////////////////////////
//Constantes:
#define FP 80000000                                                             //Frecuencia del reloj

//Subindices:
unsigned int i, j, x, y;

//Definicion de pines:
struct sdflags sdflags;                                                         //Variable y estructura que se utiliza en el archivo sdcard.c
sbit TEST at LATA2_bit;                                                         //Definicion del pin TEST
sbit TEST_Direction at TRISA2_bit;
sbit CsADXL at LATA3_bit;                                                       //Definicion del pin CS del Acelerometro
sbit CsADXL_Direction at TRISA3_bit;
sbit sd_CS_lat at LATB0_bit;                                                    //ChipSelect SD
sbit sd_CS_tris at TRISB0_bit;
sbit sd_detect_port at LATA4_bit;                                               //PinDetection SD
sbit sd_detect_tris at TRISA4_bit;
sbit MSRS485 at LATB12_bit;                                                     //Definicion del pin MS RS485
sbit MSRS485_Direction at TRISB12_bit;

//Variables para controlar la operacion del sistema:
unsigned short inicioSistema;

//Variables para manejo del tiempo:
unsigned short tiempo[6] = {0, 0, 0, 0, 0, 0};                                  //Vector de datos de tiempo del sistema
unsigned short banSetReloj;
unsigned short fuenteReloj;                                                     //Indica la fuente de reloj: 1=GPS, 2=RTC, 3=RPi
unsigned long horaSistema, fechaSistema;

//Variables para manejo del acelerometro:
unsigned short banCiclo, banInicioMuestreo;
unsigned char datosLeidos[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char datosFIFO[243];                                                   //Vector para almacenar 27 muestras de 3 ejes del vector FIFO
unsigned char tramaAceleracion[2500];
unsigned short numFIFO, numSetsFIFO;                                            //Variablea para almacenar el numero de muestras y sets recuperados del buffer FIFO
unsigned short contTimer1;                                                      //Variable para contar el numero de veces que entra a la interrupcion por Timer 1
//unsigned short buffer;
unsigned short contMuestras;
unsigned short contCiclos;
unsigned int contFIFO;
short tasaMuestreo;
short numTMR1;

//Variables para manejo del RS485:
unsigned long BAUDRATE2, BRGVAL2;                                               //Variables para el calculo del baudrate
unsigned short banRSI, banRSC;                                                  //Banderas de control de inicio de trama y trama completa
unsigned char byteRS485;
unsigned int i_rs485;                                                           //Indice
unsigned char tramaCabeceraRS485[10];                                            //Vector para almacenar los datos de cabecera de la trama RS485: [0x3A, Direccion, Funcion, NumeroDatos]
unsigned char inputPyloadRS485[15];                                             //Vector para almacenar el pyload de entrada de la trama RS485
unsigned char outputPyloadRS485[15];                                            //Vector para almacenar el pyload de salida de la trama RS485
unsigned int numDatosRS485;                                                     //Numero de datos en el pyload de la trama RS485
unsigned char *ptrnumDatosRS485;
unsigned short funcionRS485;                                                    //Funcion requerida: 0xF1 = Muestrear, 0xF2 = Actualizar tiempo, 0xF3 = Probar comunicacion
unsigned short subFuncionRS485;                                                 //Sub funcion requerida: 0xD1, 0xD2, 0xD3  (Depende de la funcion)
unsigned char tramaPruebaRS485[10]= {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};   //Trama de 10 elementos para probar la comunicacion RS485
unsigned char *ptrsectorReq;                                                    //Puntero sector requerido
unsigned long sectorReq;                                                        //Variable para recuperar el sector requerido
unsigned short contTMR2;

//Variables para manejo del SD:
unsigned long PSF;                                                              //Primer Sector Fisico
unsigned long PSE;                                                              //Primer Sector de Escritura de la SD
unsigned long USF;                                                              //Ultimo Sector Fisico
unsigned long PSEC;                                                             //Primer sector escrito en el ciclo de muestreo                                                                   //Primer sector escrito en el ciclo actual de muestreo
unsigned long sectorSD;                                                         //Variable para almacenar el numero del sector de la SD que se va a escribir
unsigned long sectorLec;                                                        //Variable para recuperar la informacion del sector de la SD que se solicita leer
const unsigned int clusterSizeSD = 512;                                         //Tamaño del cluster de la SD de 512 bytes
unsigned long infoPrimerSector;                                                 //Sector de la SD donde se graba el primer sector escrito el ciclo de muestreo
unsigned long infoUltimoSector;                                                 //Sector de la SD donde se graba el ultimo sector escrito
unsigned char cabeceraSD[6] = {255, 253, 251, 10, 0, 250};                      //Cabecera del bufferSD: | Cte1 | Cte2 | Ct3 | #Bytes/Muestra | MSB_fSample | LSB_fSample |
unsigned char bufferSD [clusterSizeSD];                                         //Buffer del tamaño del cluster, siempre se guarda este numero de datos en la SD
unsigned char checkEscSD;                                                       //Esta variable indica si la escritura en la SD se completo correctamente
unsigned char checkLecSD;
unsigned short banInsSec;                                                       //Bandera para inspeccionar un sector de la SD

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////      Main      ////////////////////////////////////////////////////////////////
void main() {

     ConfiguracionPrincipal();
     TEST = 0;                                                                                                                                        //Pin de TEST

     tasaMuestreo = 1;                                                          //1=250Hz, 2=125Hz, 4=62.5Hz, 8=31.25Hz
     ADXL355_init(tasaMuestreo);                                                //Inicializa el modulo ADXL con la tasa de muestreo requerida:
     numTMR1 = (tasaMuestreo*10)-1;                                             //Calcula el numero de veces que tienen que desbordarse el TMR1 para cada tasa de muestreo

     //Inicializacion de variables:

     //Subindices:
     i = 0;
     j = 0;
     x = 0;
     y = 0;

     //Control sistema:
     inicioSistema = 0;

     //Tiempo:
     banSetReloj = 0;
     horaSistema = 0;
     fechaSistema = 0;
     fuenteReloj = 2;

     //Acelerometro:
     banCiclo = 0;
     banInicioMuestreo = 0;
     numFIFO = 0;
     numSetsFIFO = 0;
     contTimer1 = 0;
     contMuestras = 0;
     contCiclos = 0;
     contFIFO = 0;

     //RS485:
     banRSI = 0;
     banRSC = 0;
     byteRS485 = 0;
     i_rs485 = 0;
     funcionRS485 = 0;
     subFuncionRS485 = 0;
     numDatosRS485 = 0;
     ptrnumDatosRS485 = (unsigned char *) & numDatosRS485;
     ptrsectorReq = (unsigned char *) & sectorReq;
     contTMR2 = 0;

     //SD:
     PSEC = 0;
     sectorSD = 0;
     sectorLec = 0;
     checkEscSD = 0;
     checkLecSD = 0;
     MSRS485 = 0;                                                               //Establece el Max485 en modo lectura
     banInsSec = 0;

    //Determina el ultimo sector fisico en funcion de la capacidad de la SD:
     switch (SIZESD){
            case 2:
                    PSF = 2048;
                    USF = 3911679;
                    break;
            case 4:
                    PSF = 2048;
                    USF = 7772160;
                    break;
            case 8:
                    PSF = 2048;
                    //USF = 15265792;
                    USF = 16779263;
                    break;
            case 16:
                    PSF = 2048;
                    USF = 31115263;
                    break;
     }
     infoPrimerSector = PSF+DELTASECTOR-2;                                      //Calcula el sector donde se alamcena la informacion del primer sector escrito
     infoUltimoSector = PSF+DELTASECTOR-1;                                      //Calcula el sector donde se alamcena la informacion del ultimo sector escrito
     PSE = PSF+DELTASECTOR;                                                     //Calcula el primer sector de escritura

     //datos de tiempo de prueba
     //horaSistema = 86100;        //23:55:00
     //fechaSistema = 200228;      //aa/mm/dd

     //Comprueba si esta conectada la SD:
     while (1) {
           if (SD_Detect() == DETECTED) {
              //En el caso de que este conectada activa la bandera
              sdflags.detected = true;
              //TEST = 1;
              break;
           } else {
              // Caso contrario si no esta conectada la SD, coloca las banderas en false
              sdflags.detected = false;
              sdflags.init_ok = false;
              //TEST = 0;
           }
           Delay_ms(100);
     }

     //Descomentar esta parte si el socalo no tiene el pin para detectar la SD:
     //sdflags.detected = true;

     //Inicializa la SD:
     if (sdflags.detected && !sdflags.init_ok) {
          if (SD_Init_Try(10) == SUCCESSFUL_INIT) {
              sdflags.init_ok = true;
              inicioSistema = 1;                                                //Activa la bandera para permitir el inicio del sistema
              TEST = 1;
           } else {
              sdflags.init_ok = false;
              INT1IE_bit = 0;                                                   //Desabilita la interrupcion externa INT1
              U1MODE.UARTEN = 0;                                                //Desabilita el UART
              inicioSistema = 0;                                                //Apaga la bandera de inicio del sistema
              TEST = 0;
           }
     }
     Delay_ms(2000);

     //Entra al bucle princial del programa:
     while(1){
            asm CLRWDT;         //Clear the watchdog timer
            Delay_ms(100);
     }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////// Funciones ////////////////////////////////////////////////////////////////

//*****************************************************************************************************************************************
// Funcion para realizar la configuracion principal
void ConfiguracionPrincipal(){

     //configuracion del oscilador:                                             //FPLLO = FIN*(M/(N1+N2)) = 80.017MHz
     CLKDIVbits.FRCDIV = 0;                                                     //FIN=FRC/1
     CLKDIVbits.PLLPOST = 0;                                                    //N2=2
     CLKDIVbits.PLLPRE = 5;                                                     //N1=7
     PLLFBDbits.PLLDIV = 150;                                                   //M=152

     //Configuracion de puertos:
     ANSELA = 0;                                                                //Configura PORTA como digital     *
     ANSELB = 0;                                                                //Configura PORTB como digital     *
     TEST_Direction = 0;                                                        //TEST
     CsADXL_Direction = 0;                                                      //CS ADXL
     sd_CS_tris = 0;                                                            //CS SD
     MSRS485_Direction = 0;                                                     //MAX485 MS
     sd_detect_tris = 1;                                                        //Pin detection SD
     TRISB14_bit = 1;                                                           //Pin de interrupcion V1
     //TRISB13_bit = 1;                                                           //Pin de interrupcion V2

     //Habilita las interrupciones globales:
     INTCON2.GIE = 1;                                                           //Habilita las interrupciones globales

     //Configuracion del puerto UART1:
     RPINR18bits.U1RXR = 0x2F;                                                  //Configura el pin RB15/RPI47 como Rx1
     RPOR1bits.RP36R = 0x01;                                                    //Configura el Tx1 en el pin RB4/RP36
     U1RXIE_bit = 1;                                                            //Activa la interrupcion por UART1 RX
     U1STAbits.URXISEL = 0x00;                                                  //Interrupt is set when any character is received and transferred from the UxRSR to the receive buffer; receive buffer has one or more characters
     U1RXIF_bit = 0;                                                            //Limpia la bandera de interrupcion por UART1 RX
     IPC2bits.U1RXIP = 0x04;                                                    //Prioridad de la interrupcion UART1 RX
     UART1_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED);                            //Inicializa el UART1 con una velocidad de 2Mbps

     //Configuracion del puerto SPI2 en modo Master:
     RPINR22bits.SDI2R = 0x21;                                                  //Configura el pin RB1/RPI33 como SDI2 *
     RPOR2bits.RP38R = 0x08;                                                    //Configura el SDO2 en el pin RB6/RP38 *
     RPOR1bits.RP37R = 0x09;                                                    //Configura el SCK2 en el pin RB5/RP37 *
     SPI2STAT.SPIEN = 1;                                                        //Habilita el SPI2 *
     SPI2_Init();                                                               //Inicializa el modulo SPI2

     //Configuracion de la interrupcion externa INT1
     RPINR0 = 0x2E00;                                                           //Asigna INT1 al RB14/RPI46 V1
     //RPINR0 = 0x2D00;                                                           //Asigna INT1 al RB14/RPI46 V2
     INT1IE_bit = 1;                                                            //Interrupcion externa INT1
     INT1IF_bit = 0;                                                            //Limpia la bandera de interrupcion externa INT1
     IPC5bits.INT1IP = 0x01;                                                    //Prioridad en la interrupocion externa 1

     //Configuracion del TMR1 con un tiempo de 100ms
     T1CON = 0x0020;
     T1CON.TON = 0;                                                             //Apaga el Timer1
     T1IE_bit = 1;                                                              //Habilita la interrupción de desbordamiento TMR1
     T1IF_bit = 0;                                                              //Limpia la bandera de interrupcion del TMR1
     PR1 = 62500;                                                               //Car ga el preload para un tiempo de 100ms
     IPC0bits.T1IP = 0x02;                                                      //Prioridad de la interrupcion por desbordamiento del TMR1

     //Configuracion del TMR2 con un tiempo de 300ms
     T2CON = 0x0030;                                                            //Prescalador
     T2CON.TON = 0;                                                             //Apaga el Timer2
     T2IE_bit = 1;                                                              //Habilita la interrupción de desbordamiento TMR2
     T2IF_bit = 0;                                                              //Limpia la bandera de interrupcion del TMR2
     PR2 = 46875;                                                               //Carga el preload para un tiempo de 300ms
     IPC1bits.T2IP = 0x02;                                                      //Prioridad de la interrupcion por desbordamiento del TMR2

     //Configuracion del acelerometro:
     ADXL355_write_byte(POWER_CTL, DRDY_OFF|STANDBY);                           //Coloco el ADXL en modo STANDBY para pausar las conversiones y limpiar el FIFO

     //Limpia las banderas de la SD:
     sdflags.detected = false;
     sdflags.init_ok = false;
     sdflags.saving = false;

     Delay_ms(200);                                                             //Espera hasta que se estabilicen los cambios

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Funcion para relizar el muesteo
void Muestrear(){

     if (banCiclo==0){

         ADXL355_write_byte(POWER_CTL, DRDY_OFF|MEASURING);                     //Coloca el ADXL en modo medicion
         T1CON.TON = 1;                                                         //Enciende el Timer1
         TMR1 = 0;                                                              //Encera el Timer1

     } else if (banCiclo==1) {

         banCiclo = 2;                                                          //Limpia la bandera de ciclo completo

         tramaAceleracion[0] = fuenteReloj;                                     //LLena el primer elemento de la tramaCompleta con la fuente de reloj
         numFIFO = ADXL355_read_byte(FIFO_ENTRIES);
         numSetsFIFO = (numFIFO)/3;                                             //Lee el numero de sets disponibles en el FIFO

         //Este bucle recupera tantos sets de mediciones del buffer FIFO como indique la variable anterior
         for (x=0;x<numSetsFIFO;x++){
             ADXL355_read_FIFO(datosLeidos);                                    //Lee una sola posicion del FIFO
             for (y=0;y<9;y++){
                 datosFIFO[y+(x*9)] = datosLeidos[y];                           //LLena la trama datosFIFO
             }
         }

         //Este bucle rellena la trama completa intercalando el numero de muestra correspondientes
         for (x=0;x<(numSetsFIFO*9);x++){
             if ((x==0)||(x%9==0)){
                tramaAceleracion[contFIFO+contMuestras+x] = contMuestras;          //Funciona bien
                tramaAceleracion[contFIFO+contMuestras+x+1] = datosFIFO[x];
                contMuestras++;
             } else {
                tramaAceleracion[contFIFO+contMuestras+x] = datosFIFO[x];
             }
         }

         contMuestras = 0;                                                      //Limpia el contador de muestras
         contFIFO = 0;                                                          //Limpia el contador de FIFOs
         T1CON.TON = 1;                                                         //Enciende el Timer1
         TMR1 = 0;                                                              //Encera el Timer1

         //Guarda la trama completa de aceleracion en la SD:
         GuardarTramaSD(tiempo, tramaAceleracion);

         //Si hay una solicitud pendiente inspecciona el sector de la SD:
         if (banInsSec==1){
            InspeccionarSector(1, sectorReq);
         }

     }

     contCiclos++;                                                              //Incrementa el contador de ciclos

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Funcion para guardar el buffer de 512 datos en la SD
void GuardarBufferSD(unsigned char* bufferLleno, unsigned long sector){
     // Intenta escribir los datos en la SD como maximo 5 veces:
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
//Funcion para guardar los datos de la trama de aceleracion en la SD
void GuardarTramaSD(unsigned char* tiempoSD, unsigned char* aceleracionSD){


        //**Informacion: [Cabecera + tiempo + datos aceleracion] = 2512 bytes = 5 sectores
        //Por lo tanto se graban 4 buffers completos y 1 un buffer con 464 bytes de aceleracion y 48 ceros

        //Agrega los datos de cabecera al buffer:
        for (x=0;x<6;x++){
            bufferSD[x] = cabeceraSD[x];
        }
        //Agrega los datos de tiempo al buffer:
        for (x=0;x<6;x++){
            bufferSD[6+x] = tiempoSD[x];
        }
        //Agrega los primeros 500 bytes de la trama de datos al buffer:
        for (x=0;x<500;x++){
            bufferSD[12+x] = aceleracionSD[x];
        }
        //Guarda el buffer en la SD:
        GuardarBufferSD(bufferSD, sectorSD);
        //Aumenta el indice del sector:
        sectorSD++;

        //Guarda en la SD los bytes de la trama de datos desde la posicion 500 - 1011:
        for (x=0;x<512;x++){
            bufferSD[x] = aceleracionSD[x+500];
        }
        GuardarBufferSD(bufferSD, sectorSD);
        sectorSD++;

        //Guarda en la SD los bytes de la trama de datos desde la posicion 1012 - 1523:
        for (x=0;x<512;x++){
            bufferSD[x] = aceleracionSD[x+1012];
        }
        GuardarBufferSD(bufferSD, sectorSD);
        sectorSD++;

        //Guarda en la SD los bytes de la trama de datos desde la posicion 1524 - 2035:
        for (x=0;x<512;x++){
            bufferSD[x] = aceleracionSD[x+1524];
        }
        GuardarBufferSD(bufferSD, sectorSD);
        sectorSD++;

        //Guarda en la SD los bytes de la trama de datos desde la posicion 2036 - 2499:
        for (x=0;x<512;x++){
            if (x<464){
               bufferSD[x] = aceleracionSD[x+2036];
            } else {
               bufferSD[x] = 0;
            }
        }
        GuardarBufferSD(bufferSD, sectorSD);
        sectorSD++;

        //Guarda en la SD la posicion del ultimo sector escrito cada 5 minutos:
        if (horaSistema%300==0){
           GuardarInfoSector(sectorSD, infoUltimoSector);
        }

        TEST = 0;                                                               //Apaga el TEST cuando termina de gurdar la trama

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Funcion para guardar el ultimo sector que se escribio
void GuardarInfoSector(unsigned long datoSector, unsigned long localizacionSector){

     //Crea un buffer para pasarle a la funcion que graba la informacion en la SD por sectores
     //Es muy ineficiente pero no quiero manipular la libreria
     unsigned char bufferSectores[512];
     bufferSectores[0] = (datoSector>>24)&0xFF;                                     //MSB variable sector
     bufferSectores[1] = (datoSector>>16)&0xFF;
     bufferSectores[2] = (datoSector>>8)&0xFF;
     bufferSectores[3] = (datoSector)&0xFF;                                         //LSD variable sector
     for (x=4;x<512;x++){
         bufferSectores[x] = 0;                                                 //Rellena de ceros el resto del buffer
     }

     // Intenta escribir los datos en la SD como maximo 5 veces:
     for (x=0;x<5;x++){
         checkEscSD = SD_Write_Block(bufferSectores,localizacionSector);
         if (checkEscSD == DATA_ACCEPTED){
             //TEST = ~TEST;
             break;
         }
         Delay_us(10);
     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Funcion para leer el primer sector que se escribio
unsigned long UbicarPrimerSectorEscrito(){

     unsigned char bufferSectorInicio[512];                                     //Trama para guardar los datos del sector leido
     unsigned long primerSectorSD;                                              //Variable donde se almacena el valor del sector inicial
     unsigned char *ptrPrimerSectorSD;

     ptrPrimerSectorSD = (unsigned char *) & primerSectorSD;

     checkLecSD = 1;
     // Intenta leer los datos del sector como maximo 5 veces:
     for (x=0;x<5;x++){
         //Lee los datos del sector donde se almaceno el dato del ultimo sector escrito:
         checkLecSD = SD_Read_Block(bufferSectorInicio, infoPrimerSector);
         //checkLecSD = 0, significa que la lectura fue exitosa:
         if (checkLecSD==0) {
            //Almacena el datos en la variable sectorInicioSD:
            *ptrPrimerSectorSD = bufferSectorInicio[3];                      //LSB
            *(ptrPrimerSectorSD+1) = bufferSectorInicio[2];
            *(ptrPrimerSectorSD+2) = bufferSectorInicio[1];
            *(ptrPrimerSectorSD+3) = bufferSectorInicio[0];                  //MSB
            break;
            Delay_ms(5);
         } else {
            primerSectorSD = PSE;                                           //Si no pudo realizar la lectura devuelve el Primer Sector de Escritura
         }
     }


     return primerSectorSD;

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Funcion para leer el ultimo sector que se escribio
unsigned long UbicarUltimoSectorEscrito(unsigned short sobrescribirSD){

     unsigned char bufferSectorFinal[512];                                      //Trama para guardar los datos del sector leido
     unsigned long sectorInicioSD;                                              //Variable donde se almacena el valor del sector inicial
     unsigned char *ptrSectorInicioSD;

     ptrSectorInicioSD = (unsigned char *) & sectorInicioSD;

     //Si sobrescribirSD = True sobrescribe la SD:
     if (sobrescribirSD==1){
         sectorInicioSD = PSE;                                                  //Se escoje el PSE para sobrescribir la SD
     } else {
         checkLecSD = 1;
         // Intenta leer los datos del sector como maximo 5 veces:
         for (x=0;x<5;x++){
             //Lee los datos del sector donde se almaceno el dato del ultimo sector escrito:
             checkLecSD = SD_Read_Block(bufferSectorFinal, infoUltimoSector);
             //checkLecSD = 0, significa que la lectura fue exitosa:
             if (checkLecSD==0) {
                //Almacena el datos en la variable sectorInicioSD:
                *ptrSectorInicioSD = bufferSectorFinal[3];                      //LSB
                *(ptrSectorInicioSD+1) = bufferSectorFinal[2];
                *(ptrSectorInicioSD+2) = bufferSectorFinal[1];
                *(ptrSectorInicioSD+3) = bufferSectorFinal[0];                  //MSB
                break;
                Delay_ms(5);
             } else {
                sectorInicioSD = PSE;                                           //Si no pudo realizar la lectura procede a sobreescribir la SD
             }
         }
     }

     return sectorInicioSD;

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Funcion para recuperar informacion de los sectores
void InformacionSectores(){

     unsigned char tramaInfoSec[20];

     unsigned long infoPSF;
     unsigned long infoPSE;
     unsigned long infoPSEC;
     unsigned long infoSA;

     unsigned char *ptrPSF;                                                     //Puntero primer sector fisico
     unsigned char *ptrPSE;                                                     //Puntero primer sector de escritura
     unsigned char *ptrPSEC;                                                    //Puntero primer sector escrito en el ciclo actual
     unsigned char *ptrSA;                                                      //Puntero sector actual

     infoPSF = PSF;
     infoPSE = PSE;

     //Asociacion de los punteros a las variables:
     ptrPSF = (unsigned char *) & infoPSF;
     ptrPSE = (unsigned char *) & infoPSE;
     ptrPSEC = (unsigned char *) & infoPSEC;
     ptrSA = (unsigned char *) & infoSA;

     //Revisa si hay un proceso de muestreo en marcha:
     if (banInicioMuestreo==0){
        infoPSEC = UbicarPrimerSectorEscrito();                                 //Calcula el primer sector escrito
        infoSA = UbicarUltimoSectorEscrito(0);                                  //Calcula el ultimo sector escrito
     } else {
        infoSA = sectorSD - 1;                                                  //Retorna el sector actual menos una posicion
        infoPSEC = PSEC;                                                        //Retorna el primer sector escrito en este ciclo de muestreo
     }

     tramaInfoSec[0] = 0xD1;                                                    //Subfuncion
     tramaInfoSec[1] = *ptrPSF;                                                 //LSB PSF
     tramaInfoSec[2] = *(ptrPSF+1);
     tramaInfoSec[3] = *(ptrPSF+2);
     tramaInfoSec[4] = *(ptrPSF+3);                                             //MSB PSF
     tramaInfoSec[5] = *ptrPSE;                                                 //LSB PSE
     tramaInfoSec[6] = *(ptrPSE+1);
     tramaInfoSec[7] = *(ptrPSE+2);
     tramaInfoSec[8] = *(ptrPSE+3);                                             //MSB PSE
     tramaInfoSec[9] = *ptrPSEC;                                                //LSB PSEC
     tramaInfoSec[10] = *(ptrPSEC+1);
     tramaInfoSec[11] = *(ptrPSEC+2);
     tramaInfoSec[12] = *(ptrPSEC+3);                                           //MSB PSEC
     tramaInfoSec[13] = *ptrSA;                                                 //LSB SA
     tramaInfoSec[14] = *(ptrSA+1);
     tramaInfoSec[15] = *(ptrSA+2);
     tramaInfoSec[16] = *(ptrSA+3);                                             //MSB SA

     delay_ms(10);
     EnviarTramaRS485(1, IDNODO, 0xF3, 17, tramaInfoSec);

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Funcion para inspeccionar los datos de un sector
void InspeccionarSector(unsigned short estadoMuestreo, unsigned long sectorReq){

     unsigned char tramaDatosSec[15];
     unsigned char bufferSectorReq[512];                                        //Trama para recuperar el buffer leido
     unsigned int numDatosSec;
     unsigned int contadorSector;
     unsigned long USE;

     //Calcula el ultimo sector escrito:
     if (estadoMuestreo==0){
        USE = UbicarUltimoSectorEscrito(0);
     } else {
        USE = sectorSD - 1;
     }

     tramaDatosSec[0] = 0xD2;                                                   //Subfuncion

     //Comprueba que el sector requerido este dentro del rango de sectores permitidos:
     if ((sectorReq>=PSE)&&(sectorReq<USF)){
         //Comprueba que sector requerido sea menor al ultimo sector escrito:
         if (sectorReq<USE){
             checkLecSD = 1;
             // Intenta leer los datos del sector como maximo 5 veces:
             for (x=0;x<5;x++){
                 //Lee los datos del sector donde se almaceno el dato del ultimo sector escrito:
                 checkLecSD = SD_Read_Block(bufferSectorReq, sectorReq);
                 //checkLecSD = 0, significa que la lectura fue exitosa:
                 if (checkLecSD==0) {
                    //Almacena el datos en la variable sectorInicioSD:
                    numDatosSec = 14;
                    for (y=0;y<numDatosSec;y++){
                        tramaDatosSec[y+1] = bufferSectorReq[y];
                    }
                    break;
                } else {
                    //Indica el error E3: Error al leer la SD
                    numDatosSec = 3;
                    tramaDatosSec[1] = 0xEE;
                    tramaDatosSec[2] = 0xE3;
                }
                Delay_us(10);
            }
        } else {
            //Indica el error E2: Sector vacio
            numDatosSec = 3;
            tramaDatosSec[1] = 0xEE;
            tramaDatosSec[2] = 0xE2;
        }

    } else {

        //Indica el error E1: Sector fuera de rango
        numDatosSec = 3;
        tramaDatosSec[1] = 0xEE;
        tramaDatosSec[2] = 0xE1;

    }

    banInsSec = 0;
    delay_ms(10);
    EnviarTramaRS485(1, IDNODO, 0xF3, numDatosSec, tramaDatosSec);

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Funcion para recuperar un segundo de datos de aceleracion
void RecuperarTramaAceleracion(unsigned long sectorReq){

    unsigned char tramaAcelSeg[2515];                                           //Trama para almacenar los datos del vector de aceleracion
    unsigned char bufferSectorReq[512];                                         //Trama para recuperar el buffer leido
    unsigned short tiempoAcel[6];                                               //Trama de tiempo del vector de aceleracion
    unsigned long contSector;
    unsigned int numDatosTramaAcel;
    unsigned short banLecturaCorrecta;

    tramaAcelSeg[0] = 0xD3;                                                     //Subfuncion
    contSector = 0;
    banLecturaCorrecta = 0;
    numDatosTramaAcel = 2513;

    //Lee el primer sector:
    checkLecSD = 1;
    // Intenta leer los datos del sector como maximo 5 veces:
    for (x=0;x<5;x++){
        checkLecSD = SD_Read_Block(bufferSectorReq, (sectorReq+contSector));
        if (checkLecSD==0) {
            //Recupera los datos de tiempo:
            for (y=0;y<6;y++){
                tiempoAcel[y] = bufferSectorReq[y+6];
            }
            //Recupera los datos de cabecera:
            for (y=0;y<6;y++){
                tramaAcelSeg[y+1] = bufferSectorReq[y];
            }
            //Recupera los primeros 500 bytes de aceleracion:
            for (y=0;y<500;y++){
                tramaAcelSeg[y+7] = bufferSectorReq[y+12];
            }
            banLecturaCorrecta = 1;
            contSector++;
            break;
        } else {
            //Indica el error E3: Error al leer la SD
            tramaAcelSeg[1] = 0xEE;
            tramaAcelSeg[2] = 0xE3;
            numDatosTramaAcel = 3;
            banLecturaCorrecta = 2;
        }
        Delay_us(10);
    }

    //Lee el segundo sector:
    if (banLecturaCorrecta==1){
        checkLecSD = 1;
        // Intenta leer los datos del sector como maximo 5 veces:
        for (x=0;x<5;x++){
            checkLecSD = SD_Read_Block(bufferSectorReq, (sectorReq+contSector));
            if (checkLecSD==0) {
                //Recupera los siguientes 512 bytes de aceleracion (500 - 1011):
                for (y=0;y<512;y++){
                    tramaAcelSeg[y+507] = bufferSectorReq[y];
                }
                banLecturaCorrecta = 1;
                contSector++;
                break;
            } else {
                //Indica el error E3: Error al leer la SD
                tramaAcelSeg[1] = 0xEE;
                tramaAcelSeg[2] = 0xE3;
                numDatosTramaAcel = 3;
                banLecturaCorrecta = 2;
            }
            Delay_us(10);
        }
    }

    //Lee el tercer sector:
    if (banLecturaCorrecta==1){
        checkLecSD = 1;
        // Intenta leer los datos del sector como maximo 5 veces:
        for (x=0;x<5;x++){
            checkLecSD = SD_Read_Block(bufferSectorReq, (sectorReq+contSector));
            if (checkLecSD==0) {
                //Recupera los siguientes 512 bytes de aceleracion (1012 - 1523):
                for (y=0;y<512;y++){
                    tramaAcelSeg[y+1019] = bufferSectorReq[y];
                }
                banLecturaCorrecta = 1;
                contSector++;
                break;
            } else {
                //Indica el error E3: Error al leer la SD
                tramaAcelSeg[1] = 0xEE;
                tramaAcelSeg[2] = 0xE3;
                numDatosTramaAcel = 3;
                banLecturaCorrecta = 2;
            }
            Delay_us(10);
        }
    }

    //Lee el cuarto sector:
    if (banLecturaCorrecta==1){
        checkLecSD = 1;
        // Intenta leer los datos del sector como maximo 5 veces:
        for (x=0;x<5;x++){
            checkLecSD = SD_Read_Block(bufferSectorReq, (sectorReq+contSector));
            if (checkLecSD==0) {
                //Recupera los siguientes 512 bytes de aceleracion (1524 - 2035):
                for (y=0;y<512;y++){
                    tramaAcelSeg[y+1531] = bufferSectorReq[y];
                }
                banLecturaCorrecta = 1;
                contSector++;
                break;
            } else {
                //Indica el error E3: Error al leer la SD
                tramaAcelSeg[1] = 0xEE;
                tramaAcelSeg[2] = 0xE3;
                numDatosTramaAcel = 3;
                banLecturaCorrecta = 2;
            }
            Delay_us(10);
        }
    }

    //Lee el quinto sector:
    if (banLecturaCorrecta==1){
        checkLecSD = 1;
        // Intenta leer los datos del sector como maximo 5 veces:
        for (x=0;x<5;x++){
            checkLecSD = SD_Read_Block(bufferSectorReq, (sectorReq+contSector));
            if (checkLecSD==0) {
                //Recupera los ultimos 464 bytes de aceleracion (2036 - 2499):
                for (y=0;y<464;y++){
                    tramaAcelSeg[y+2043] = bufferSectorReq[y];
                }
                banLecturaCorrecta = 1;
                break;
            } else {
                //Indica el error E3: Error al leer la SD
                tramaAcelSeg[1] = 0xEE;
                tramaAcelSeg[2] = 0xE3;
                numDatosTramaAcel = 3;
                banLecturaCorrecta = 2;
            }
            Delay_us(10);
        }
    }

    //Agrega la trama de tiempo al final de la trama de aceleracion (Para conincidir con el formato de datos anterior):
    if (banLecturaCorrecta==1){
        for (x=0;x<6;x++){
            tramaAcelSeg[2507+x] = tiempoAcel[x];
        }
        TEST = ~TEST;
    }

    //Envia la trama aceleracion por RS485:
    delay_ms(10);
    EnviarTramaRS485(1, IDNODO, 0xF3, numDatosTramaAcel, tramaAcelSeg);

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Funcion para guardar datos de prueba en la SD
void GuardarPruebaSD(unsigned char* tiempoSD){

         unsigned short contadorEjemploSD;
         unsigned char aceleracionSD[2506];

        //**Informacion: [Cabecera + tiempo + datos aceleracion] = 2512 bytes = 5 sectores
        //Por lo tanto se graban 4 buffers completos y 1 un buffer con 464 bytes de aceleracion y 48 ceros

        //DATOS DE PRUEBA:
        contadorEjemploSD = 0;
        for (x=0;x<2500;x++){
            aceleracionSD[x] = contadorEjemploSD;
            contadorEjemploSD ++;
            if (contadorEjemploSD >= 255){
                contadorEjemploSD = 0;
            }
        }

        //Agrega los datos de cabecera al buffer:
        for (x=0;x<6;x++){
            bufferSD[x] = cabeceraSD[x];
        }
        //Agrega los datos de tiempo al buffer:
        for (x=0;x<6;x++){
            bufferSD[6+x] = tiempoSD[x];
        }
        //Agrega los primeros 500 bytes de la trama de datos al buffer:
        for (x=0;x<500;x++){
            bufferSD[12+x] = aceleracionSD[x];
        }
        //Guarda el buffer en la SD:
        GuardarBufferSD(bufferSD, sectorSD);
        //Aumenta el indice del sector:
        sectorSD++;

        //Guarda en la SD los bytes de la trama de datos desde la posicion 500 - 1011:
        for (x=0;x<512;x++){
            bufferSD[x] = aceleracionSD[x+500];
        }
        GuardarBufferSD(bufferSD, sectorSD);
        sectorSD++;

        //Guarda en la SD los bytes de la trama de datos desde la posicion 1012 - 1523:
        for (x=0;x<512;x++){
            bufferSD[x] = aceleracionSD[x+1012];
        }
        GuardarBufferSD(bufferSD, sectorSD);
        sectorSD++;

        //Guarda en la SD los bytes de la trama de datos desde la posicion 1524 - 2035:
        for (x=0;x<512;x++){
            bufferSD[x] = aceleracionSD[x+1524];
        }
        GuardarBufferSD(bufferSD, sectorSD);
        sectorSD++;

        //Guarda en la SD los bytes de la trama de datos desde la posicion 2036 - 2499:
        for (x=0;x<512;x++){
            if (x<464){
               bufferSD[x] = aceleracionSD[x+2036];
            } else {
               bufferSD[x] = 0;
            }
        }
        GuardarBufferSD(bufferSD, sectorSD);
        sectorSD++;

        //Guarda en la SD la posicion del ultimo sector escrito cada 5 minutos:
        if (horaSistema%300==0){
           GuardarInfoSector(sectorSD, infoUltimoSector);
        }

        TEST = 0;                                                               //Apaga el TEST cuando termina de gurdar la trama

}


//*****************************************************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////// Interrupciones /////////////////////////////////////////////////////////////

//*****************************************************************************************************************************************
//Interrupcion INT1
void int_1() org IVT_ADDR_INT1INTERRUPT {

     INT1IF_bit = 0;                                                            //Limpia la bandera de interrupcion externa INT1

     //Sobrescribe el PSEC al cambio de dia (0 horas):
     if ((horaSistema==0)&&(banInicioMuestreo==1)){
        PSEC = sectorSD;
        GuardarInfoSector(PSEC, infoPrimerSector);
     }

     if (banSetReloj==1){
        horaSistema++;                                                          //Incrementa el reloj del sistema
        if (horaSistema==86400){                                                //(24*3600)+(0*60)+(0) = 86400
           horaSistema = 0;                                                     //Reinicia el reloj al llegar a las 24:00:00 horas
           fechaSistema = IncrementarFecha(fechaSistema);                       //Incrementa la fecha del sistema
        }
        AjustarTiempoSistema(horaSistema, fechaSistema, tiempo);                //Actualiza la trama de tiempo
        TEST = ~TEST;
     }

     if (banInicioMuestreo==1){
        Muestrear();                                                          //Inicia el muestreo
        //GuardarPruebaSD(tiempo);                                                //Muestreo con datos
     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Interrupcion por desbordamiento del Timer1
void Timer1Int() org IVT_ADDR_T1INTERRUPT{

     T1IF_bit = 0;                                                              //Limpia la bandera de interrupcion por desbordamiento del Timer1

     numFIFO = ADXL355_read_byte(FIFO_ENTRIES); //75                            //Lee el numero de muestras disponibles en el FIFO
     numSetsFIFO = (numFIFO)/3;                 //25                            //Lee el numero de sets disponibles en el FIFO

     //Este bucle recupera tantos sets de mediciones del buffer FIFO como indique la variable anterior
     for (x=0;x<numSetsFIFO;x++){
         ADXL355_read_FIFO(datosLeidos);                                        //Lee una sola posicion del FIFO
         for (y=0;y<9;y++){
             datosFIFO[y+(x*9)] = datosLeidos[y];                               //LLena la trama datosFIFO
         }
     }

     //Este bucle rellena la trama completa intercalando el numero de muestra correspondientes
     for (x=0;x<(numSetsFIFO*9);x++){      //0-224
         if ((x==0)||(x%9==0)){
            tramaAceleracion[contFIFO+contMuestras+x] = contMuestras;
            tramaAceleracion[contFIFO+contMuestras+x+1] = datosFIFO[x];
            contMuestras++;
         } else {
            tramaAceleracion[contFIFO+contMuestras+x] = datosFIFO[x];
         }
     }

     contFIFO = (contMuestras*9);                                               //Incrementa el contador de FIFOs

     contTimer1++;                                                              //Incrementa una unidad cada vez que entra a la interrupcion por Timer1

     if (contTimer1==numTMR1){                                                  //Verifica si se cumplio el numero de interrupciones por TMR1 para la tasa de muestreo seleccionada
        T1CON.TON = 0;                                                          //Apaga el Timer1
        banCiclo = 1;                                                           //Activa la bandera que indica que se completo un ciclo de medicion
        contTimer1 = 0;                                                         //Limpia el contador de interrupciones por Timer1
     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Interrupcion por desbordamiento del Timer2
void Timer2Int() org IVT_ADDR_T2INTERRUPT{

     T2IF_bit = 0;                                                              //Limpia la bandera de interrupcion por desbordamiento del Timer2
     contTMR2++;                                                                //Incrementa el contador de TMR2

     //Despues de 1200ms apaga el TMR2 y vuelve a encender el UART1:
     if (contTMR2==4){
         T2CON.TON = 0;
         TMR2 = 0;
         contTMR2 = 0;
         //Limpia estas banderas para restablecer la comunicacion por RS485:
         banRSI = 0;
         banRSC = 0;
         i_rs485 = 0;
         //Activa el UART1:
         UART1_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED); //**
     }

     //**Parece que cuando se usa el UARTx_Init_Advanced el U1MODE.UARTEN = 1 no es suficiente para encender el UART
}

//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Interrupcion UART1
void urx_1() org  IVT_ADDR_U1RXINTERRUPT {

     //Recupera el byte recibido en cada interrupcion:
     U1RXIF_bit = 0;                                                            //Limpia la bandera de interrupcion por UART
     byteRS485 = U1RXREG;
     OERR_bit = 0;                                                              //Limpia este bit para limpiar el FIFO UART

     //Recupera el pyload de la trama RS485:                                    //Aqui deberia entrar despues de recuperar la cabecera de trama
     if (banRSI==2){
        //Recupera el pyload mas 2 bytes de final de trama:
        if (i_rs485<(numDatosRS485)){
           inputPyloadRS485[i_rs485] = byteRS485;
           i_rs485++;
        } else {
           //T2CON.TON = 0;                                                       //Apaga el Timer2
           banRSI = 0;                                                          //Limpia la bandera de inicio de trama
           banRSC = 1;                                                          //Activa la bandera de trama completa
        }
     }

     //Recupera la cabecera de la trama RS485:                                  //Aqui deberia entrar primero cada vez que se recibe una trama nueva
     if ((banRSI==0)&&(banRSC==0)){
        if (byteRS485==0x3A){                                                   //Verifica si el primer byte recibido sea la cabecera de trama
           banRSI = 1;
           i_rs485 = 0;
        }
     }
     if ((banRSI==1)&&(i_rs485<5)){
        tramaCabeceraRS485[i_rs485] = byteRS485;                                //Recupera los datos de cabecera de la trama UART: [0x3A, Direccion, Funcion, NumeroDatosLSB, NumeroDatosMSB]
        i_rs485++;
     }
     if ((banRSI==1)&&(i_rs485==5)){
        //Comprueba la direccion:
        if ((tramaCabeceraRS485[1]==IDNODO)||(tramaCabeceraRS485[1]==255)){
           funcionRS485 = tramaCabeceraRS485[2];
           *(ptrnumDatosRS485) = tramaCabeceraRS485[3];                         //LSB numDatosRS485
           *(ptrnumDatosRS485+1) = tramaCabeceraRS485[4];                       //MSB numDatosRS485
           banRSI = 2;
           i_rs485 = 0;
        } else {
           banRSI = 0;
           banRSC = 0;
           i_rs485 = 0;
           T2CON.TON = 1;                                                       //Enciende el Timer2
           TMR2 = 0;                                                            //Encera el Timer2
           contTMR2 = 0;
           U1MODE.UARTEN = 0;                                                   //Desactiva el UART1
        }
     }

     //Realiza el procesamiento de la informacion del  pyload:                  //Aqui se realiza cualquier accion con el pyload recuperado
     if (banRSC==1){
        subFuncionRS485 = inputPyloadRS485[0];
        switch (funcionRS485){

               case 0xF1:
                    //***Funcion de tiempo***
                    //Recupera el tiempo de la trama RS485:
                    if (subFuncionRS485==0xD1){
                        for (x=0;x<6;x++) {
                            tiempo[x] = inputPyloadRS485[x+1];                       //LLena la trama tiempo con el payload de la trama recuperada
                        }
                        horaSistema = RecuperarHoraRPI(tiempo);                      //Recupera la hora de la RPi
                        fechaSistema = RecuperarFechaRPI(tiempo);                    //Recupera la fecha de la RPi
                        fuenteReloj =  inputPyloadRS485[7];                          //Recupera la fuente de reloj
                        banSetReloj = 1;                                             //Activa la bandera para indicar que se establecio la hora y fecha
                    }
                    //Envia la hora local al Master:
                    if (subFuncionRS485==0xD2){

                        //Llena el pyload de salida:
                        outputPyloadRS485[0] = 0xD2;
                        for (x=0;x<6;x++){
                            outputPyloadRS485[x+1] = tiempo[x];
                        }
                        outputPyloadRS485[7] = fuenteReloj;
                        delay_ms(10);
                        EnviarTramaRS485(1, IDNODO, 0xF1, 8, outputPyloadRS485);     //Envia la hora local al Master
                        //TEST = ~TEST;
                    }
                    break;

               case 0xF2:
                    //***Funcion de muestreo***
                    //Inicia el muestreo:
                    if ((subFuncionRS485==0xD1)&&(banInicioMuestreo==0)){
                        sectorSD = UbicarUltimoSectorEscrito(inputPyloadRS485[1]);   //inputPyloadRS485[1] = sobrescribir (0=no, 1=si)
                        PSEC = sectorSD;                                             //Guarda el numero del primer sector escrito en este ciclo de muestreo
                        GuardarInfoSector(PSEC, infoPrimerSector);
                        banInicioMuestreo = 1;                                       //Activa la bandera para iniciar el muestreo
                    }
                    //Detiene el muestreo:
                    if ((subFuncionRS485==0xD2)&&(banInicioMuestreo==1)){
                       GuardarInfoSector(sectorSD, infoUltimoSector);                //Guarda la posicion del ultimo sector escrito
                       banInicioMuestreo = 0;                                        //Limpia la bandera para detener el muestreo
                    }
                    break;

               case 0xF3:
                    //***Funcion de analisis y lectura***
                    //Extrae el dato del sector requerido (subfunciones D2 y D3):
                    *ptrsectorReq = inputPyloadRS485[1];                             //LSB sectorReq
                    *(ptrsectorReq+1) = inputPyloadRS485[2];
                    *(ptrsectorReq+2) = inputPyloadRS485[3];
                    *(ptrsectorReq+3) = inputPyloadRS485[4];                         //MSB sectorReq

                    //Envia informacion de sectores clave:
                    if (subFuncionRS485==0xD1){
                       //Llena el pyload de salida y envia la trama de respuesta al Master:
                        InformacionSectores();
                    }
                    //Inspecciona el contenido del sector solicitado:
                    if (subFuncionRS485==0xD2){
                       //Verifica si se esta muestreando en este momento:
                       if (banInicioMuestreo==0){
                          //Envia la orden de inspeccionar el sector inmediatamente:
                          InspeccionarSector(0, sectorReq);
                       } else {
                          //Activa la bandera para inspeccionar el sector cuando haya terminado de escribir la SD:
                          banInsSec=1;
                       }
                    }
                    //Recupera los datos de aceleracion de un segundo:
                    if (subFuncionRS485==0xD3){
                       //Recupera todos los datos de aceleracion dsede el sector requerido y envia la trama de respuesta al Master:
                       //Verifica si se esta muestreando en este momento:
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////