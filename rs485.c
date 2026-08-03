/*-------------------------------------------------------------------------------------------------------------------------
RS485.c
Autor: David Timbi

Modulo compartido para el bus RS485 principal de datos.
Gestiona la transmision de tramas y el control de direccion del MAX485.

Formato de trama:
   [0] 0x3A          Inicio de trama
   [1] Direccion     Nodo destino (255 = broadcast)
   [2] Funcion       Codigo de comando
   [3] numDatos LSB  Longitud del payload (LSB)
   [4] numDatos MSB  Longitud del payload (MSB)
   [5..N] Payload    Datos de la funcion
---------------------------------------------------------------------------------------------------------------------------*/

#ifndef _RS485_C
#define _RS485_C

//////////////////////////////////////////////////////////////  CONSTANTES  //////////////////////////////////////////////////////////////

#define RS485_BYTE_INICIO     0x3A
#define RS485_PUERTO_UART1    1
#define RS485_PUERTO_UART2    2
#define RS485_DIR_BROADCAST   255

//Trama fija de prueba, usada unicamente por EnviarTramaPruebaRS485() para validar el enlace fisico:
unsigned char tramaPruebaRS485[10] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//*****************************************************************************************************************************************
// RS485_EsperarFinTx
// Espera a que el ultimo bit haya salido fisicamente por la linea antes de
// liberar el MAX485 para recepcion.
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