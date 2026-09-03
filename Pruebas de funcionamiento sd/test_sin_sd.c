/*
 * test_sin_sd.c - Diagnostico del sistema sin inicializar la SD.
 *
 * Utiliza los mismos pines y reloj que nodo1.c, pero no llama a las funciones
 * de la libreria SD. Para esta prueba, retirar sdcard.c y spiSD.c del proyecto
 * de mikroC.
 *
 * Si el LED parpadea, revisar la ruta de inicializacion de la SD. Si no
 * parpadea, revisar la configuracion basica de pines y del sistema.
 */
#include <stdbool.h>

#define IDNODO 1

unsigned int i, j, x, y;

sbit TEST at LATA2_bit;
sbit TEST_Direction at TRISA2_bit;
sbit CsADXL at LATA3_bit;
sbit CsADXL_Direction at TRISA3_bit;
sbit sd_CS_lat at LATB0_bit;
sbit sd_CS_tris at TRISB0_bit;
sbit sd_detect_port at LATA4_bit;
sbit sd_detect_tris at TRISA4_bit;
sbit MSRS485 at LATB12_bit;
sbit MSRS485_Direction at TRISB12_bit;

void ConfiguracionPrincipal();

void main() {
     ConfiguracionPrincipal();
     TEST = 0;

    // Indica que la configuracion inicial se ejecuto correctamente.
     for (i = 0; i < 3; i++) {
         TEST = 1; Delay_ms(150);
         TEST = 0; Delay_ms(150);
     }

     MSRS485 = 0;

    // Indica que el programa continua ejecutandose.
     while (1) {
         asm CLRWDT;
         TEST = 1; Delay_ms(500);
         TEST = 0; Delay_ms(500);
     }
}

void ConfiguracionPrincipal(){

     CLKDIVbits.FRCDIV = 0;
     CLKDIVbits.PLLPOST = 0;
     CLKDIVbits.PLLPRE = 5;
     PLLFBDbits.PLLDIV = 150;

     ANSELA = 0;
     ANSELB = 0;

     for (i = 0; i < 40; i++) {
         asm CLRWDT;
         Delay_ms(100);
     }

     sd_CS_tris = 0;
     sd_detect_tris = 1;

     for (i = 0; i < 2; i++) {
         asm CLRWDT;
         Delay_ms(100);
     }
}
