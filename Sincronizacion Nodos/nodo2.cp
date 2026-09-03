#line 1 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/nodo2.c"
#line 25 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/nodo2.c"
sbit TEST1 at LATA2_bit;
sbit TEST1_Direction at TRISA2_bit;

sbit MSRS485 at LATB12_bit;
sbit MSRS485_Direction at TRISB12_bit;
#line 1 "c:/users/public/documents/mikroelektronika/mikroc pro for dspic/examples/rs485.c"
#line 28 "c:/users/public/documents/mikroelektronika/mikroc pro for dspic/examples/rs485.c"
unsigned char tramaPruebaRS485[10] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};








static void RS485_EsperarFinTx(unsigned short puerto){

 if (puerto== 1 ){
 while(!U1STAbits.TRMT);
 } else {
 while(!U2STAbits.TRMT);
 }

}





static void RS485_EscribirByte(unsigned short puerto, unsigned char dato){

 if (puerto== 1 ){
 UART1_Write(dato);
 } else {
 UART2_Write(dato);
 }

}
#line 74 "c:/users/public/documents/mikroelektronika/mikroc pro for dspic/examples/rs485.c"
void EnviarTramaRS485(unsigned short puerto, unsigned short direccion, unsigned short funcion, unsigned int numDatos, unsigned char *payload){

 unsigned int k;
 unsigned char *ptrNumDatos;

 ptrNumDatos = (unsigned char *) &numDatos;

 MSRS485 = 1;
 Delay_us(10);

 RS485_EscribirByte(puerto,  0x3A );
 RS485_EscribirByte(puerto, direccion);
 RS485_EscribirByte(puerto, funcion);
 RS485_EscribirByte(puerto, *(ptrNumDatos));
 RS485_EscribirByte(puerto, *(ptrNumDatos+1));

 for (k=0; k<numDatos; k++){
 RS485_EscribirByte(puerto, payload[k]);
 }

 RS485_EsperarFinTx(puerto);
 Delay_us(10);
 MSRS485 = 0;

}








void EnviarTramaPruebaRS485(unsigned short direccion){

 EnviarTramaRS485( 2 , direccion, 0xF3, 10, tramaPruebaRS485);

}
#line 46 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/nodo2.c"
unsigned int i, j, x;


unsigned short banRSI, banRSC;
unsigned char byteRS485;
unsigned int i_rs485;
unsigned char tramaCabeceraRS485[10];
unsigned char inputPyloadRS485[15];
unsigned char outputPyloadRS485[15];
unsigned int numDatosRS485;
unsigned char *ptrnumDatosRS485;
unsigned short funcionRS485;
unsigned short subFuncionRS485;
unsigned short contTMR2;


void ConfiguracionPrincipal();


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

 MSRS485 = 0;

 while(1){
 asm CLRWDT;
 Delay_ms(100);
 }

}





void ConfiguracionPrincipal(){


 CLKDIVbits.FRCDIV = 0;
 CLKDIVbits.PLLPOST = 0;
 CLKDIVbits.PLLPRE = 5;
 PLLFBDbits.PLLDIV = 150;


 ANSELA = 0;
 ANSELB = 0;
 TEST1_Direction = 0;
 MSRS485_Direction = 0;
 TRISB14_bit = 1;

 INTCON2.GIE = 1;


 RPINR18bits.U1RXR = 0x2F;
 RPOR1bits.RP36R = 0x01;
 U1RXIE_bit = 1;
 U1STAbits.URXISEL = 0x00;
 U1RXIF_bit = 0;
 IPC2bits.U1RXIP = 0x04;
 UART1_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED);


 RPINR0 = 0x2E00;
 INT1IE_bit = 1;
 INT1IF_bit = 0;
 IPC5bits.INT1IP = 0x01;


 T2CON = 0x0030;
 T2CON.TON = 0;
 T2IE_bit = 1;
 T2IF_bit = 0;
 PR2 = 46875;
 IPC1bits.T2IP = 0x02;

 Delay_ms(200);

}
#line 146 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/nodo2.c"
void int_1() org IVT_ADDR_INT1INTERRUPT {

 INT1IF_bit = 0;

 TEST1 = ~TEST1;

}




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





void urx_1() org IVT_ADDR_U1RXINTERRUPT {

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
 if (byteRS485== 0x3A ){
 banRSI = 1;
 i_rs485 = 0;
 }
 }
 if ((banRSI==1)&&(i_rs485<5)){
 tramaCabeceraRS485[i_rs485] = byteRS485;
 i_rs485++;
 }
 if ((banRSI==1)&&(i_rs485==5)){
 if ((tramaCabeceraRS485[1]== 2 )||(tramaCabeceraRS485[1]== 255 )){
 funcionRS485 = tramaCabeceraRS485[2];
 *(ptrnumDatosRS485) = tramaCabeceraRS485[3];
 *(ptrnumDatosRS485+1) = tramaCabeceraRS485[4];
 banRSI = 2;
 i_rs485 = 0;
 } else {

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


 if ((funcionRS485==0xF1)&&(subFuncionRS485==0xD2)){
 outputPyloadRS485[0] = 0xD2;
 outputPyloadRS485[1] =  2 ;
 delay_ms(10);
 EnviarTramaRS485( 1 ,  2 , 0xF1, 2, outputPyloadRS485);
 }

 banRSC = 0;
 banRSI = 0;
 }

}
