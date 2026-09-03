/*
 * test_sd_sector2500.c - Prueba de escritura de un unico sector SD.
 *
 * Compilar como archivo principal junto con sdcard.c, sdcard.h, spiSD.c
 * y spiSD.h. No consulta el pin de deteccion RA4 y no lee ningun sector.
 * La unica escritura de datos se realiza en el sector 2500.
 */
#include <spiSD.h>
#include <sdcard.h>

#define SECTOR_PRUEBA 2500UL
#define TAMANO_SECTOR 512
#define INTENTOS_INICIALIZACION 3
#define INTENTOS_ESCRITURA 5

// RP41 = RB9: entrada SDI1 de SPI1.
#define RPINR20 (*((volatile unsigned int*)0x06C8))

sbit TEST at LATA2_bit;
sbit TEST_Direction at TRISA2_bit;
sbit sd_CS_lat at LATB0_bit;
sbit sd_CS_tris at TRISB0_bit;
sbit sd_detect_port at LATA4_bit;
sbit sd_detect_tris at TRISA4_bit;

struct sdflags sdflags;
unsigned char bufferSD[TAMANO_SECTOR];
unsigned int indice;

void ConfigurarPinesYSPI();
void PrepararBuffer();
void ParpadearUnSegundo(unsigned char veces);
void Parpadear250ms(unsigned char veces);
void ErrorInicializacion();
void ErrorEscritura();

void main() {
    unsigned char resultado;
    unsigned char intento;

    ConfigurarPinesYSPI();
    PrepararBuffer();

    // La placa no tiene un contacto de deteccion utilizable: se fuerza la prueba.
    sdflags.detected = true;
    sdflags.init_ok = false;
    sdflags.saving = false;

    // SD_Init usa 625 kHz y cambia a 2.5 MHz despues de inicializar correctamente.
    resultado = SD_Init_Try(INTENTOS_INICIALIZACION);
    if (resultado != SUCCESSFUL_INIT) {
        ErrorInicializacion();
    }
    sdflags.init_ok = true;

    // Solo se escribe el sector 2500; no se escriben sectores adyacentes.
    for (intento = 0; intento < INTENTOS_ESCRITURA; intento++) {
        resultado = SD_Write_Block(bufferSD, SECTOR_PRUEBA);
        if (resultado == DATA_ACCEPTED) {
            sdflags.saving = true;
            ParpadearUnSegundo(1);
            while (1) {
                asm CLRWDT;
                TEST = 1;
                Delay_ms(1000);
                TEST = 0;
                Delay_ms(3000);
            }
        }
        Delay_ms(10);
    }

    ErrorEscritura();
}

void ConfigurarPinesYSPI() {
    // Estabiliza la placa y conserva el reloj de proyecto de 80 MHz.
    Delay_ms(4000);

    ANSELA = 0;
    ANSELB = 0;

    // SCK1 en RB7, SDO1/MOSI en RB8 y SDI1/MISO en RB9.
    asm {
        MOV #0x46, W0
        MOV #0x57, W1
        MOV #0x0742, W2
        MOV.b W0, [W2]
        MOV.b W1, [W2]
        BCLR OSCCON, #6
    }

    RPOR2 = (RPOR2 & 0x00FF) | 0x0600;
    RPOR3 = (RPOR3 & 0xFF00) | 0x0005;
    RPINR20 = (RPINR20 & 0xFF00) | 41;

    TRISBbits.TRISB7 = 0;
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB9 = 1;

    TEST_Direction = 0;
    TEST = 0;
    sd_CS_tris = 0;
    sd_CS_lat = 1;
    sd_detect_tris = 1;

    Delay_ms(200);
}

void PrepararBuffer() {
    // Patron facil de reconocer al inspeccionar el sector 2500 en hexadecimal.
    for (indice = 0; indice < TAMANO_SECTOR; indice++) {
        bufferSD[indice] = (unsigned char)(indice & 0xFF);
    }
}

void ParpadearUnSegundo(unsigned char veces) {
    unsigned char repeticion;
    for (repeticion = 0; repeticion < veces; repeticion++) {
        TEST = 1;
        Delay_ms(1000);
        TEST = 0;
        Delay_ms(1000);
    }
}

void Parpadear250ms(unsigned char veces) {
    unsigned char repeticion;
    for (repeticion = 0; repeticion < veces; repeticion++) {
        TEST = 1;
        Delay_ms(250);
        TEST = 0;
        Delay_ms(250);
    }
}

void ErrorInicializacion() {
    // Error fijo: evita confundir el fallo de SPI con una falsa deteccion de SD.
    while (1) {
        TEST = 1;
        Delay_ms(250);
        TEST = 0;
        Delay_ms(250);
    }
}

void ErrorEscritura() {
    // Dos parpadeos repetidos indican que fallo la escritura del sector 2500.
    while (1) {
        Parpadear250ms(2);
        Delay_ms(2000);
    }
}
