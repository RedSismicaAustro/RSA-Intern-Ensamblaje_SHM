/*
 * test_blink.c - Prueba minima de funcionamiento del LED.
 *
 * Confirma que el microcontrolador esta programado, ejecutando el programa
 * y que el LED D3 (TEST1/RA2) funciona. No utiliza la SD ni la interfaz RS485.
 * Si el LED no parpadea, revisar la alimentacion, MCLR, el LED y la
 * programacion del dispositivo.
 */

sbit TEST           at LATA2_bit;
sbit TEST_Direction at TRISA2_bit;

void main(void) {

    // Configura el reloj con FRC+PLL, sin cristal externo.
    CLKDIVbits.FRCDIV = 0;
    CLKDIVbits.PLLPOST = 0;
    CLKDIVbits.PLLPRE = 5;
    PLLFBDbits.PLLDIV = 150;

    ANSELA = 0;
    ANSELB = 0;

    TEST_Direction = 0;   // RA2 como salida
    TEST = 0;

    while (1) {
        TEST = 1;
        Delay_ms(300);
        TEST = 0;
        Delay_ms(300);
    }
}
