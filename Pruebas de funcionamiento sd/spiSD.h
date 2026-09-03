#ifndef _SPISD_H_
#define _SPISD_H_

/*
 * spiSD.h - Prototipos y constantes para la capa SPI de la libreria SD
 *
 * La asignacion de SCK1, SDI1 y SDO1 a RB7, RB9 y RB8 se realiza mediante
 * PPS en mikroC, no en este archivo.
 */

#define SLOW 0
#define FAST 1

void SPISD_Init(unsigned char speed);
unsigned char SPISD_Write(unsigned char datos);

#endif
