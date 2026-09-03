/*-------------------------------------------------------------------------------------------------------------------------
Nodo sensor - Sistema de monitorizacion de salud estructural V1.5
Autor: David Timbi

Descripcion:
  Firmware de nodo sensor, version "solo RS485" para validar el bus y la
  sincronizacion Daisy Chain antes de reincorporar ADXL355/SD.
  IDNODO define la direccion del nodo: este es 1 (Nodo A). Para el Nodo B,
  copiar este archivo y cambiar unicamente IDNODO a 2.

Hardware:
  dsPIC33EP256MC202, XT=80MHz (FRC+PLL, sin cristal), MAX485 receptor
  permanente (pines 2 y 3 a GND) conectado a INT1 para capturar el pulso
  de sincronizacion, y MAX485 bidireccional para datos RS485.

   LED de estado (segun el informe de diseno de la PCB):
      D3 (TEST1 / RA2) -> heartbeat de sincronismo recibido; cambia de estado en cada
                         pulso INT_SINC capturado. Debe parpadear en fase
                         con D5 del Concentrador si la red esta sincronizada.

---------------------------------------------------------------------------------------------------------------------------*/

////////////////////////////////////////////////////         Pines         /////////////////////////////////////////////////

sbit TEST1 at LATA2_bit;                                                        // D3: heartbeat de sincronismo recibido.
sbit TEST1_Direction at TRISA2_bit;

sbit MSRS485 at LATB12_bit;                                                     // Control DE/RE del MAX485 de datos.
sbit MSRS485_Direction at TRISB12_bit;

// El MAX485 receptor de sincronismo permanece en escucha; su salida INT_SINC
// se conecta directamente a la interrupcion externa INT1 (RB14).

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////         Librerias         /////////////////////////////////////////////////////////////

#include <RS485.c>                // Modulo compartido con el concentrador.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////  Variables globales  /////////////////////////////////////////////////////////////

#define IDNODO 1                                                               // ID RS485 del nodo: 1 = nodo A; nodo B usa IDNODO 2.

unsigned int i, j, x;

// Estado y datos del protocolo RS485.
unsigned short banRSI, banRSC;
unsigned char byteRS485;
unsigned int i_rs485;
unsigned char tramaCabeceraRS485[10];                                           // [0x3A, direccion, funcion, numDatos LSB, numDatos MSB].
unsigned char inputPyloadRS485[15];
unsigned char outputPyloadRS485[15];
unsigned int numDatosRS485;
unsigned char *ptrnumDatosRS485;
unsigned short funcionRS485;
unsigned short subFuncionRS485;
unsigned short contTMR2;                                                        // Timeout de recuperacion del UART1.

// ============================================================================
// Declaracion de funciones
// ============================================================================
void ConfiguracionPrincipal();

// ============================================================================
// Programa principal
// ============================================================================
void main() {

     ConfiguracionPrincipal();
     TEST1 = 0;

     i = 0; j = 0; x = 0;

     banRSI = 0; banRSC = 0;
     byteRS485 = 0; i_rs485 = 0;
     funcionRS485 = 0; subFuncionRS485 = 0;
     numDatosRS485 = 0;
     ptrnumDatosRS485 = (unsigned char *) &numDatosRS485;
     contTMR2 = 0;

   MSRS485 = 0;                                                               // MAX485 de datos en modo recepcion.

     while(1){
           asm CLRWDT;
           Delay_ms(100);
     }

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ============================================================================
// Funciones
// ============================================================================

//*****************************************************************************************************************************************
void ConfiguracionPrincipal(){

   // Configuracion del oscilador. FPLLO = FIN * (M / (N1 + N2)) = 80.017 MHz.
     CLKDIVbits.FRCDIV = 0;
     CLKDIVbits.PLLPOST = 0;
     CLKDIVbits.PLLPRE = 5;
     PLLFBDbits.PLLDIV = 150;

   // Configuracion de puertos.
     ANSELA = 0;
     ANSELB = 0;
     TEST1_Direction = 0;
     MSRS485_Direction = 0;
   TRISB14_bit = 1;                                                           // RB14/RPI46 = INT_SINC, entrada desde el MAX485 receptor.

   INTCON2.GIE = 1;                                                           // Habilita las interrupciones globales.

   // Configuracion del UART1 (bus RS485 principal y MAX485 de datos).
   RPINR18bits.U1RXR = 0x2F;                                                  // Rx1 en RB15/RPI47. Verificar en la placa.
   RPOR1bits.RP36R = 0x01;                                                    // Tx1 en RB4/RP36. Verificar en la placa.
     U1RXIE_bit = 1;
     U1STAbits.URXISEL = 0x00;
     U1RXIF_bit = 0;
     IPC2bits.U1RXIP = 0x04;
     UART1_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED);

   // Configuracion de INT1 <- INT_SINC (MAX485 receptor en escucha permanente).
   RPINR0 = 0x2E00;                                                           // INT1 <- RB14/RPI46 (INT_SINC). Verificar en la placa.
     INT1IE_bit = 1;
     INT1IF_bit = 0;
     IPC5bits.INT1IP = 0x01;

   // Configuracion del TMR2 para recuperar UART1 tras descartar una trama.
     T2CON = 0x0030;
     T2CON.TON = 0;
     T2IE_bit = 1;
     T2IF_bit = 0;
     PR2 = 46875;                                                               //300ms
     IPC1bits.T2IP = 0x02;

     Delay_ms(200);

}
//*****************************************************************************************************************************************

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////  INTERRUPCIONES  /////////////////////////////////////////////////////////////

//*****************************************************************************************************************************************
// INT1 <- INT_SINC: pulso de sincronizacion recibido del Concentrador via MAX485 receptor.
// D3 conmuta en cada pulso capturado, dando visualmente el "heartbeat" de sincronismo
// de este nodo. Debe parpadear en fase con D5 del concentrador.
void int_1() org IVT_ADDR_INT1INTERRUPT {

     INT1IF_bit = 0;

   TEST1 = ~TEST1;                                                           // D3: heartbeat de sincronismo recibido.

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
// TMR2: si una trama RS485 no iba dirigida a este nodo, reactiva el UART1 tras 1200ms
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
// UART1: recibe tramas RS485 del concentrador, valida la direccion y responde
// al eco 0xF1/0xD2 para confirmar el enlace.
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
           // La trama esta dirigida a otro nodo; se descarta para evitar colisiones.
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

      // Eco de prueba: responde con IDNODO para confirmar el enlace bidireccional.
        if ((funcionRS485==0xF1)&&(subFuncionRS485==0xD2)){
            outputPyloadRS485[0] = 0xD2;
            outputPyloadRS485[1] = IDNODO;
            delay_ms(10);
            EnviarTramaRS485(RS485_PUERTO_UART1, IDNODO, 0xF1, 2, outputPyloadRS485);
        }

        banRSC = 0;
        banRSI = 0;
     }

}
//*****************************************************************************************************************************************