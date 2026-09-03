/*
 * test_pines.c - Diagnostico de la configuracion de pines.
 *
 * Configura los mismos pines que nodo1.c y test_sin_sd.c, sin utilizar las
 * librerias de la SD. La resistencia pull-up de RA4 evita que la entrada de
 * deteccion quede flotante cuando el conector no dispone de Card Detect.
 *
 * Resultado esperado: cinco parpadeos de 100 ms al arrancar y, a
 * continuacion, parpadeo continuo de un segundo. Si no ocurre, revisar la
 * alimentacion, MCLR, la programacion del dispositivo y el modelo seleccionado
 * en mikroC (dsPIC33EP256MC202).
 */

// Pines compartidos con nodo1.c.
sbit TEST           at LATA2_bit;
sbit TEST_Direction at TRISA2_bit;
sbit CsADXL           at LATA3_bit;
sbit CsADXL_Direction at TRISA3_bit;
sbit sd_CS_lat      at LATB0_bit;
sbit sd_CS_tris     at TRISB0_bit;
sbit sd_detect_port at LATA4_bit;
sbit sd_detect_tris at TRISA4_bit;
sbit MSRS485           at LATB12_bit;
sbit MSRS485_Direction at TRISB12_bit;

void main(void) {

    // Configura el reloj con FRC+PLL.
    CLKDIVbits.FRCDIV = 0;
    CLKDIVbits.PLLPOST = 0;
    CLKDIVbits.PLLPRE  = 5;
    PLLFBDbits.PLLDIV  = 150;

    // Deshabilita las funciones analogicas.
    ANSELA = 0;
    ANSELB = 0;

    // Configura la direccion de los pines.
    TEST_Direction = 0;       // RA2 salida (LED)
    CsADXL_Direction = 0;    // RA3 salida (CS Acelerometro)
    sd_CS_tris = 0;          // RB0 salida (CS SD)
    sd_detect_tris = 1;      // RA4 entrada (Card Detect)
    MSRS485_Direction = 0;   // RB12 salida (RS485 direccion)

    // Establece los estados iniciales de los pines.
    TEST   = 0;
    CsADXL = 1;              // CS ADXL inactivo (alto)
    sd_CS_lat = 1;           // CS SD inactivo (alto)
    MSRS485 = 0;             // RS485 en modo lectura

    // Habilita la pull-up de RA4, ya que el conector no tiene Card Detect.
    CNPUA = CNPUA | (1 << 4);   // Pull-up de RA4.

    // Espera breve para estabilizar el sistema.
    Delay_ms(50);

    // Indica que la configuracion de pines se ejecuto correctamente.
    {
        unsigned int k;
        for (k = 0; k < 5; k++) {
            asm CLRWDT;
            TEST = 1; Delay_ms(100);
            TEST = 0; Delay_ms(100);
        }
    }

    // Separa el diagnostico del estado normal.
    Delay_ms(500);

    // Indica que el programa continua ejecutandose.
    while (1) {
        asm CLRWDT;
        TEST = 1; Delay_ms(1000);
        asm CLRWDT;
        TEST = 0; Delay_ms(1000);
    }
}
