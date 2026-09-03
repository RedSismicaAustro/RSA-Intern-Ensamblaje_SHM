/*-----------------------------------------------------------------------------------------------------------------------
Prueba de SD (modo SPI) - dsPIC33EP256MC202 - Nodo Sensor  (VERSION 2 - CORREGIDA)

Cambios respecto a nodo1.c original:
  1. Eliminado delay de 4 segundos al arranque (causa posible de reset por WDT)
  2. Pull-up interna en RA4 (Card Detect) ya que el socket de 8 pines no tiene pin CD
  3. Diagnostico mejorado con patrones de LED diferentes para cada etapa/error
  4. Reducido tiempo de espera en deteccion de SD

ARCHIVOS QUE DEBE TENER ESTE PROYECTO EN mikroC (Project -> Add File):
  - nodo1_v2.c   (este archivo, con su unico main())
  - spiSD.h      (prototipos + FAST/SLOW)
  - spiSD.c      (SPISD_Init/SPISD_Write)
  - sdcard.h     (struct sdflags + constantes CMD + prototipos)
  - sdcard.c     (SD_Init/SD_Read_Block/SD_Write_Block - VERSION CORREGIDA)

CONFIGURACION OBLIGATORIA EN EL IDE (Project -> Edit Project):
  1. Buscar la pestana de PPS / Routing / Peripheral Pin Select
  2. Asignar:
     SCK1 (salida) -> RB7    (pin fisico del clock SPI hacia la SD)
     SDI1 (entrada) -> RB9   (pin fisico de MISO, datos de la SD al PIC)
     SDO1 (salida) -> RB8    (pin fisico de MOSI, datos del PIC a la SD)
  3. Sin esta configuracion, el SPI NO VA A FUNCIONAR

PATRONES DE LED PARA DIAGNOSTICO (usa RA2 = TEST):
  - 3 parpadeos rapidos al arranque    = Programa corriendo OK
  - LED fijo encendido                 = SD inicializada con exito
  - Parpadeo lento 500/500ms           = SD fallo al inicializar
  - 1 parpadeo largo + pausa           = Escritura OK
  - 2 parpadeos largos + pausa         = Lectura OK
  - 3 parpadeos largos + pausa         = Reescritura OK (todo funciono!)
  - Parpadeo ultra rapido 50/50ms      = Error de lectura o escritura
-------------------------------------------------------------------------------------------------------------------------*/
#include "spiSD.h"
#include "sdcard.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Parametros del nodo y de la tarjeta SD.
#define IDNODO 1
#define SIZESD 16
#define DELTASECTOR 97952

// Declaracion de variables y constantes.
#define FP 80000000

// Indices de trabajo.
unsigned int i, j, x, y;

// Pines del sistema.
struct sdflags sdflags;
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

// Variables de control del sistema.
unsigned short inicioSistema;
unsigned long horaSistema, fechaSistema;

// Variables de manejo de la tarjeta SD.
unsigned long PSF;
unsigned long PSE;
unsigned long USF;
unsigned long PSEC;
unsigned long sectorSD;
unsigned long sectorLec;
const unsigned int clusterSizeSD = 512;
unsigned long infoPrimerSector;
unsigned long infoUltimoSector;
unsigned char cabeceraSD[6] = {255, 253, 251, 10, 0, 250};
unsigned char bufferSD [clusterSizeSD];
unsigned char checkEscSD;
unsigned char checkLecSD;
unsigned short banInsSec;

// Declaracion de funciones.
void ConfiguracionPrincipal();
void ConfigurarPPS_SPI1();
void Ejemplo_uso_SD();
void LED_patron(int veces, unsigned int t_on, unsigned int t_off);

void main() {
     ConfiguracionPrincipal();
     TEST = 0;

    // Diagnostico inicial: tres parpadeos de 150 ms.
     for (i = 0; i < 3; i++) {
         asm CLRWDT;
         TEST = 1; Delay_ms(150);
         TEST = 0; Delay_ms(150);
     }

    // Separa el diagnostico de los patrones siguientes.
     Delay_ms(500);

    // Inicializa los indices de trabajo.
     i = 0; j = 0; x = 0; y = 0;

    // Inicializa el estado del sistema.
     inicioSistema = 0;

    // Inicializa los datos de tiempo.
     horaSistema = 0;
     fechaSistema = 0;

    // Inicializa las variables de la tarjeta SD.
     PSEC = 0;
     sectorSD = 0;
     sectorLec = 0;
     checkEscSD = 0;
     checkLecSD = 0;
     MSRS485 = 0;
     banInsSec = 0;

    // Determina el ultimo sector fisico segun la capacidad de la tarjeta.
     switch (SIZESD){
            case 2:
                    PSF = 2048;
                    USF = 3911679;
                    break;
            case 4:
                    PSF = 2048;
                    USF = 7772160;
                    break;
            case 8:
                    PSF = 2048;
                    USF = 16779263;
                    break;
            case 16:
                    PSF = 2048;
                    USF = 31115263;
                    break;
     }
     infoPrimerSector = PSF+DELTASECTOR-2;
     infoUltimoSector = PSF+DELTASECTOR-1;
     PSE = PSF+DELTASECTOR;

    // Datos temporales de prueba.
     horaSistema = 86100;
     fechaSistema = 200228;

    // La deteccion se fuerza porque el conector no dispone de Card Detect.
     sdflags.detected = true;

    // Inicializa la tarjeta SD.
     if (sdflags.detected && !sdflags.init_ok) {
          if (SD_Init_Try(10) == SUCCESSFUL_INIT) {
              sdflags.init_ok = true;
              inicioSistema = 1;
              TEST = 1;                                    // Inicializacion correcta.
           } else {
              sdflags.init_ok = false;
              inicioSistema = 0;
              // Error de inicializacion: parpadeo lento permanente.
              while (1) {
                  asm CLRWDT;
                  TEST = 1; Delay_ms(500);
                  TEST = 0; Delay_ms(500);
              }
           }
     }

    // La tarjeta esta inicializada; ejecuta la prueba de escritura y lectura.
    Delay_ms(1000);  // Mantiene visible el indicador de inicializacion.
     TEST = 0;
     Delay_ms(500);

     Ejemplo_uso_SD();

    // Indica que la prueba completa finalizo correctamente.
     while (1) {
           asm CLRWDT;
           TEST = 1; Delay_ms(200);
           TEST = 0; Delay_ms(200);
     }
}


// Ejecuta una prueba de escritura, lectura y reescritura en la tarjeta SD.
void Ejemplo_uso_SD(){
     unsigned char data_to_write[512];
     unsigned long sector;
     unsigned char buffer[512];

    // Llena el bloque con un patron legible en un visor hexadecimal:
     // " [NODO 1 - SECTOR 2500 - GUARDADO OK] " (repetido hasta completar 512 bytes)
     const char patron[] = " [NODO 1 - SECTOR 2500 - GUARDADO OK] ";
     unsigned int len_patron = sizeof(patron) - 1;
     for (i = 0; i < 512; i++) {
         data_to_write[i] = patron[i % len_patron];
     }

    // Escribe el patron en varios sectores de referencia.
     // 1. Sector 2500 (LBA 2500)
     checkEscSD = SD_Write_Block(data_to_write, 2500);

    // Sector bajo de referencia.
     SD_Write_Block(data_to_write, 100);

    // Primer sector despues del MBR.
     SD_Write_Block(data_to_write, 1);

    // Sector equivalente para tarjetas SDSC con direccionamiento por bytes.
     SD_Write_Block(data_to_write, 4);

     if (checkEscSD == DATA_ACCEPTED){
         // Un parpadeo largo indica una escritura correcta.
         LED_patron(1, 1000, 500);
     } else {
         // Error de escritura: parpadeo rapido permanente.
         while(1){
            asm CLRWDT;
            TEST = 1; Delay_ms(50);
            TEST = 0; Delay_ms(50);
         }
     }

    // Lee el sector anteriormente escrito.
     checkLecSD = SD_Read_Block(buffer, sector);
     if (checkLecSD == 0) {
        // Dos parpadeos largos indican una lectura correcta.
        LED_patron(2, 1000, 500);
     } else {
        // Error de lectura: parpadeo rapido permanente.
        while(1){
            asm CLRWDT;
            TEST = 1; Delay_ms(50);
            TEST = 0; Delay_ms(50);
        }
     }

    // Reescribe los datos en el sector siguiente.
     checkEscSD = SD_Write_Block(buffer, sector+1);
     if (checkEscSD == DATA_ACCEPTED){
        // Tres parpadeos largos indican que la prueba finalizo correctamente.
        LED_patron(3, 1000, 500);
     } else {
        // Error de reescritura.
        while(1){
            asm CLRWDT;
            TEST = 1; Delay_ms(50);
            TEST = 0; Delay_ms(50);
        }
     }
}

// Enciende el LED (RA2 = TEST) un numero de veces con tiempos configurables.
void LED_patron(int veces, unsigned int t_on, unsigned int t_off){
     unsigned int k;
     for (k = 0; k < veces; k++){
         asm CLRWDT;
         TEST = 1; Delay_ms(t_on);
         asm CLRWDT;
         TEST = 0; Delay_ms(t_off);
     }
    // Pausa adicional para distinguir los patrones.
     asm CLRWDT;
     Delay_ms(1000);
}


// Configura el reloj, los puertos y las banderas de la tarjeta SD.
void ConfiguracionPrincipal(){

    // Configura el reloj con FRC+PLL, sin cristal externo.
     CLKDIVbits.FRCDIV = 0;
     CLKDIVbits.PLLPOST = 0;
     CLKDIVbits.PLLPRE = 5;
     PLLFBDbits.PLLDIV = 150;

    // Deshabilita las funciones analogicas.
     ANSELA = 0;
     ANSELB = 0;

    // Configura PPS antes de inicializar el SPI y de activar IOLOCK.
     ConfigurarPPS_SPI1();

    // Espera la estabilizacion del sistema y atiende el Watchdog.
     for (i = 0; i < 5; i++) {
         asm CLRWDT;
         Delay_ms(100);
     }

    // Configura la direccion de los puertos.
     TEST_Direction = 0;       // RA2 salida (LED)
     CsADXL_Direction = 0;    // RA3 salida (CS Acelerometro)
     sd_CS_tris = 0;          // RB0 salida (CS SD)
     sd_detect_tris = 1;      // RA4 entrada (Card Detect)
     MSRS485_Direction = 0;   // RB12 salida (RS485)

    // Establece los estados iniciales de los pines.
     TEST = 0;
     CsADXL = 1;              // CS ADXL inactivo
     sd_CS_lat = 1;           // CS SD inactivo (alto)
     MSRS485 = 0;             // RS485 en modo lectura

    // Habilita la pull-up de RA4; el conector no dispone de Card Detect.
     CNPUA = CNPUA | (1 << 4);   // Pull-up en RA4
    // Habilita la pull-up de RB9 para evitar lecturas falsas sin tarjeta.
     CNPUB = CNPUB | (1 << 9);   // Pull-up en RB9

    // Reinicia las banderas de estado de la tarjeta.
     sdflags.detected = false;
     sdflags.init_ok = false;
     sdflags.saving = false;

     Delay_ms(200);
}

// Configura PPS para SPI1 y mapea sus pines fisicos.
// Sin esta configuracion, la tarjeta SD no responde.
//
// Conexiones segun esquema del nodo:
//   RB7 (RP39) -> SCK1  (reloj SPI, salida al pin CLK de la SD)
//   RB8 (RP40) -> SDO1  (datos del PIC a la SD, salida al pin DI/MOSI)
//   RB9 (RP41) -> SDI1  (datos de la SD al PIC, entrada desde pin DO/MISO)
//
// Despues del Power-On Reset, el bit IOLOCK de OSCCON esta en 0
// (desbloqueado), por lo que podemos escribir los registros RPOR/RPINR
// directamente sin necesidad de la secuencia de desbloqueo.
// Esta funcion debe ejecutarse antes de activar IOLOCK.
void ConfigurarPPS_SPI1(){

    // Desbloquea los registros PPS si IOLOCK esta activado.
     asm {
         MOV #0x46, W0
         MOV #0x57, W1
         MOV #0x0742, W2
         MOV.b W0, [W2]
         MOV.b W1, [W2]
         BCLR OSCCON, #6
     }

    // Mapea las salidas de SPI1.

     // RB7 (RP39) -> SCK1OUT (funcion de salida #6: Clock SPI)
     // RPOR3: bits[13:8] = RP39R (controla la salida de RB7)
     RPOR3 = (RPOR3 & 0x00FF) | (0x0006 << 8);

     // RB8 (RP40) -> SDO1 (funcion de salida #5: Data Out MOSI)
     // RPOR4: bits[5:0]  = RP40R (controla la salida de RB8)
     RPOR4 = (RPOR4 & 0xFF00) | 0x0005;

    // Mapea la entrada de SPI1.

     // SDI1 <- RB9 (RP41) (Data In MISO)
     // RPINR20: bits[7:0] = SDI1R (numero de RP = 41 para RB9)
     RPINR20 = (RPINR20 & 0xFF00) | 41;

    // Configura la direccion de los pines SPI.
     TRISBbits.TRISB7 = 0;   // RB7 (SCK1) como salida
     TRISBbits.TRISB8 = 0;   // RB8 (SDO1) como salida
     TRISBbits.TRISB9 = 1;   // RB9 (SDI1) como entrada
}
