/*-------------------------------------------------------------------------------------------------------------------------
RS485.c
Modulo de gestion del bus RS485 principal (datos) del Nodo Concentrador.

Hardware: dsPIC33EP256MC202 + MAX485 #1 (bidireccional, control de
direccion por el pin MSRS485). El segundo MAX485 (TX permanente,
pulso de sincronizacion INT_SINC_1) NO se maneja aqui, ver
Nodo_Concentrador_Sync.c

Formato de trama (coincide con lo que ya espera main.c en urx_2 / spi_1):
   [0] 0x3A          -> Byte de inicio de trama
   [1] Direccion     -> Direccion del nodo destino (255 = Broadcast)
   [2] Funcion       -> Codigo de funcion solicitada (0xF1, 0xF2, 0xF3...)
   [3] numDatos LSB  -> Cantidad de bytes del payload (LSB)
   [4] numDatos MSB  -> Cantidad de bytes del payload (MSB)
   [5..N] Payload    -> Datos asociados a la funcion


---------------------------------------------------------------------------------------------------------------------------*/

#ifndef _RS485_C
#define _RS485_C

//////////////////////////////////////////////////////////////  Constantes  //////////////////////////////////////////////////////////////

#define RS485_BYTE_INICIO     0x3A
#define RS485_PUERTO_UART1    1
#define RS485_PUERTO_UART2    2
#define RS485_DIR_BROADCAST   255

//Trama fija de prueba, usada unicamente por EnviarTramaPruebaRS485() para validar el enlace fisico:
unsigned char tramaPruebaRS485[10] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//*****************************************************************************************************************************************
// Espera a que el ultimo bit haya salido fisicamente por la linea (registro de corrimiento vacio)
// antes de soltar el bus, evitando cortar la trama al conmutar el MAX485 a recepcion.
static void RS485_EsperarFinTx(unsigned short puerto){

     if (puerto==RS485_PUERTO_UART1){
        while(!U1STAbits.TRMT);
     } else {
        while(!U2STAbits.TRMT);
     }

}
//*****************************************************************************************************************************************


//*****************************************************************************************************************************************
// Escribe un byte por el UART indicado (1 o 2)
static void RS485_EscribirByte(unsigned short puerto, unsigned char dato){

     if (puerto==RS485_PUERTO_UART1){
        UART1_Write(dato);
     } else {
        UART2_Write(dato);
     }

}
//*****************************************************************************************************************************************


//*****************************************************************************************************************************************
// EnviarTramaRS485
// Arma y transmite una trama de comando por el bus RS485 principal (datos), controlando
// el MAX485 #1 (bidireccional) mediante el pin MSRS485 (0=Recepcion, 1=Transmision).
//
//   puerto    : UART fisico a usar (RS485_PUERTO_UART1 o RS485_PUERTO_UART2)
//   direccion : direccion del nodo destino (RS485_DIR_BROADCAST = todos los nodos)
//   funcion   : codigo de funcion solicitada
//   numDatos  : cantidad de bytes del payload
//   payload   : puntero al arreglo de datos a enviar (longitud >= numDatos)
//*****************************************************************************************************************************************
void EnviarTramaRS485(unsigned short puerto, unsigned short direccion, unsigned short funcion, unsigned int numDatos, unsigned char *payload){

     unsigned int k;
     unsigned char *ptrNumDatos;

     ptrNumDatos = (unsigned char *) &numDatos;

     MSRS485 = 1;                                    // Habilita el MAX485 de datos en modo transmision (DE/RE = 1)
     Delay_us(10);                                    // Tiempo de establecimiento del transceptor (margen sobre datasheet MAX485)

     RS485_EscribirByte(puerto, RS485_BYTE_INICIO);   // [0] Cabecera
     RS485_EscribirByte(puerto, direccion);           // [1] Direccion del nodo destino
     RS485_EscribirByte(puerto, funcion);             // [2] Funcion solicitada
     RS485_EscribirByte(puerto, *(ptrNumDatos));      // [3] numDatos LSB
     RS485_EscribirByte(puerto, *(ptrNumDatos+1));    // [4] numDatos MSB

     for (k=0; k<numDatos; k++){
         RS485_EscribirByte(puerto, payload[k]);      // [5..N] Payload
     }

     RS485_EsperarFinTx(puerto);                      // Espera a que el ultimo byte salga fisicamente por la linea
     Delay_us(10);
     MSRS485 = 0;                                     // Regresa el MAX485 a modo recepcion para escuchar la respuesta del nodo

}
//*****************************************************************************************************************************************


//*****************************************************************************************************************************************
// EnviarTramaPruebaRS485
// Variante de utilidad para probar el enlace fisico con un nodo (funcion 0xF3 en tu protocolo),
// reutiliza EnviarTramaRS485 con un payload fijo de prueba.
//*****************************************************************************************************************************************
void EnviarTramaPruebaRS485(unsigned short direccion){

     EnviarTramaRS485(RS485_PUERTO_UART2, direccion, 0xF3, 10, tramaPruebaRS485);

}
//*****************************************************************************************************************************************

#endif
