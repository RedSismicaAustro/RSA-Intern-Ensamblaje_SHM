/*-------------------------------------------------------------------------------------------------------------------------
Autor: Milton Munoz, email: miltonrodrigomunoz@gmail.com, github: https://github.com/Rodrig087/SaludEstructuralCS.git
Fecha de creacion: 16/01/2020
Configuracion: dsPIC33EP256MC202, XT=80MHz
---------------------------------------------------------------------------------------------------------------------------*/


////////////////////////////////////////////////////         Librerias         /////////////////////////////////////////////////////////////

#include <TIEMPO_RTC.c>
#include <TIEMPO_GPS.c>
#include <TIEMPO_RPI.c>
#include <RS485.c>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////// Declaracion de variables y costantes ///////////////////////////////////////////////////////

//Subindices:
unsigned int i, j, x, y;

//Definicion de pines:
sbit RP1 at LATA4_bit;                                                          //Definicion del pin P1
sbit RP1_Direction at TRISA4_bit;
sbit MSRS485 at LATB11_bit;                                                     //Definicion del pin MS RS485
sbit MSRS485_Direction at TRISB11_bit;
//Pines de interrupcion para manejar los nodos:
sbit INT_SINC at LATA1_bit;                                                     //Comparte con el pin TEST
sbit INT_SINC_Direction at TRISA1_bit;
sbit INT_SINC1 at LATA0_bit;
sbit INT_SINC1_Direction at TRISA0_bit;
sbit INT_SINC2 at LATA3_bit;
sbit INT_SINC2_Direction at TRISA3_bit;
sbit INT_SINC3 at LATB10_bit;
sbit INT_SINC3_Direction at TRISB10_bit;
sbit INT_SINC4 at LATB12_bit;
sbit INT_SINC4_Direction at TRISB12_bit;

//Constantes para el UART:
#define FP 80000000
#define BAUDRATE 115200
#define BRGVAL ((FP/BAUDRATE)/16)-1

//Variables para manejo del GPS:
unsigned int i_gps;
unsigned char byteGPS, banGPSI, banGPSC;
unsigned short banSetGPS;
unsigned char tramaGPS[70];
unsigned char datosGPS[13];
unsigned short contTimeout1;

//Variables para manejo del tiempo:
unsigned short tiempo[6];                                                       //Vector de datos de tiempo del sistema
unsigned short tiempoRPI[6];                                                    //Vector para recuperar el tiempo enviado desde la RPi
unsigned short banSetReloj, banSyncReloj, banRespuestaPi;
unsigned short fuenteReloj;                                                     //Indica la fuente de reloj: 1=GPS, 2=RTC, 3=RPi
unsigned long horaSistema, fechaSistema;
unsigned short referenciaTiempo;                                                //Variable para la referencia de tiempo solicitada: 1=GPS, 2=RTC

//Variables para la comunicacion SPI:
unsigned short bufferSPI;
unsigned short banLec, banEsc;                                                                                                              //Numero de bytes que se van a enviar a la RPi por SPI
unsigned char *ptrnumBytesSPI;                                                  //Puntero asociado al numero de bytes SPI
unsigned char tramaSolicitudSPI[10];                                            //Vector para almacenar los datos de solicitud que envia la RPi a traves del SPI
unsigned char tramaSolicitudNodo[10];                                           //Vector para almacenar los datos de solicitud que se reenvia a los nodos
unsigned char tramaCompleta[2506];                                              //Vector para almacenar 10 vectores datosFIFO, 250 cabeceras de muestras y el vector tiempo
unsigned char tramaPrueba[10];                                                  //Trama de 10 elementos para probar la comunicacion RS485
unsigned short banInicio;
unsigned short banOperacion;
unsigned short banCheckRS485;
unsigned short banSPI0, banSPI1, banSPI2, banSPI3, banSPI4, banSPI5, banSPI6, banSPI7, banSPI8, banSPI9, banSPIA;

//Variables para manejo del RS485:
unsigned short banRSI, banRSC;                                                  //Banderas de control de inicio de trama y trama completa
unsigned char byteRS485;
unsigned int i_rs485;                                                           //Indice
unsigned char tramaCabeceraRS485[10];                                            //Vector para almacenar los datos de cabecera de la trama RS485: [0x3A, Direccion, Funcion, NumeroDatos]
unsigned char inputPyloadRS485[2600];                                            //Vector para almacenar el pyload de la trama RS485 recibida
unsigned char outputPyloadRS485[15];                                            //Vector para almacenar el pyload de la trama RS485 a enviar
unsigned short direccionRS485;                                                  //Varaible para la direccion del nodo. Broadcast = 255
unsigned short funcionRS485;                                                    //Funcion requerida: 0xF1 = Muestrear, 0xF2 = Actualizar tiempo, 0xF3 = Probar comunicacion
unsigned short subFuncionRS485;                                                 //Sub funcion requerida: 0xD1, 0xD2, 0xD3  (Depende de la funcion)
unsigned int numDatosRS485;                                                     //Numero de datos en el pyload de la trama RS485
unsigned char *ptrnumDatosRS485;
unsigned char tramaPruebaRS485[10]= {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};   //Trama de 10 elementos para probar la comunicacion RS485

//Variables para el control del muestreo:
unsigned short banInicioMuestreo;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////  Declaracion de funciones  /////////////////////////////////////////////////////////
void ConfiguracionPrincipal();
void Muestrear();
void InterrupcionP1(unsigned short funcionSPI, unsigned short subFuncionSPI, unsigned int numBytesSPI);
void CambiarEstadoBandera(unsigned short bandera, unsigned short estado);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////      Main      ////////////////////////////////////////////////////////////////
void main() {

     ConfiguracionPrincipal();
     GPS_init(1,1);
     DS3234_init();

     //Inicializacion de variables:

     //Subindices:
     i = 0;
     j = 0;
     x = 0;
     y = 0;

     //Comunicacion SPI:
     banSPI0 = 0;
     banSPI1 = 0;
     banSPI2 = 0;
     banSPI3 = 0;
     banSPI4 = 0;
     banSPI5 = 0;
     banSPI6 = 0;
     banSPI7 = 0;
     banSPI8 = 0;
     banSPI9 = 0;
     banSPIA = 0;

     //GPS:
     i_gps = 0;
     byteGPS = 0;
     banGPSI = 0;
     banGPSC = 0;
     banSetGPS = 0;
         contTimeout1 = 0;

     //Tiempo:
     banSetReloj = 0;
     banSyncReloj = 0;
     banRespuestaPi = 0;
     horaSistema = 0;
     fechaSistema = 0;
     fuenteReloj = 0;
     referenciaTiempo = 0;

     //RS485:
     banRSI = 0;
     banRSC = 0;
     byteRS485 = 0;
     i_rs485 = 0;
     funcionRS485 = 0;
     subFuncionRS485 = 0;
     numDatosRS485 = 0;
     ptrnumDatosRS485 = (unsigned char *) & numDatosRS485;

     //Muestreo:
     banInicioMuestreo = 0;

     //Puertos:
     RP1 = 0;
     INT_SINC = 1;                                                              //Enciende el pin TEST
     INT_SINC1 = 0;                                                             //Inicializa los pines de sincronizacion en 0
     INT_SINC2 = 0;
     INT_SINC3 = 0;
     INT_SINC4 = 0;

     MSRS485 = 0;                                                               //Establece el Max485 en modo de lectura;

     SPI1BUF = 0x00;

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

     //configuracion del oscilador                                              //FPLLO = FIN*(M/(N1+N2)) = 80.017MHz
     CLKDIVbits.FRCDIV = 0;                                                     //FIN=FRC/1
     CLKDIVbits.PLLPOST = 0;                                                    //N2=2
     CLKDIVbits.PLLPRE = 5;                                                     //N1=7
     PLLFBDbits.PLLDIV = 150;                                                   //M=152

     //Configuracion de puertos
     ANSELA = 0;                                                                //Configura PORTA como digital     *
     ANSELB = 0;                                                                //Configura PORTB como digital     *

     TRISA2_bit = 0;                                                            //RTC_CS
     INT_SINC_Direction = 0;                                                    //INT_SINC
     INT_SINC1_Direction = 0;                                                   //INT_SINC1
     INT_SINC2_Direction = 0;                                                   //INT_SINC2
     INT_SINC3_Direction = 0;                                                   //INT_SINC3
     INT_SINC4_Direction = 0;                                                   //INT_SINC4
     RP1_Direction = 0;                                                         //RP1
     MSRS485_Direction = 0;                                                     //MSRS485
     TRISB13_bit = 1;                                                           //SQW
     TRISB14_bit = 1;                                                           //PPS

     INTCON2.GIE = 1;                                                           //Habilita las interrupciones globales *

    //Configuracion del puerto UART1
     RPINR18bits.U1RXR = 0x22;                                                  //Configura el pin RB2/RPI34 como Rx1
     RPOR0bits.RP35R = 0x01;                                                    //Configura el Tx1 en el pin RB3/RP35
     U1RXIE_bit = 1;                                                            //Habilita la interrupcion UART1 RX
     IPC2bits.U1RXIP = 0x04;                                                    //Prioridad de la interrupcion UART1 RX
     U1STAbits.URXISEL = 0x00;                                                  //Interrupt is set when any character is received and transferred from the UxRSR to the receive buffer; receive buffer has one or more characters
     UART1_Init(9600);                                                          //Inicializa el UART1 con una velocidad de 9600 baudios

     //Configuracion del puerto UART2
     RPINR19bits.U2RXR = 0x2F;                                                  //Configura el pin RB15/RPI47 como Rx2
     RPOR1bits.RP36R = 0x03;                                                    //Configura el Tx2 en el pin RB4/RP36
     U2RXIE_bit = 1;                                                            //Habilita la interrupcion UART2 RX
     IPC7bits.U2RXIP = 0x04;                                                    //Prioridad de la interrupcion UART1 RX
     U2STAbits.URXISEL = 0x00;
     UART2_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED);

     //Configuracion del puerto SPI1 en modo Esclavo
     SPI1STAT.SPIEN = 1;                                                        //Habilita el SPI1 *
     SPI1_Init_Advanced(_SPI_SLAVE, _SPI_8_BIT, _SPI_PRESCALE_SEC_1, _SPI_PRESCALE_PRI_1, _SPI_SS_ENABLE, _SPI_DATA_SAMPLE_END, _SPI_CLK_IDLE_HIGH, _SPI_ACTIVE_2_IDLE);
     SPI1IF_bit = 0;                                                            //Limpia la bandera de interrupcion por SPI *
     IPC2bits.SPI1IP = 0x03;                                                    //Prioridad de la interrupcion SPI1

     //Configuracion del puerto SPI2 en modo Master
     RPINR22bits.SDI2R = 0x21;                                                  //Configura el pin RB1/RPI33 como SDI2 *
     RPOR2bits.RP38R = 0x08;                                                    //Configura el SDO2 en el pin RB6/RP38 *
     RPOR1bits.RP37R = 0x09;                                                    //Configura el SCK2 en el pin RB5/RP37 *
     SPI2STAT.SPIEN = 1;                                                        //Habilita el SPI2 *
     SPI2_Init();                                                               //Inicializa el modulo SPI2
     CS_DS3234 = 1;                                                             //Pone en alto el CS del RTC

     //Configuracion de las interrupciones externas INT1 e INT2
     RPINR0 = 0x2D00;                                                           //Asigna INT1 al RB13/RPI45 (SQW)
     RPINR1 = 0x002E;                                                           //Asigna INT2 al RB14/RPI46 (PPS)
     INT1IF_bit = 0;                                                            //Limpia la bandera de interrupcion externa INT1
     INT2IF_bit = 0;                                                            //Limpia la bandera de interrupcion externa INT2
     IPC5bits.INT1IP = 0x02;                                                    //Prioridad en la interrupocion externa INT1
     IPC7bits.INT2IP = 0x01;                                                    //Prioridad en la interrupocion externa INT2

     //Configuracion del TMR1 con un tiempo de 300ms
     T1CON = 0x30;                                                              //Prescalador
     T1CON.TON = 0;                                                             //Apaga el Timer1
     T1IE_bit = 1;                                                              //Habilita la interrupción de desbordamiento TMR1
     T1IF_bit = 0;                                                              //Limpia la bandera de interrupcion del TMR1
     PR1 = 46875;                                                               //Carga el preload para un tiempo de 300ms
     IPC0bits.T1IP = 0x02;                                                      //Prioridad de la interrupcion por desbordamiento del TMR1

     //Configuracion del TMR2 con un tiempo de 300ms
     T2CON = 0x30;                                                              //Prescalador
     T2CON.TON = 0;                                                             //Apaga el Timer2
     T2IE_bit = 1;                                                              //Habilita la interrupción de desbordamiento TMR2
     T2IF_bit = 0;                                                              //Limpia la bandera de interrupcion del TMR2
     PR2 = 46875;                                                               //Carga el preload para un tiempo de 300ms
     IPC1bits.T2IP = 0x02;                                                      //Prioridad de la interrupcion por desbordamiento del TMR2

     //Habilitacion de interrupciones
     SPI1IE_bit = 1;                                                            //SPI1
     INT1IE_bit = 1;                                                            //INT1
     INT2IE_bit = 1;                                                            //INT2

     Delay_ms(200);                                                             //Espera hasta que se estabilicen los cambios

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Funcion para realizar la interrupcion en la RPi
 void InterrupcionP1(unsigned short funcionSPI, unsigned short subFuncionSPI, unsigned int numBytesSPI){

     //Si se ejecuta una funcionSPI de tiempo, activa la bandera banSetRelo para incrementar la hora del sistema con cada pulso PPS y envia la hora local a los nodos:
     if ((funcionSPI==0xB1)&&(subFuncionSPI==0xD1)){
        //Llena el pyload de salida:
        outputPyloadRS485[0] = 0xD1;
        for (x=1;x<7;x++){                                                      //Subfuncion
            outputPyloadRS485[x] = tiempo[x-1];                                 //Trama de tiempo
        }
        outputPyloadRS485[7] = fuenteReloj;                                     //Fuente de reloj
        EnviarTramaRS485(2, 255, 0xF1, 8, outputPyloadRS485);                   //Envia la hora local a los nodos
     }

     if (banRespuestaPi==1){
         //Asocia el puntero a la variable:
         ptrnumBytesSPI = (unsigned char *) & numBytesSPI;
         //Llena la trama de solicitud SPI:
         tramaSolicitudSPI[0] = funcionSPI;                                     //Operacion solicitada
         tramaSolicitudSPI[1] = subFuncionSPI;                                  //Subfuncion solicitada
         tramaSolicitudSPI[2] = *(ptrnumBytesSPI);                              //LSB numBytesSPI
         tramaSolicitudSPI[3] = *(ptrnumBytesSPI+1);                            //MSB numBytesSPI
         //Genera el pulso P1 para producir la interrupcion externa en la RPi:
         RP1 = 1;
         Delay_us(20);
         RP1 = 0;
         banRespuestaPi = 0;
     }
}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Funcion para cambiar el estado de las banderas:
void CambiarEstadoBandera(unsigned short bandera, unsigned short estado){

     if (estado==1){
         //Cambia el estado de todas las baderas para evitar posibles interferencias
         banSPI0 = 3;
         banSPI1 = 3;
         banSPI2 = 3;
         banSPI4 = 3;
         banSPI5 = 3;
         banSPI6 = 3;
         banSPI7 = 3;
         banSPI8 = 3;
         banSPIA = 3;
         //Activa la bandera requerida:
         switch (bandera){
                case 0:
                     banSPI0 = 1;
                     break;
                case 1:
                     banSPI1 = 1;
                     break;
                case 2:
                     banSPI2 = 1;
                     break;
                case 4:
                     banSPI4 = 1;
                     break;
                case 5:
                     banSPI5 = 1;
                     break;
                case 6:
                     banSPI6 = 1;
                     break;
                case 7:
                     banSPI7 = 1;
                     break;
                case 8:
                     banSPI8 = 1;
                     break;
                case 0x0A:
                     banSPIA = 1;
                     break;
         }
     }

     //Limpia todas las banderas de comunicacion SPI:
     if (estado==0){
         banSPI0 = 0;
         banSPI1 = 0;
         banSPI2 = 0;
         banSPI4 = 0;
         banSPI5 = 0;
         banSPI6 = 0;
         banSPI7 = 0;
         banSPI8 = 0;
         banSPIA = 0;
     }
}
//*****************************************************************************************************************************************

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////// Interrupciones /////////////////////////////////////////////////////////////

//*****************************************************************************************************************************************
//Interrupcion SPI1
void spi_1() org  IVT_ADDR_SPI1INTERRUPT {

     SPI1IF_bit = 0;                                                            //Limpia la bandera de interrupcion por SPI
     bufferSPI = SPI1BUF;                                                       //Guarda el contenido del bufeer (lectura)

     //************************************************************************************************************************************
     //Rutina para enviar la peticion de operacion a la RPi  (C:0xA0   F:0xF0)
     if ((banSPI0==0)&&(bufferSPI==0xA0)) {
        CambiarEstadoBandera(0,1);                                              //Activa la bandera para enviar el tipo de operacion requerido a la RPi
        i = 1;
        SPI1BUF = tramaSolicitudSPI[0];                                         //Carga en el buffer la funcion requerida
     }
     if ((banSPI0==1)&&(bufferSPI!=0xA0)&&(bufferSPI!=0xF0)){
        SPI1BUF = tramaSolicitudSPI[i];                                         //Se envia la subfuncion, y el LSB y MSB de la variable numBytesSPI
        i++;
     }
     if ((banSPI0==1)&&(bufferSPI==0xF0)){
        CambiarEstadoBandera(0,0);                                              //Limpia las banderas
     }
     //************************************************************************************************************************************

     //************************************************************************************************************************************
     //Rutinas de control del muestreo:

     //Rutina para iniciar el muestreo (C:0xA1   F:0xF1):
     if ((banSPI1==0)&&(bufferSPI==0xA1)){
        CambiarEstadoBandera(1,1);
        i = 0;
     }
     if ((banSPI1==1)&&(bufferSPI!=0xA1)&&(bufferSPI!=0xF1)){
        tramaSolicitudSPI[i] = bufferSPI;                                       //Recupera la direccion del nodo y el indicador de sobrescritura de la SD
        i++;
     }
     if ((banSPI1==1)&&(bufferSPI==0xF1)){
        direccionRS485 = tramaSolicitudSPI[0];
        outputPyloadRS485[0] = 0xD1;                                            //Subfuncion iniciar muestreo
        outputPyloadRS485[1] = tramaSolicitudSPI[1];                            //Payload sobrescribir SD
        EnviarTramaRS485(2, direccionRS485, 0xF2, 2, outputPyloadRS485);        //Envia la solicitud al nodo
        CambiarEstadoBandera(1,0);                                              //Limpia la bandera
     }

     //Rutina para detener el muestreo (C:0xA2   F:0xF2):
     if ((banSPI2==0)&&(bufferSPI==0xA2)){
        CambiarEstadoBandera(2,1);
        i = 0;
     }
     if ((banSPI2==1)&&(bufferSPI!=0xA2)&&(bufferSPI!=0xF2)){
        tramaSolicitudSPI[i] = bufferSPI;                                       //Recupera la direccion del nodo
     }
     if ((banSPI2==1)&&(bufferSPI==0xF2)){
        direccionRS485 = tramaSolicitudSPI[0];
        outputPyloadRS485[0] = 0xD2;                                            //Subfuncion detener muestreo
        EnviarTramaRS485(2, direccionRS485, 0xF2, 1, outputPyloadRS485);        //Envia la solicitud al nodo
        CambiarEstadoBandera(2,0);                                              //Limpia las banderas
     }
     //************************************************************************************************************************************

     //************************************************************************************************************************************
     //Rutinas de manejo de tiempo:

     //(C:0xA4   F:0xF4)
     //Rutina para obtener la hora de la RPi:
     if ((banSPI4==0)&&(bufferSPI==0xA4)){
         CambiarEstadoBandera(4,1);
         j = 0;
     }
     if ((banSPI4==1)&&(bufferSPI!=0xA4)&&(bufferSPI!=0xF4)){
        tiempoRPI[j] = bufferSPI;
        j++;
     }
     if ((banSPI4==1)&&(bufferSPI==0xF4)){
        CambiarEstadoBandera(4,0);                                              //Limpia las banderas
        horaSistema = RecuperarHoraRPI(tiempoRPI);                              //Recupera la hora de la RPi
        fechaSistema = RecuperarFechaRPI(tiempoRPI);                            //Recupera la fecha de la RPi
        DS3234_setDate(horaSistema, fechaSistema);                              //Configura la hora en el RTC
        horaSistema = RecuperarHoraRTC();                                       //Recupera la hora del RTC
        fechaSistema = RecuperarFechaRTC();                                     //Recupera la fecha del RTC
        AjustarTiempoSistema(horaSistema, fechaSistema, tiempo);                //Actualiza los datos de la trama tiempo con la hora y fecha recuperadas
        fuenteReloj = 0;                                                        //Fuente de reloj = RED
        banSetReloj = 1;                                                        //Activa esta bandera para usar la hora/fecha recuperada
        banRespuestaPi = 1;                                                     //Activa esta bandera para enviar una respuesta a la RPi
        InterrupcionP1(0xB1,0xD1,8);                                            //Envia la hora local a la RPi y a los nodos
     }

     //(C:0xA5   F:0xF5)
     //Rutina para enviar la hora local a la RPi:
     if ((banSPI5==0)&&(bufferSPI==0xA5)){
        CambiarEstadoBandera(5,1);
        j = 0;
        SPI1BUF = fuenteReloj;                                                  //Envia el indicador de fuente de reloj (0:RTC, 1:GPS)
     }
     if ((banSPI5==1)&&(bufferSPI!=0xA5)&&(bufferSPI!=0xF5)){
        SPI1BUF = tiempo[j];
        j++;
     }
     if ((banSPI5==1)&&(bufferSPI==0xF5)){
        CambiarEstadoBandera(5,0);                                              //Limpia las banderas
     }

     //(C:0xA6   F:0xF6)
     //Rutina para obtener la referencia de tiempo (1=GPS, 2=RTC):
     if ((banSPI6==0)&&(bufferSPI==0xA6)){
        CambiarEstadoBandera(6,1);
     }
     if ((banSPI6==1)&&(bufferSPI!=0xA6)&&(bufferSPI!=0xF6)){
        referenciaTiempo =  bufferSPI;                                          //Recupera la opcion de referencia de tiempo solicitada
     }
     if ((banSPI6==1)&&(bufferSPI==0xF6)){
        CambiarEstadoBandera(6,0);                                              //Limpia las banderas
        banSetReloj = 1;                                                        //Activa esta bandera para usar la hora/fecha recuperada
        banRespuestaPi = 1;                                                     //Activa esta bandera para enviar una respuesta a la RPi
        if (referenciaTiempo==1){
            //Recupera el tiempo del GPS:
            banGPSI = 1;                                                        //Activa la bandera de inicio de trama  del GPS
            banGPSC = 0;                                                        //Limpia la bandera de trama completa
            U1MODE.UARTEN = 1;                                                  //Inicializa el UART1
            //Inicia el Timeout 1:
            T1CON.TON = 1;
            TMR1 = 0;
        } else {
            //Recupera el tiempo del RTC:
            horaSistema = RecuperarHoraRTC();                                   //Recupera la hora del RTC
            fechaSistema = RecuperarFechaRTC();                                 //Recupera la fecha del RTC
            AjustarTiempoSistema(horaSistema, fechaSistema, tiempo);            //Actualiza los datos de la trama tiempo con la hora y fecha recuperadas
            fuenteReloj = 2;                                                    //Fuente de reloj = RTC
            InterrupcionP1(0xB1,0xD1,8);                                        //Envia la hora local a la RPi
        }
     }

     //(C:0xA7   F:0xF7)
     //Rutina para enviar la solicitud de tiempo a los nodos:
     if ((banSPI7==0)&&(bufferSPI==0xA7)){
        CambiarEstadoBandera(7,1);
        i = 0;
     }
     if ((banSPI7==1)&&(bufferSPI!=0xA7)&&(bufferSPI!=0xF7)){
        tramaSolicitudSPI[i] = bufferSPI;
     }
     if ((banSPI7==1)&&(bufferSPI==0xF7)){
        direccionRS485 =  tramaSolicitudSPI[i];
        outputPyloadRS485[0] = 0xD2;                                            //Llena el pyload de salidas con la subfuncion solicitada
        EnviarTramaRS485(2, direccionRS485, 0xF1, 1, outputPyloadRS485);        //Envia la solicitud al nodo
        banRespuestaPi = 1;
        CambiarEstadoBandera(7,0);                                              //Limpia las banderas
     }

     //(C:0xA8   F:0xF8)
     //Rutina de reenvio de instrucciones a los nodos (C:0xA8   F:0xF8):
     if ((banSPI8==0)&&(bufferSPI==0xA8)){
        CambiarEstadoBandera(8,1);
        i = 0;
     }
     //Recupera la cabecera de datos (cabecera, direccion, funcion, #datos):
     if ((banSPI8==1)&&(i<4)){
        tramaSolicitudNodo[i] = bufferSPI;
        i++;
     }
     //Extrae los datos de la cabecera:
     if ((banSPI8==1)&&(i==4)){
        direccionRS485 = tramaSolicitudNodo[1];
        funcionRS485 = tramaSolicitudNodo[2];
        numDatosRS485 = tramaSolicitudNodo[3];
        i = 0;
        banSPI8 = 2;
     }
     //Recupera los datos del pyload:
     if ((banSPI8==2)&&(i<=numDatosRS485)){
        tramaSolicitudNodo[i] = bufferSPI;
        i++;
     }
     //Procesa la solicitud:
     if ((banSPI8==2)&&(bufferSPI==0xF8)&&(i>numDatosRS485)){
        CambiarEstadoBandera(8,0);                                              //Limpia las banderas
        //Llena la trama outputPyloadRS485 con los datos de solicitud:
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
        //Reenvia la solicitud al nodo por RS485:
        EnviarTramaRS485(2, direccionRS485, funcionRS485, numDatosRS485, outputPyloadRS485);
        //Inicia el Timeout 2:
        T2CON.TON = 1;
        TMR2 = 0;
     }

     //(C:0xAA   F:0xFA)
     //Rutina para enviar el contenido del pyload de la trama RS485 a la RPi:
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
        CambiarEstadoBandera(0x0A,0);                                           //Limpia las banderas
     }

     //************************************************************************************************************************************

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Interrupcion INT1
void int_1() org IVT_ADDR_INT1INTERRUPT {

     INT1IF_bit = 0;                                                            //Limpia la bandera de interrupcion externa INT1

     if (banSetReloj==1){
         horaSistema++;                                                         //Incrementa el reloj del sistema
         AjustarTiempoSistema(horaSistema, fechaSistema, tiempo);
         INT_SINC = ~INT_SINC;                                                  //TEST
         //Genera los pulsos de interrupcion en los nodos:
         //INT_SINC = 1;
         INT_SINC1 = 1;
         INT_SINC2 = 1;
         INT_SINC3 = 1;
         INT_SINC4 = 1;
         Delay_ms(1);
         //INT_SINC = 0;
         INT_SINC1 = 0;
         INT_SINC2 = 0;
         INT_SINC3 = 0;
         INT_SINC4 = 0;
     }

     //Sincroniza el reloj local con el GPS cada hora:
     if ((horaSistema!=0)&&(horaSistema%3600==0)){
        banRespuestaPi = 0;                                                     //No envia respuesta a la RPi
        //Recupera el tiempo del GPS:
        banGPSI = 1;                                                            //Activa la bandera de inicio de trama  del GPS
        banGPSC = 0;                                                            //Limpia la bandera de trama completa
        U1MODE.UARTEN = 1;                                                      //Inicializa el UART1
        //Inicia el Timeout 1:
        T1CON.TON = 1;
        TMR1 = 0;
     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Interrupcion INT2
void int_2() org IVT_ADDR_INT2INTERRUPT {

     INT2IF_bit = 0;                                                            //Limpia la bandera de interrupcion externa INT2

     if (banSyncReloj==1){
         //Cumple en este turno las tareas del pulso SQW:
         AjustarTiempoSistema(horaSistema, fechaSistema, tiempo);
         INT_SINC = ~INT_SINC;                                                  //TEST
         //Genera los pulsos de interrupcion en los nodos:
         //INT_SINC = 1;
         INT_SINC1 = 1;
         INT_SINC2 = 1;
         INT_SINC3 = 1;
         INT_SINC4 = 1;
         Delay_ms(1);
         //INT_SINC = 0;
         INT_SINC1 = 0;
         INT_SINC2 = 0;
         INT_SINC3 = 0;
         INT_SINC4 = 0;

         //Realiza el retraso necesario para sincronizar el RTC con el PPS (Consultar Datasheet del DS3234)
         Delay_ms(499);
         DS3234_setDate(horaSistema, fechaSistema);                             //Configura la hora en el RTC con la hora recuperada de la RPi

         banSyncReloj = 0;
         banSetReloj = 1;                                                       //Activa esta bandera para continuar trabajando con el pulso SQW

         //Sincroniza el tiempo de los nodos cada hora:
         InterrupcionP1(0xB1,0xD1,8);                                           //Envia la hora local a la RPi y a los nodos

     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Timeout de 4*300ms para el UART1:
void Timer1Int() org IVT_ADDR_T1INTERRUPT{

     T1IF_bit = 0;                                                              //Limpia la bandera de interrupcion por desbordamiento del Timer1
     contTimeout1++;                                                            //Incrementa el contador de Timeout

     //Despues de 4 desbordamientos apaga el Timer2 y recupera la hora del RTC:
     if (contTimeout1==4){
        T1CON.TON = 0;
        TMR1 = 0;
        contTimeout1 = 0;
        //Recupera la hora del RTC:
        horaSistema = RecuperarHoraRTC();                                       //Recupera la hora del RTC
        fechaSistema = RecuperarFechaRTC();                                     //Recupera la fecha del RTC
        AjustarTiempoSistema(horaSistema, fechaSistema, tiempo);                //Actualiza los datos de la trama tiempo con la hora y fecha recuperadas del RTC
        fuenteReloj = 7;                                                        //**Indica que se obtuvo la hora del RTC
        InterrupcionP1(0xB1,0xD1,8);                                            //Envia la hora local a la RPi y a los nodos
     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Timeout de 300ms para el UART2:
void Timer2Int() org IVT_ADDR_T2INTERRUPT{

     T2IF_bit = 0;                                                              //Limpia la bandera de interrupcion por desbordamiento del Timer2
     T2CON.TON = 0;                                                             //Apaga el Timer
     TMR2 = 0;

     INT_SINC = ~INT_SINC;//TEST

     //Limpia estas banderas para restablecer la comunicacion por RS485:
     banRSI = 0;
     banRSC = 0;
     i_rs485 = 0;

     //Envia el codigo de error de TimeOut a la RPi:
     numDatosRS485 = 3;
     inputPyloadRS485[0] = 0xD3;
     inputPyloadRS485[1] = 0xEE;
     inputPyloadRS485[2] = 0xE4;
     InterrupcionP1(0xB3,0xD3,3);

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Interrupcion UART1
void urx_1() org  IVT_ADDR_U1RXINTERRUPT {

     //Recupera el byte recibido en cada interrupcion:
     U1RXIF_bit = 0;                                                            //Limpia la bandera de interrupcion por UART
     byteGPS = U1RXREG;                                                         //Lee el byte de la trama enviada por el GPS
     U1STA.OERR = 0;                                                            //Limpia este bit para limpiar el FIFO UART1

     //Recupera el pyload de la trama GPS:                                      //Aqui deberia entrar despues de recuperar la cabecera de trama
     if (banGPSI==3){
        if (byteGPS!=0x2A){
           tramaGPS[i_gps] = byteGPS;                                           //LLena la tramaGPS hasta recibir el ultimo simbolo ("*") de la trama GPS
           i_gps++;
        } else {
           banGPSI = 0;                                                         //Limpia la bandera de inicio de trama
           banGPSC = 1;                                                         //Activa la bandera de trama completa
        }
     }

     //Recupera la cabecera de la trama GPS:                                    //Aqui deberia entrar primero cada vez que se recibe una trama nueva
     if ((banGPSI==1)){
        if (byteGPS==0x24){                                                     //Verifica si el primer byte recibido sea la cabecera de trama "$"
           banGPSI = 2;
           i_gps = 0;
        }
     }
     if ((banGPSI==2)&&(i_gps<6)){
        tramaGPS[i_gps] = byteGPS;                                              //Recupera los datos de cabecera de la trama GPS: ["$", "G", "P", "R", "M", "C"]
        i_gps++;
     }
     if ((banGPSI==2)&&(i_gps==6)){
        //Detiene el Timeout 1:
        T1CON.TON = 0;
        TMR1 = 0;
        //Comprueba la cabecera GPRMC:
        if (tramaGPS[1]=='G'&&tramaGPS[2]=='P'&&tramaGPS[3]=='R'&&tramaGPS[4]=='M'&&tramaGPS[5]=='C'){
           banGPSI = 3;
           i_gps = 0;
        } else {
           banGPSI = 0;
           banGPSC = 0;
           i_gps = 0;
           //Recupera la hora del RTC:
           horaSistema = RecuperarHoraRTC();                                    //Recupera la hora del RTC
           fechaSistema = RecuperarFechaRTC();                                  //Recupera la fecha del RTC
           AjustarTiempoSistema(horaSistema, fechaSistema, tiempo);             //Actualiza los datos de la trama tiempo con la hora y fecha recuperadas del RTC
           fuenteReloj = 5;                                                     //**Fuente de reloj = RTC
           InterrupcionP1(0xB1,0xD1,8);                                         //Envia la hora local a la RPi y a los nodos                                                   //Envia la hora local a la RPi
           banGPSI = 0;
           banGPSC = 0;
           i_gps = 0;
           U1MODE.UARTEN = 0;                                                   //Desactiva el UART1
        }
     }

     //Realiza el procesamiento de la informacion del  pyload:                  //Aqui se realiza cualquier accion con el pyload recuperado
     if (banGPSC==1){
        //Verifica que el caracter 12 sea igual a "A" lo cual comprueba que los datos son validos:
        if (tramaGPS[12]==0x41) {
           for (x=0;x<6;x++){
               datosGPS[x] = tramaGPS[x+1];                                     //Guarda los datos de hhmmss
           }
           //Busca el simbolo "," a partir de la posicion 44
           for (x=44;x<54;x++){
               if (tramaGPS[x]==0x2C){
                   for (y=0;y<6;y++){
                       datosGPS[6+y] = tramaGPS[x+y+1];                         //Guarda los datos de DDMMAA en la trama datosGPS
                   }
               }
           }
           horaSistema = RecuperarHoraGPS(datosGPS);                            //Recupera la hora del GPS
           fechaSistema = RecuperarFechaGPS(datosGPS);                          //Recupera la fecha del GPS
           AjustarTiempoSistema(horaSistema, fechaSistema, tiempo);             //Actualiza los datos de la trama tiempo con la hora y fecha recuperadas del gps
           fuenteReloj = 1;                                                     //Indica que se obtuvo la hora del GPS
           banSyncReloj = 1;
           banSetReloj = 0;
        } else {
           //Recupera la hora del RTC:
           horaSistema = RecuperarHoraRTC();                                    //Recupera la hora del RTC
           fechaSistema = RecuperarFechaRTC();                                  //Recupera la fecha del RTC
           AjustarTiempoSistema(horaSistema, fechaSistema, tiempo);             //Actualiza los datos de la trama tiempo con la hora y fecha recuperadas del RTC
           fuenteReloj = 6;                                                     //**Indica que se obtuvo la hora del RTC
           InterrupcionP1(0xB1,0xD1,8);                                         //Envia la hora local a la RPi y a los nodos
        }

        banGPSI = 0;
        banGPSC = 0;
        i_gps = 0;
        U1MODE.UARTEN = 0;                                                      //Desactiva el UART1

     }

}
//*****************************************************************************************************************************************

//*****************************************************************************************************************************************
//Interrupcion UART2
void urx_2() org  IVT_ADDR_U2RXINTERRUPT {

     //Recupera el byte recibido en cada interrupcion:
     U2RXIF_bit = 0;                                                            //Limpia la bandera de interrupcion por UART2
     byteRS485 = U2RXREG;                                                       //Lee el byte de la trama enviada por el nodo
     U2STA.OERR = 0;                                                            //Limpia este bit para limpiar el FIFO UART2

     //Recupera el pyload de la trama RS485:                                    //Aqui deberia entrar despues de recuperar la cabecera de trama
     if (banRSI==2){
        //Recupera el pyload de final de trama:
        if (i_rs485<(numDatosRS485)){
           inputPyloadRS485[i_rs485] = byteRS485;
           i_rs485++;
        } else {
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
        tramaCabeceraRS485[i_rs485] = byteRS485;                                 //Recupera los datos de cabecera de la trama UART: [0x3A, Direccion, Funcion, NumeroDatos]
        i_rs485++;
     }
     if ((banRSI==1)&&(i_rs485==5)){
        //Detiene el Timeout 2:
        T2CON.TON = 0;
        TMR2 = 0;
        //Comprueba la direccion del nodo solicitado:
        if (tramaCabeceraRS485[1]==direccionRS485){
           funcionRS485 = tramaCabeceraRS485[2];
           *(ptrnumDatosRS485) = tramaCabeceraRS485[3];                         //LSB numDatosRS485
           *(ptrnumDatosRS485+1) = tramaCabeceraRS485[4];                       //MSB numDatosRS485
           banRSI = 2;
           i_rs485 = 0;
        } else {
           banRSI = 0;
           banRSC = 0;
           i_rs485 = 0;
        }
     }

     //Realiza el procesamiento de la informacion del  pyload:                  //Aqui se realiza cualquier accion con el pyload recuperado
     if (banRSC==1){
        subFuncionRS485 = inputPyloadRS485[0];
        switch (funcionRS485){
               case 0xF1:
                    InterrupcionP1(0xB1,subFuncionRS485,numDatosRS485);
                    break;
               case 0xF3:
                    InterrupcionP1(0xB3,subFuncionRS485,numDatosRS485);
                    break;
        }

        banRSC = 0;

     }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////