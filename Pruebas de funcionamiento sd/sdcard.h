#ifndef _SDCARD_H_
#define _SDCARD_H_

/*
 * sdcard.h - Constantes y prototipos para sdcard.c
 *
 * Los comandos definidos en este archivo corresponden al protocolo SD
 * utilizado en modo SPI.
 *
 * VOLTAGE_RANGE_MASK representa el rango de 2.7 V a 3.6 V del registro OCR.
 */

#include <stdbool.h>

// Tiempo maximo de espera expresado en intentos de polling.
#define SD_TIME_OUT 200

// Comandos SD para el modo SPI.
#define GO_IDLE_STATE     0    // CMD0
#define SEND_IF_COND      8    // CMD8
#define SEND_OP_COND      1    // CMD1
#define READ_OCR          58   // CMD58
#define CRC_ON_OFF        59   // CMD59
#define APP_CMD           55   // CMD55
#define SD_SEND_OP_COND   41   // ACMD41
#define SET_BLOCKLEN      16   // CMD16
#define READ_SINGLE_BLOCK 17   // CMD17
#define WRITE_BLOCK       24   // CMD24

// Bit de estado IDLE dentro de la respuesta R1.
#define IDLE_STATE 0           // bit0 de R1 -> (1<<IDLE_STATE) = 0x01

// Mascaras para las respuestas de 32 bits de CMD8 y CMD58.
#define ECHO_BACK_MASK        0x000000FF   // bits[7:0] de R7  (patron de eco de CMD8)
#define VOLTAGE_ACCEPTED_MASK 0x00000F00   // bits[11:8] de R7 (rango de voltaje aceptado)
#define VOLTAGE_RANGE_MASK    0x00FF8000   // ventana de voltaje 2.7-3.6V en el OCR (CMD58) - ver nota arriba

// Codigos de estado y error propios de la libreria.
// No forman parte del protocolo SD y no se confunden con las respuestas R1.
#define SUCCESSFUL_INIT          0x00
#define DETECTED                 0xDE   // Tarjeta detectada.
#define CARD_NOT_INSERTED        0xF0
#define SD_NOT_READY             0xF1
#define UNUSABLE_CARD             0xF2
#define ECHO_BACK_ERROR           0xF3
#define INCOMPATIBLE_VOLTAGE      0xF4
#define TOKEN_NOT_RECEIVED        0xF5
#define DATA_ACCEPTED             0xF6
#define DATA_REJECTED_CRC_ERROR   0xF7
#define DATA_REJECTED_WR_ERROR    0xF8
#define ERROR                     0xF9

// Banderas de estado de la tarjeta SD.
struct sdflags {
    unsigned detected : 1;
    unsigned init_ok  : 1;
    unsigned saving   : 1;
};

// Prototipos de la libreria.
unsigned char SD_Init(void);
unsigned char SD_Init_Try(unsigned char try_value);
unsigned char SD_Detect(void);
unsigned char SD_Read(unsigned char *Buffer, unsigned int nbytes);
unsigned char SD_Read_Block(unsigned char *Buffer, unsigned long Address);
unsigned char SD_Write_Block(unsigned char *Buffer, unsigned long Address);
unsigned char SD_Ready(void);
void SD_Send_Command(unsigned char command, unsigned long argument, unsigned char crc);
unsigned char R1_Response(void);
unsigned int  R2_Response(void);
unsigned long Response_32b(void);
void Release_SD(void);
void Select_SD(void);
void LEDcard(int veces, float tiempo_seg);

#endif
