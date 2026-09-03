#line 1 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/Concentrador.c"
#line 27 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/Concentrador.c"
sbit RP1 at LATA4_bit;
sbit RP1_Direction at TRISA4_bit;
sbit MSRS485 at LATB11_bit;
sbit MSRS485_Direction at TRISB11_bit;

sbit INT_SINC_1 at LATA0_bit;
sbit INT_SINC_1_Direction at TRISA0_bit;

sbit INT_SINC at LATA1_bit;
sbit INT_SINC_Direction at TRISA1_bit;

sbit INT_SINC_2 at LATA3_bit;
sbit INT_SINC_2_Direction at TRISA3_bit;
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
#line 54 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/Concentrador.c"
unsigned int i, j, x;






unsigned short banLec, banEsc;
unsigned char *ptrnumBytesSPI;
unsigned char tramaSolicitudSPI[10];
unsigned char tramaSolicitudNodo[10];
unsigned short banInicio;
unsigned short banRespuestaPi;
unsigned short banSPI0, banSPI1, banSPI2, banSPI7, banSPI8, banSPIA;
unsigned short bufferSPI;


unsigned short banRSI, banRSC;
unsigned char byteRS485;
unsigned int i_rs485;
unsigned char tramaCabeceraRS485[10];
unsigned char inputPyloadRS485[2600];
unsigned char outputPyloadRS485[15];
unsigned short direccionRS485;
unsigned short funcionRS485;
unsigned short subFuncionRS485;
unsigned int numDatosRS485;
unsigned char *ptrnumDatosRS485;


unsigned short banInicioMuestreo;





void ConfiguracionPrincipal();
void InterrupcionP1(unsigned short funcionSPI, unsigned short subFuncionSPI, unsigned int numBytesSPI);
void CambiarEstadoBandera(unsigned short bandera, unsigned short estado);




void main() {

 ConfiguracionPrincipal();


 i = 0; j = 0; x = 0;


 banSPI0 = 0; banSPI1 = 0; banSPI2 = 0; banSPI7 = 0; banSPI8 = 0; banSPIA = 0;
 banRespuestaPi = 0;


 banRSI = 0;
 banRSC = 0;
 byteRS485 = 0;
 i_rs485 = 0;
 funcionRS485 = 0;
 subFuncionRS485 = 0;
 numDatosRS485 = 0;
 ptrnumDatosRS485 = (unsigned char *) &numDatosRS485;


 banInicioMuestreo = 0;


 RP1 = 0;
 INT_SINC = 1;
 INT_SINC_1 = 0;
 INT_SINC_2 = 0;

 MSRS485 = 0;

 SPI1BUF = 0x00;

 T3CON.TON = 1;

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

 INT_SINC_Direction = 0;
 INT_SINC_1_Direction = 0;
 INT_SINC_2_Direction = 0;
 RP1_Direction = 0;
 MSRS485_Direction = 0;

 INTCON2.GIE = 1;


 RPINR19bits.U2RXR = 0x2F;
 RPOR1bits.RP36R = 0x03;
 U2RXIE_bit = 1;
 IPC7bits.U2RXIP = 0x04;
 U2STAbits.URXISEL = 0x00;
 UART2_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED);


 SPI1STAT.SPIEN = 1;
 SPI1_Init_Advanced(_SPI_SLAVE, _SPI_8_BIT, _SPI_PRESCALE_SEC_1, _SPI_PRESCALE_PRI_1, _SPI_SS_ENABLE, _SPI_DATA_SAMPLE_END, _SPI_CLK_IDLE_HIGH, _SPI_ACTIVE_2_IDLE);
 SPI1IF_bit = 0;
 IPC2bits.SPI1IP = 0x03;


 T2CON = 0x30;
 T2CON.TON = 0;
 T2IE_bit = 1;
 T2IF_bit = 0;
 PR2 = 46875;
 IPC1bits.T2IP = 0x02;


 T3CON = 0x30;
 T3CON.TON = 0;
 T3IE_bit = 1;
 T3IF_bit = 0;
 PR3 = 15625;
 IPC2bits.T3IP = 0x02;


 SPI1IE_bit = 1;

 Delay_ms(200);

}




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









void spi_1() org IVT_ADDR_SPI1INTERRUPT {

 SPI1IF_bit = 0;
 bufferSPI = SPI1BUF;


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


 if ((banSPI1==0)&&(bufferSPI==0xA1)){
 CambiarEstadoBandera(1,1);
 i = 0;
 }
 if ((banSPI1==1)&&(bufferSPI!=0xA1)&&(bufferSPI!=0xF1)){
 tramaSolicitudSPI[i] = bufferSPI;
 i++;
 }
 if ((banSPI1==1)&&(bufferSPI==0xF1)){
 direccionRS485 = tramaSolicitudSPI[0];
 outputPyloadRS485[0] = 0xD1;
 outputPyloadRS485[1] = tramaSolicitudSPI[1];
 EnviarTramaRS485( 2 , direccionRS485, 0xF2, 2, outputPyloadRS485);
 CambiarEstadoBandera(1,0);
 }


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
 EnviarTramaRS485( 2 , direccionRS485, 0xF2, 1, outputPyloadRS485);
 CambiarEstadoBandera(2,0);
 }


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
 EnviarTramaRS485( 2 , direccionRS485, 0xF1, 1, outputPyloadRS485);
 T2CON.TON = 1;
 TMR2 = 0;
 banRespuestaPi = 1;
 CambiarEstadoBandera(7,0);
 }


 if ((banSPI8==0)&&(bufferSPI==0xA8)){
 CambiarEstadoBandera(8,1);
 i = 0;
 }
 if ((banSPI8==1)&&(i<4)){
 tramaSolicitudNodo[i] = bufferSPI;
 i++;
 }
 if ((banSPI8==1)&&(i==4)){
 direccionRS485 = tramaSolicitudNodo[1];
 funcionRS485 = tramaSolicitudNodo[2];
 numDatosRS485 = tramaSolicitudNodo[3];
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
 EnviarTramaRS485( 2 , direccionRS485, funcionRS485, numDatosRS485, outputPyloadRS485);
 if (direccionRS485 !=  255 ) {
 T2CON.TON = 1;
 TMR2 = 0;
 }
 }


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




void Timer3Int() org IVT_ADDR_T3INTERRUPT{

 static unsigned int contadorSync = 0;

 T3IF_bit = 0;
 contadorSync++;

 if (contadorSync >= ( 1000 /100)){
 contadorSync = 0;

 INT_SINC = ~INT_SINC;


 INT_SINC_1 = 1;
 Delay_us( 1000 );
 INT_SINC_1 = 0;
 }

}




void Timer2Int() org IVT_ADDR_T2INTERRUPT{

 T2IF_bit = 0;
 T2CON.TON = 0;
 TMR2 = 0;

 INT_SINC = ~INT_SINC;

 banRSI = 0;
 banRSC = 0;
 i_rs485 = 0;


 numDatosRS485 = 3;
 inputPyloadRS485[0] = 0xD3;
 inputPyloadRS485[1] = 0xEE;
 inputPyloadRS485[2] = 0xE4;
 banRespuestaPi = 1;
 InterrupcionP1(0xB3,0xD3,3);

}




void urx_2() org IVT_ADDR_U2RXINTERRUPT {

 U2RXIF_bit = 0;
 byteRS485 = U2RXREG;
 U2STA.OERR = 0;


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
 if (tramaCabeceraRS485[1]==direccionRS485){
 T2CON.TON = 0;
 TMR2 = 0;
 funcionRS485 = tramaCabeceraRS485[2];
 *(ptrnumDatosRS485) = tramaCabeceraRS485[3];
 *(ptrnumDatosRS485+1) = tramaCabeceraRS485[4];
 banRSI = 2;
 i_rs485 = 0;
 } else {
 banRSI = 0;
 banRSC = 0;
 i_rs485 = 0;
 }
 }


 if (banRSC==1){
 subFuncionRS485 = inputPyloadRS485[0];
 banRespuestaPi = 1;

 INT_SINC_2 = ~INT_SINC_2;

 switch (funcionRS485){
 case 0xF1: InterrupcionP1(0xB1,subFuncionRS485,numDatosRS485); break;
 case 0xF3: InterrupcionP1(0xB3,subFuncionRS485,numDatosRS485); break;
 }
 banRSC = 0;
 }
}
