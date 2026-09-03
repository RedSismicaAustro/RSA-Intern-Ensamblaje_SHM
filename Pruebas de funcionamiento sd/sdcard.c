
// Libreria para inicializar, leer y escribir sectores de 512 bytes en una
// tarjeta SD mediante SPI. No utiliza un sistema de archivos: accede
// directamente a los sectores logicos de la tarjeta.
//
// La aplicacion debe declarar los pines de Chip Select y Card Detect, ademas
// de la estructura sdflags, antes de utilizar estas funciones.

// Dependencias de la capa SPI y de las definiciones de la tarjeta SD.
#include "spiSD.h"
#include "sdcard.h"
// Tipos booleanos utilizados por las banderas de estado.
#include <stdbool.h>
//#define SD_TIME_OUT 100

// Pines de Chip Select declarados por la aplicacion.
extern sfr sbit sd_CS_lat; // CS bajo: transaccion SPI activa.

extern sfr sbit sd_CS_tris; // Direccion del CS: 1 entrada, 0 salida.

// Pines de deteccion de la tarjeta SD declarados por la aplicacion.
extern sfr sbit sd_detect_port;
extern sfr sbit sd_detect_tris;

// Banderas de estado de la tarjeta, declaradas e inicializadas por la aplicacion.
extern struct sdflags sdflags;

// Card Capacity Status obtenido durante la inicializacion.
unsigned char ccs;
unsigned int j_spi;  // Indice auxiliar para ciclos SPI.

// Genera un patron de parpadeo en el LED de diagnostico.
void LEDcard(int veces, float tiempo_seg){
     unsigned int i;
     TRISAbits.TRISA1 = 0; // Configura RA1 como salida
     if (tiempo_seg==1){
       for (i=0; i<veces; i++){
           LATAbits.LATA1 = 1; // Enciende el LED
           Delay_ms(1000);   // Espera
           LATAbits.LATA1 = 0; // Apaga el LED
           Delay_ms(1000);   // Espera
       }
     }else if (tiempo_seg==2){
       for (i=0; i<veces; i++){
           LATAbits.LATA1 = 1; // Enciende el LED
           Delay_ms(350);   // Espera
           LATAbits.LATA1 = 0; // Apaga el LED
           Delay_ms(350);   // Espera
       }
     }
}

// Lee una cantidad variable de bytes de la tarjeta SD.
unsigned char SD_Read(unsigned char *Buffer, unsigned int nbytes){
    unsigned int i;
    unsigned char temp;
    for(i = 0; i < SD_TIME_OUT; i++){
        temp = SPISD_Write(0xFF);
        if(temp == 0xFE) break;
        if(i == SD_TIME_OUT-1) return TOKEN_NOT_RECEIVED;
                //if(i == SD_TIME_OUT-1) return 0xEE;
    }
    for(i = 0; i < nbytes; i++){
        Buffer[i] = SPISD_Write(0xFF);
    }
    temp = SPISD_Write(0xFF);     // Lee el primer byte del CRC.
    temp = SPISD_Write(0xFF);     //
    return 0x00;                  // Lectura correcta.
}
// *****************************************************************************
// **************************** Fin SD_Read ************************************
// *****************************************************************************



// Lee un sector completo de 512 bytes.
unsigned char SD_Read_Block(unsigned char *Buffer, unsigned long Address){
    unsigned char temp;
    Select_SD();

    if(ccs == 0x02) Address<<=9;                      // Address * 512 for SDSC cards
    if(SD_Ready() == 0) return SD_NOT_READY;
    SD_Send_Command(READ_SINGLE_BLOCK,Address,0xFF);
    temp = R1_Response();
    if(temp == 0xFF){LEDcard(10,2);}
    if(temp != 0x00) return temp;
    temp = SD_Read(Buffer,512);

    Release_SD();
    return temp;
}
// *****************************************************************************
// ********************** Fin metodo SD_Read_Block *****************************
// *****************************************************************************



// Escribe un sector completo de 512 bytes.
unsigned char SD_Write_Block(unsigned char *Buffer, unsigned long Address){
    unsigned char temp;
    unsigned int i;

    // Activa el Chip Select.
    Select_SD();

    if(ccs == 0x02) Address<<=9;        // Address * 512 for SDSC cards
    if(SD_Ready() == 0) return SD_NOT_READY;
    SD_Send_Command(WRITE_BLOCK,Address,0xFF);
    temp = R1_Response();
    if(temp != 0x00) return temp;
    temp = SPISD_Write(0xFE);    // Envia el token de inicio del bloque.
    for(i = 0; i < 512; i++){
        temp = SPISD_Write(Buffer[i]);
    }
    temp = SPISD_Write(0xFF);        // Envia un CRC ficticio de 16 bits.
    temp = SPISD_Write(0xFF);

    // Espera el token de respuesta de datos (0bxxx00101 = 0x05).
    for(i = 0; i < 64; i++){
        temp = SPISD_Write(0xFF);
        if((temp & 0x11) == 0x01) break; // Token valido.
    }

    // El token de aceptacion de datos es 0x05 (0bxxx00101).
    if((temp & 0x1F) == 0x05) {
        // Espera activa a que la tarjeta SD termine la grabacion interna en flash (MISO vuelve a 0xFF)
        for(i = 0; i < 65535; i++){
            if(SPISD_Write(0xFF) == 0xFF) break;
        }
        Release_SD();
        return DATA_ACCEPTED;
    } else {
        Release_SD();
        return ERROR;
    }
}
// *****************************************************************************
// ********************* Fin metodo SD_Write_Block *****************************
// *****************************************************************************



// Intenta inicializar la tarjeta SD el numero de veces indicado.
unsigned char SD_Init_Try(unsigned char try_value){
    unsigned char i,init_status;
    if(try_value == 0) try_value = 1;
    for(i = 0; i < try_value; i++){
        init_status = SD_Init();
        if(init_status == SUCCESSFUL_INIT) break;
        Release_SD();
        Delay_ms(10);
    }
    return init_status;
}
// *****************************************************************************
// ************************ Fin metodo SD_Init_Try *****************************
// *****************************************************************************



// Inicializa la tarjeta SD y devuelve el resultado de la operacion.
unsigned char SD_Init(void){
    unsigned int i, j_spi;
    unsigned char temp;
    unsigned long temp_long;

    sd_CS_tris = 0;
    Release_SD();

    // Inicializa el SPI a baja velocidad (156 kHz, inferior a 400 kHz).
    SPISD_Init(SLOW);

    // Genera al menos 80 ciclos con CS desactivado.
    for(i = 0; i < 80; i++) SPISD_Write(0xFF);

    Select_SD();

    // Ciclos adicionales de reloj tras activar CS (bajo)
    for(i = 0; i < 16; i++) SPISD_Write(0xFF);

    // 1. CMD0: Poner tarjeta en estado IDLE (debe responder 0x01)
    for(i = 0; i < 200; i++){
        Release_SD();
        Delay_ms(2);
        Select_SD();
        for(j_spi = 0; j_spi < 4; j_spi++) SPISD_Write(0xFF);
        SD_Send_Command(GO_IDLE_STATE, 0x00000000, 0x4A);
        temp = R1_Response();
        if(temp == (1<<IDLE_STATE)) break;
    }

    if(temp != (1<<IDLE_STATE)) {
        Release_SD();
        return CARD_NOT_INSERTED;
    }

    // 2. CMD8: Verificar condicion de operacion (SD V2.0 / SDHC)
    SD_Send_Command(SEND_IF_COND, 0x000001AA, 0x43);
    temp = R1_Response();

    if(temp == (1<<IDLE_STATE)){
        temp_long = Response_32b();
        temp = (temp_long & ECHO_BACK_MASK);
        if(temp != 0xAA) {
            Release_SD();
            return ECHO_BACK_ERROR;
        }

        // 3. ACMD41: Polling de inicializacion (CMD55 + ACMD41)
        for(i = 0; i < 1000; i++){
            SD_Send_Command(APP_CMD, 0x00000000, 0x32);
            temp = R1_Response();

            for(j_spi = 0; j_spi < 16; j_spi++) SPISD_Write(0xFF);

            SD_Send_Command(SD_SEND_OP_COND, 0x40000000, 0x3B);
            temp = R1_Response();
            if(temp == 0x00) break;  // Inicializacion completada exitosamente!
            Delay_ms(1);
            if(i == 999) {
                Release_SD();
                return UNUSABLE_CARD;
            }
        }
    } else {
        // Tarjeta antigua (CMD1)
        for(i = 0; i < 1000; i++){
            SD_Send_Command(SEND_OP_COND, 0x00000000, 0x7C);
            temp = R1_Response();
            if(temp == 0x00) break;
            Delay_ms(1);
            if(i == 999) {
                Release_SD();
                return UNUSABLE_CARD;
            }
        }
    }

    // 4. CMD16: Configurar longitud de bloque a 512 bytes
    SD_Send_Command(SET_BLOCKLEN, 0x00000200, 0x0A);
    temp = R1_Response();

    // 5. CMD58: Leer OCR y capacidad (CCS)
    SD_Send_Command(READ_OCR, 0x00000000, 0x7E);
    temp = R1_Response();
    if(temp != 0x00) {
        Release_SD();
        return temp;
    }
    temp_long = Response_32b();

    // Validar rango de voltaje real en el registro OCR (2.7V - 3.6V)
    if((temp_long & 0x00FF8000) != 0x00FF8000) {
        Release_SD();
        return INCOMPATIBLE_VOLTAGE;
    }
    ccs = (long)(temp_long >> 30);

    Release_SD();

    for(i = 0; i < 16; i++) SPISD_Write(0xFF);

    // Cambia el SPI a alta velocidad tras una inicializacion correcta.
    SPISD_Init(FAST);

    return SUCCESSFUL_INIT;
}
// *****************************************************************************
// ************************** Fin metodo SD_Init *******************************
// *****************************************************************************


// Lee una respuesta R1 de la tarjeta SD.
unsigned char R1_Response(void){
    unsigned char temp;
    unsigned int retry;
    // La SD puede tardar hasta NCR (1-8 bytes) en responder.
    // Esperamos hasta 16 lecturas a que bit7 sea 0 (respuesta valida).
    for(retry = 0; retry < 16; retry++){
        temp = SPISD_Write(0xFF);
        if((temp & 0x80) == 0) return temp;  // bit7=0 -> respuesta R1 valida
    }
    return 0xFF;  // Timeout: no hubo respuesta
}
// *****************************************************************************
// ************************* Fin metodo R1_Response ****************************
// *****************************************************************************


// Lee una respuesta R2 de la tarjeta SD.
unsigned int R2_Response(void){
    unsigned char temp;
    unsigned int response;
    temp = SPISD_Write(0xFF);
    response = SPISD_Write(0xFF);
    temp = SPISD_Write(0xFF);
    response = (response<<8)|temp;
    return response;
}
// *****************************************************************************
// ************************* Fin metodo R2_Response ****************************
// *****************************************************************************


// *****************************************************************************
// ************************** Metodo Response_32b ******************************
// *****************************************************************************
unsigned long Response_32b(void){
    unsigned char temp;
    unsigned long response;
    response = SPISD_Write(0xFF);
    temp = SPISD_Write(0xFF);
    response = (response<<8)|temp;
    temp = SPISD_Write(0xFF);
    response = (response<<8)|temp;
    temp = SPISD_Write(0xFF);
    response = (response<<8)|temp;
    return response;
}
// *****************************************************************************
// *********************** Fin metodo Response_32b *****************************
// *****************************************************************************


// *****************************************************************************
// ****************** Metodo para enviar un comando a la SD ********************
// *****************************************************************************
void SD_Send_Command(unsigned char command,unsigned long argument, unsigned char crc){

    // transmit command to sd card
    SPISD_Write(command |= 0x40);

    // transmit argument
    SPISD_Write((unsigned char)(argument>>24));
    SPISD_Write((unsigned char)(argument>>16));
    SPISD_Write((unsigned char)(argument>>8));
    SPISD_Write((unsigned char)(argument));

    // transmit crc
    SPISD_Write((crc<<1)|0x01);
}
// *****************************************************************************
// ************************** Fin SD_Send_Command ******************************
// *****************************************************************************



// *****************************************************************************
// Metodo que devuelve un valor igual a 0 si la SD tiene algun problema y no
// esta lista para usar. Si la SD esta lista devuelve un valor distinto de 0
// *****************************************************************************
unsigned char SD_Ready(void){
    unsigned int i;
    unsigned char temp;
    for(i = 0; i < 10000; i++){
        temp = SPISD_Write(0xFF);
        if(temp == 0xFF) break;
        if(i == 9999) return 0x00;
    }
    return temp;
}
// *****************************************************************************
// ****************************** Fin SD_Ready *********************************
// *****************************************************************************



// *****************************************************************************
// Metodo que coloca el pin Chip Select en 1, cuando se llama a este metodo no
// es posible ni leer ni escribir en la SD
// *****************************************************************************
void Release_SD(void){
    // Libera el Chip Select y envia ocho pulsos si el SPI esta habilitado.
    sd_CS_lat = 1;
    if(SPI1STATbits.SPIEN){
        SPISD_Write(0xFF);
    }
}
// *****************************************************************************
// ****************************** Fin Release_SD *******************************
// *****************************************************************************



// *****************************************************************************
// Metodo para colocar el pin del Chip Select en 0 , para activar la lectura y
// escritura de la SD
// *****************************************************************************
void Select_SD(void){
    // Coloca el Chip Select en 0
    sd_CS_lat = 0;
    asm nop;
}
// *****************************************************************************
// ***************************** Fin Select_SD *********************************
// *****************************************************************************



// *****************************************************************************
// Metodo que permite detectar si esta o no conectada la tarjeta SD, en el caso
// de que este conectada devuelve DETECTED (valor 0xDE), caso contrario 0x00
// En funcion del pin
// *****************************************************************************
unsigned char SD_Detect(void) {
    // Un nivel bajo indica que la tarjeta esta conectada.
    if (sd_detect_port == 0) {
         return DETECTED;
    // Un nivel alto indica que no hay tarjeta.
    } else {
         return 0;
    }
}
// *****************************************************************************
// ***************************** Fin SD_Detect *********************************
// *****************************************************************************