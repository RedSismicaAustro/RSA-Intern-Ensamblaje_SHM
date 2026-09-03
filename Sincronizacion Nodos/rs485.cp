#line 1 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/rs485.c"
#line 28 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/rs485.c"
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
#line 74 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/rs485.c"
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
