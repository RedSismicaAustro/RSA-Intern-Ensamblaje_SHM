
// Funciones de inicializacion y transferencia para la interfaz SPI.

// Declara la interfaz publica de esta capa.
#include "spiSD.h"

// Inicializa el SPI a la velocidad seleccionada.
void SPISD_Init(unsigned char speed) {
    SPI1STAT.SPIEN = 0;              // Deshabilita el SPI1.

    // Configura la velocidad SPI para un reloj de 80 MHz (40 MIPS).
    if(speed == FAST) {
        // Velocidad SPI: 2.5 MHz.
        SPI1_Init_Advanced(_SPI_MASTER, _SPI_8_BIT, _SPI_PRESCALE_SEC_4, _SPI_PRESCALE_PRI_64, _SPI_SS_DISABLE, _SPI_DATA_SAMPLE_MIDDLE, _SPI_CLK_IDLE_HIGH, _SPI_ACTIVE_2_IDLE);
    } else {
        // Velocidad SPI: 156.25 kHz para la inicializacion de la SD.
        SPI1_Init_Advanced(_SPI_MASTER, _SPI_8_BIT, _SPI_PRESCALE_SEC_4, _SPI_PRESCALE_PRI_64, _SPI_SS_DISABLE, _SPI_DATA_SAMPLE_MIDDLE, _SPI_CLK_IDLE_HIGH, _SPI_ACTIVE_2_IDLE);
    }
    SPI1STAT.SPIEN = 1;    // Habilita el SPI1.
}
// Transfiere un byte por SPI y devuelve el byte recibido.
unsigned char SPISD_Write(unsigned char datos) {
    SPI1BUF = datos;                     // Carga el byte en el buffer de transmision.
    while(SPI1STATbits.SPITBF);          // Espera a que finalice la transmision.
    while(SPI1STATbits.SPIRBF == 0);     // Espera a recibir un byte.
    return SPI1BUF;
}
