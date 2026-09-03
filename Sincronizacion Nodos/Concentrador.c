/*-------------------------------------------------------------------------------------------------------------------------
Nodo Concentrador - Sistema de Monitorizacion de Salud Estructural V1.5
Autor: David Timbi

Descripcion:
  Firmware principal del concentrador, genera el pulso de sincronizacion
  INT_SINC_1 y maneja el intercambio de mensajes por el bus RS485.

Hardware:
  dsPIC33EP256MC202, XT=80MHz (FRC+PLL, sin cristal), MAX485 #1 (bidireccional)
  y MAX485 #2 (solo transmisor para sincronizacion).

  LEDs de estado (segun informe de diseno PCB):
    D5 (LED_DSPIC / RA1)  -> Heartbeat de sincronismo, toggle cada 1s (Timer3).
    D2 (INT_SINC_2 / RA3) -> Indicador de trafico RS485 (dato recibido de un nodo).
    D4 (LED_RPI)          -> Controlado por la Raspberry Pi, no por este firmware.

  IMPORTANTE Config Bits: Oscillator Source Selection = FRCPLL (sin cristal),
  y OSC2 Pin Function = I/O (no "clock output"), para poder usar RA3 como D2.

---------------------------------------------------------------------------------------------------------------------------*/


////////////////////////////////////////////////////         Pines         /////////////////////////////////////////////////


sbit RP1 at LATA4_bit;                                                          // Pulso de interrupcion hacia la RPi.
sbit RP1_Direction at TRISA4_bit;
sbit MSRS485 at LATB11_bit;                                                     // Control DE/RE del MAX485 #1 (bidireccional, datos).
sbit MSRS485_Direction at TRISB11_bit;

sbit INT_SINC_1 at LATA0_bit;                                                   // Pulso de sincronizacion -> DI del MAX485 #2 (TX permanente).
sbit INT_SINC_1_Direction at TRISA0_bit;

sbit INT_SINC at LATA1_bit;                                                     // D5 (LED_DSPIC): heartbeat de sincronismo.
sbit INT_SINC_Direction at TRISA1_bit;

sbit INT_SINC_2 at LATA3_bit;                                                   // D2: indicador de trafico RS485.
sbit INT_SINC_2_Direction at TRISA3_bit;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////         Librerias         /////////////////////////////////////////////////////////////

#include <RS485.c>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////  Variables globales  /////////////////////////////////////////////////////////////

// Indices generales
unsigned int i, j, x;

// Periodo de sincronizacion
#define PERIODO_SYNC_MS      1000                                              // Intervalo de sincronizacion en ms.
#define ANCHO_PULSO_SYNC_US  1000                                              // Duracion del pulso INT_SINC_1 en us.

// SPI / RPi
unsigned short banLec, banEsc;
unsigned char *ptrnumBytesSPI;
unsigned char tramaSolicitudSPI[10];                                            // Solicitud desde la RPi
unsigned char tramaSolicitudNodo[10];                                           // Solicitud para el nodo RS485
unsigned short banInicio;
unsigned short banRespuestaPi;
unsigned short banSPI0, banSPI1, banSPI2, banSPI7, banSPI8, banSPIA;
unsigned short bufferSPI;

// RS485
unsigned short banRSI, banRSC;                                                  // Estado de recepcion RS485.
unsigned char byteRS485;
unsigned int i_rs485;
unsigned char tramaCabeceraRS485[10];                                           // [0x3A, direccion, funcion, numDatos LSB, numDatos MSB].
unsigned char inputPyloadRS485[2600];                                           // Payload recibido desde el nodo.
unsigned char outputPyloadRS485[15];                                           // Payload a enviar al nodo.
unsigned short direccionRS485;                                                  // Direccion del nodo destino (255 = broadcast).
unsigned short funcionRS485;
unsigned short subFuncionRS485;
unsigned int numDatosRS485;
unsigned char *ptrnumDatosRS485;

// Muestreo
unsigned short banInicioMuestreo;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////  Declaracion de funciones  /////////////////////////////////////////////////////////
void ConfiguracionPrincipal();
void InterrupcionP1(unsigned short funcionSPI, unsigned short subFuncionSPI, unsigned int numBytesSPI);
void CambiarEstadoBandera(unsigned short bandera, unsigned short estado);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////      Main      ////////////////////////////////////////////////////////////////
void main() {

     ConfiguracionPrincipal();

   // Subindices.
     i = 0; j = 0; x = 0;

   // Comunicacion SPI.
     banSPI0 = 0; banSPI1 = 0; banSPI2 = 0; banSPI7 = 0; banSPI8 = 0; banSPIA = 0;
     banRespuestaPi = 0;

   // RS485.
     banRSI = 0;
     banRSC = 0;
     byteRS485 = 0;
     i_rs485 = 0;
     funcionRS485 = 0;
     subFuncionRS485 = 0;
     numDatosRS485 = 0;
     ptrnumDatosRS485 = (unsigned char *) &numDatosRS485;

   // Muestreo.
     banInicioMuestreo = 0;

   // Puertos.
     RP1 = 0;
   INT_SINC = 1;                                                              // D5: enciende el testigo de sincronismo (opcional).
   INT_SINC_1 = 0;                                                            // Linea de sincronizacion en reposo.
   INT_SINC_2 = 0;                                                            // D2: indicador de trafico RS485, apagado al inicio.

   MSRS485 = 0;                                                               // MAX485 #1 en modo recepcion.

     SPI1BUF = 0x00;

   T3CON.TON = 1;                                                             // Arranca el generador de pulsos de sincronizacion.

     while(1){
              asm CLRWDT;
              Delay_ms(100);
     }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////  FUNCIONES  ////////////////////////////////////////////////////////////////

//*****************************************************************************************************************************************
// Configura los perifericos principales del concentrador.
void ConfiguracionPrincipal(){

   // Configuracion del oscilador. FPLLO = FIN * (M / (N1 + N2)) = 80.017 MHz.
     CLKDIVbits.FRCDIV = 0;
     CLKDIVbits.PLLPOST = 0;
     CLKDIVbits.PLLPRE = 5;
     PLLFBDbits.PLLDIV = 150;

   // Configuracion de puertos.
   ANSELA = 0;                                                                // PORTA digital.
   ANSELB = 0;                                                                // PORTB digital.

   INT_SINC_Direction   = 0;                                                  // D5.
   INT_SINC_1_Direction = 0;                                                  // Salida hacia el MAX485 #2 (pulso de sincronizacion).
   INT_SINC_2_Direction = 0;                                                  // D2 (OSC2 debe estar configurado como I/O).
   RP1_Direction = 0;                                                         // RP1 (interrupcion hacia la RPi).
   MSRS485_Direction = 0;                                                     // Control del MAX485 #1.

   INTCON2.GIE = 1;                                                           // Habilita las interrupciones globales.

   // Configuracion del puerto UART2 (bus RS485 principal y datos).
   RPINR19bits.U2RXR = 0x2F;                                                  // Rx2 en RB15/RPI47.
   RPOR1bits.RP36R = 0x03;                                                    // Tx2 en RB4/RP36.
   U2RXIE_bit = 1;                                                            // Habilita interrupcion UART2 RX.
     IPC7bits.U2RXIP = 0x04;
     U2STAbits.URXISEL = 0x00;
     UART2_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED);

   // Configuracion del puerto SPI1 en modo esclavo (interfaz con la RPi).
     SPI1STAT.SPIEN = 1;
     SPI1_Init_Advanced(_SPI_SLAVE, _SPI_8_BIT, _SPI_PRESCALE_SEC_1, _SPI_PRESCALE_PRI_1, _SPI_SS_ENABLE, _SPI_DATA_SAMPLE_END, _SPI_CLK_IDLE_HIGH, _SPI_ACTIVE_2_IDLE);
     SPI1IF_bit = 0;
     IPC2bits.SPI1IP = 0x03;

   // Configuracion del TMR2: timeout de 300 ms para respuestas RS485.
     T2CON = 0x30;
     T2CON.TON = 0;
     T2IE_bit = 1;
     T2IF_bit = 0;
     PR2 = 46875;
     IPC1bits.T2IP = 0x02;

   // Configuracion del TMR3: generador del pulso de sincronizacion INT_SINC_1.
   T3CON = 0x30;                                                              // Prescalador 1:256.
   T3CON.TON = 0;                                                             // Se enciende en main() al finalizar la configuracion.
     T3IE_bit = 1;
     T3IF_bit = 0;
   PR3 = 15625;                                                               // Preload equivalente a 100 ms.
     IPC2bits.T3IP = 0x02;

   // Habilitacion de interrupciones.
     SPI1IE_bit = 1;

   Delay_ms(200);                                                             // Espera a que se estabilicen los cambios.

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// Genera la interrupcion hacia la RPi y notifica, si corresponde, una respuesta pendiente.
void InterrupcionP1(unsigned short funcionSPI, unsigned short subFuncionSPI, unsigned int numBytesSPI){

     if (banRespuestaPi==1){
         ptrnumBytesSPI = (unsigned char *) &numBytesSPI;
         tramaSolicitudSPI[0] = funcionSPI;
         tramaSolicitudSPI[1] = subFuncionSPI;
         tramaSolicitudSPI[2] = *(ptrnumBytesSPI);
         tramaSolicitudSPI[3] = *(ptrnumBytesSPI+1);
         RP1 = 1;
         Delay_us(20);
         RP1 = 0;
         banRespuestaPi = 0;
     }
}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// Cambia el estado de las banderas de comunicacion SPI.
void CambiarEstadoBandera(unsigned short bandera, unsigned short estado){

     if (estado==1){
         banSPI0 = 3; banSPI1 = 3; banSPI2 = 3; banSPI7 = 3; banSPI8 = 3; banSPIA = 3;
         switch (bandera){
                case 0: banSPI0 = 1; break;
                case 1: banSPI1 = 1; break;
                case 2: banSPI2 = 1; break;
                case 7: banSPI7 = 1; break;
                case 8: banSPI8 = 1; break;
                case 0x0A: banSPIA = 1; break;
         }
     }

     if (estado==0){
         banSPI0 = 0; banSPI1 = 0; banSPI2 = 0; banSPI7 = 0; banSPI8 = 0; banSPIA = 0;
     }
}
//*****************************************************************************************************************************************

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////  INTERRUPCIONES  /////////////////////////////////////////////////////////////

//*****************************************************************************************************************************************
// Interrupcion SPI1: comandos desde la RPi hacia el bus RS485.
void spi_1() org  IVT_ADDR_SPI1INTERRUPT {

     SPI1IF_bit = 0;
     bufferSPI = SPI1BUF;

   // Solicitud de operacion generica (C:0xA0, F:0xF0).
     if ((banSPI0==0)&&(bufferSPI==0xA0)) {
        CambiarEstadoBandera(0,1);
        i = 1;
        SPI1BUF = tramaSolicitudSPI[0];
     }
     if ((banSPI0==1)&&(bufferSPI!=0xA0)&&(bufferSPI!=0xF0)){
        SPI1BUF = tramaSolicitudSPI[i];
        i++;
     }
     if ((banSPI0==1)&&(bufferSPI==0xF0)){
        CambiarEstadoBandera(0,0);
     }

   // Iniciar muestreo (C:0xA1, F:0xF1).
     if ((banSPI1==0)&&(bufferSPI==0xA1)){
        CambiarEstadoBandera(1,1);
        i = 0;
     }
     if ((banSPI1==1)&&(bufferSPI!=0xA1)&&(bufferSPI!=0xF1)){
      tramaSolicitudSPI[i] = bufferSPI;                                       // Direccion del nodo + indicador de sobrescritura de SD.
        i++;
     }
     if ((banSPI1==1)&&(bufferSPI==0xF1)){
        direccionRS485 = tramaSolicitudSPI[0];
        outputPyloadRS485[0] = 0xD1;
        outputPyloadRS485[1] = tramaSolicitudSPI[1];
        EnviarTramaRS485(RS485_PUERTO_UART2, direccionRS485, 0xF2, 2, outputPyloadRS485);
        CambiarEstadoBandera(1,0);
     }

   // Detener muestreo (C:0xA2, F:0xF2).
     if ((banSPI2==0)&&(bufferSPI==0xA2)){
        CambiarEstadoBandera(2,1);
        i = 0;
     }
     if ((banSPI2==1)&&(bufferSPI!=0xA2)&&(bufferSPI!=0xF2)){
        tramaSolicitudSPI[i] = bufferSPI;
     }
     if ((banSPI2==1)&&(bufferSPI==0xF2)){
        direccionRS485 = tramaSolicitudSPI[0];
        outputPyloadRS485[0] = 0xD2;
        EnviarTramaRS485(RS485_PUERTO_UART2, direccionRS485, 0xF2, 1, outputPyloadRS485);
        CambiarEstadoBandera(2,0);
     }

   // Reenvio de solicitud de estado a los nodos (C:0xA7, F:0xF7).
     if ((banSPI7==0)&&(bufferSPI==0xA7)){
        CambiarEstadoBandera(7,1);
        i = 0;
     }
     if ((banSPI7==1)&&(bufferSPI!=0xA7)&&(bufferSPI!=0xF7)){
        tramaSolicitudSPI[i] = bufferSPI;
     }
     if ((banSPI7==1)&&(bufferSPI==0xF7)){
        direccionRS485 = tramaSolicitudSPI[i];
        outputPyloadRS485[0] = 0xD2;
        EnviarTramaRS485(RS485_PUERTO_UART2, direccionRS485, 0xF1, 1, outputPyloadRS485);
      T2CON.TON = 1;                                                             // Inicia el timeout 2 (espera de respuesta del nodo).
        TMR2 = 0;
        banRespuestaPi = 1;
        CambiarEstadoBandera(7,0);
     }

   // Reenvio generico de instrucciones a los nodos (C:0xA8, F:0xF8).
     if ((banSPI8==0)&&(bufferSPI==0xA8)){
        CambiarEstadoBandera(8,1);
        i = 0;
     }
     if ((banSPI8==1)&&(i<4)){
      tramaSolicitudNodo[i] = bufferSPI;                                      // Cabecera: [0x3A, direccion, funcion, numDatos].
        i++;
     }
     if ((banSPI8==1)&&(i==4)){
        direccionRS485 = tramaSolicitudNodo[1];
        funcionRS485   = tramaSolicitudNodo[2];
        numDatosRS485  = tramaSolicitudNodo[3];
        i = 0;
        banSPI8 = 2;
     }
     if ((banSPI8==2)&&(i<=numDatosRS485)){
        tramaSolicitudNodo[i] = bufferSPI;
        i++;
     }
     if ((banSPI8==2)&&(bufferSPI==0xF8)&&(i>numDatosRS485)){
        CambiarEstadoBandera(8,0);
        if (numDatosRS485>1){
            for (x=0;x<numDatosRS485;x++){
                outputPyloadRS485[x] = tramaSolicitudNodo[x+1];
            }
        } else {
            outputPyloadRS485[0] = tramaSolicitudNodo[1];
        }
        banRSI = 0;
        banRSC = 0;
        i_rs485 = 0;
        banRespuestaPi = 1;
        EnviarTramaRS485(RS485_PUERTO_UART2, direccionRS485, funcionRS485, numDatosRS485, outputPyloadRS485);
        if (direccionRS485 != RS485_DIR_BROADCAST) {
            T2CON.TON = 1;                                                         // Inicia el timeout 2 para respuestas individuales.
            TMR2 = 0;
        }
     }

   // Entrega el payload recibido por RS485 a la RPi (C:0xAA, F:0xFA).
     if ((banSPIA==0)&&(bufferSPI==0xAA)){
        CambiarEstadoBandera(0x0A,1);
        SPI1BUF = inputPyloadRS485[0];
        i = 1;
     }
     if ((banSPIA==1)&&(bufferSPI!=0xAA)&&(bufferSPI!=0xFA)){
        SPI1BUF = inputPyloadRS485[i];
        i++;
     }
     if ((banSPIA==1)&&(bufferSPI==0xFA)){
        CambiarEstadoBandera(0x0A,0);
     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// Timer3: genera periodicamente el pulso de sincronizacion INT_SINC_1.
void Timer3Int() org IVT_ADDR_T3INTERRUPT{

     static unsigned int contadorSync = 0;

     T3IF_bit = 0;
     contadorSync++;

     if (contadorSync >= (PERIODO_SYNC_MS/100)){
        contadorSync = 0;

      INT_SINC = ~INT_SINC;                                                  // D5: heartbeat de sincronismo.

      // Pulso de sincronizacion hacia el MAX485 #2 (TX permanente).
        INT_SINC_1 = 1;
        Delay_us(ANCHO_PULSO_SYNC_US);
        INT_SINC_1 = 0;
     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// Timeout de 300 ms para esperar la respuesta de un nodo por RS485 (funcion 0xA8/0xF8).
void Timer2Int() org IVT_ADDR_T2INTERRUPT{

     T2IF_bit = 0;
     T2CON.TON = 0;
     TMR2 = 0;

   INT_SINC = ~INT_SINC;                                                     // D5: tambien se conmuta en caso de timeout.

     banRSI = 0;
     banRSC = 0;
     i_rs485 = 0;

   // Notifica a la RPi el codigo de error por timeout.
     numDatosRS485 = 3;
     inputPyloadRS485[0] = 0xD3;
     inputPyloadRS485[1] = 0xEE;
     inputPyloadRS485[2] = 0xE4;
     banRespuestaPi = 1;
     InterrupcionP1(0xB3,0xD3,3);

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// Interrupcion UART2: recepcion de tramas por el bus RS485 principal.
void urx_2() org  IVT_ADDR_U2RXINTERRUPT {

     U2RXIF_bit = 0;
     byteRS485 = U2RXREG;
     U2STA.OERR = 0;

   // Recupera el payload de la trama RS485 despues de la cabecera.
     if (banRSI==2){
        if (i_rs485<(numDatosRS485)){
           inputPyloadRS485[i_rs485] = byteRS485;
           i_rs485++;
        } else {
           banRSI = 0;
           banRSC = 1;
        }
     }

   // Recupera la cabecera: [0x3A, direccion, funcion, numDatos LSB, numDatos MSB].
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
        if (tramaCabeceraRS485[1]==direccionRS485){
           T2CON.TON = 0;                                                          // Detiene el timeout 2.
           TMR2 = 0;
           funcionRS485 = tramaCabeceraRS485[2];
           *(ptrnumDatosRS485)   = tramaCabeceraRS485[3];                       // LSB de numDatosRS485.
           *(ptrnumDatosRS485+1) = tramaCabeceraRS485[4];                       // MSB de numDatosRS485.
           banRSI = 2;
           i_rs485 = 0;
        } else {
           banRSI = 0;
           banRSC = 0;
           i_rs485 = 0;
        }
     }

   // Procesa la trama completa recibida.
     if (banRSC==1){
        subFuncionRS485 = inputPyloadRS485[0];
        banRespuestaPi = 1;

      INT_SINC_2 = ~INT_SINC_2;                                              // D2: trama valida recibida de un nodo.

        switch (funcionRS485){
               case 0xF1: InterrupcionP1(0xB1,subFuncionRS485,numDatosRS485); break;
               case 0xF3: InterrupcionP1(0xB3,subFuncionRS485,numDatosRS485); break;
        }
        banRSC = 0;
     }
}
//*****************************************************************************************************************************************