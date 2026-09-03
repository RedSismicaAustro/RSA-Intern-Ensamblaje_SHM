/*-----------------------------------------------------------------------------------------------------------------------
Prueba de SD (modo SPI) - dsPIC33EP256MC202 - Nodo Sensor

ARCHIVOS QUE DEBE TENER ESTE PROYECTO EN mikroC (Project -> Add File):
  - nodo1.c      (este archivo, con su unico main())
  - spiSD.h      (prototipos + FAST/SLOW)
  - spiSD.c      (SPISD_Init/SPISD_Write)
  - sdcard.h     (struct sdflags + constantes CMD + prototipos)
  - sdcard.c     (SD_Init/SD_Read_Block/SD_Write_Block - VERSION CORREGIDA)

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

// Acceso directo al registro SFR RPINR20 (0x06C8) del dsPIC33EP256MC202.
#define RPINR20 (*((volatile unsigned int*)0x06C8))

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
void LED_patron(int veces);

void main() {
     ConfiguracionPrincipal();
     TEST = 0;

    // Diagnostico inicial: tres parpadeos de 50 ms.
     for (i = 0; i < 3; i++) {
         asm CLRWDT;
         TEST = 1; Delay_ms(50);
         TEST = 0; Delay_ms(50);
     }

     // Pausa para distinguir de los parpadeos siguientes
     Delay_ms(500);

     // Subindices:
     i = 0; j = 0; x = 0; y = 0;

     // Control sistema:
     inicioSistema = 0;

     // Tiempo:
     horaSistema = 0;
     fechaSistema = 0;

     // SD:
     PSEC = 0;
     sectorSD = 0;
     sectorLec = 0;
     checkEscSD = 0;
     checkLecSD = 0;
     MSRS485 = 0;
     banInsSec = 0;

     // Determina el ultimo sector fisico en funcion de la capacidad de la SD:
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

     // Datos de tiempo de prueba
     horaSistema = 86100;
     fechaSistema = 200228;

    // La deteccion se fuerza porque el conector no dispone de Card Detect.
     sdflags.detected = true;

    // Inicializa la tarjeta SD.
     if (sdflags.detected && !sdflags.init_ok) {
          unsigned char estado_init;
          estado_init = SD_Init_Try(10);
          if (estado_init == SUCCESSFUL_INIT) {
              sdflags.init_ok = true;
              inicioSistema = 1;
              // Indica una inicializacion correcta con el LED encendido.
              TEST = 1;
              for (i = 0; i < 20; i++) {
                  asm CLRWDT;
                  Delay_ms(100);
              }
              TEST = 0;
              Delay_ms(500);
           } else {
              unsigned int num_blinks, b;
              sdflags.init_ok = false;
              inicioSistema = 0;

              // Determina el numero exacto de parpadeos segun el codigo de error:
              // 1 parpadeo  = CARD_NOT_INSERTED (0xF0) -> CMD0 no responde 0x01
              // 2 parpadeos = SD_NOT_READY (0xF1) -> Timeout previo a comando
              // 3 parpadeos = UNUSABLE_CARD (0xF2) -> ACMD41/CMD1 no termina
              // 4 parpadeos = ECHO_BACK_ERROR (0xF3) -> CMD8 respuesta invalida
              // 5 parpadeos = INCOMPATIBLE_VOLTAGE (0xF4) -> CMD58 voltaje invalido
              if (estado_init == CARD_NOT_INSERTED) num_blinks = 1;
              else if (estado_init == SD_NOT_READY) num_blinks = 2;
              else if (estado_init == UNUSABLE_CARD) num_blinks = 3;
              else if (estado_init == ECHO_BACK_ERROR) num_blinks = 4;
              else if (estado_init == INCOMPATIBLE_VOLTAGE) num_blinks = 5;
              else num_blinks = 6;

              while (1) {
                  for (b = 0; b < num_blinks; b++) {
                      asm CLRWDT;
                      TEST = 1; Delay_ms(200);
                      TEST = 0; Delay_ms(200);
                  }
                  asm CLRWDT;
                  Delay_ms(1500); // Pausa de 1.5s entre rafagas de diagnostico
              }
           }
     }

     Ejemplo_uso_SD();

    // Indica que la prueba completa finalizo correctamente.
     while (1) {
           asm CLRWDT;
           TEST = 1; Delay_ms(200);
           TEST = 0; Delay_ms(200);
     }
}


//******************************************************************************
// Ejecuta una prueba de escritura, lectura y reescritura en la tarjeta SD.
//******************************************************************************
void Ejemplo_uso_SD(){
     unsigned char data_to_write[512];
     unsigned long sector;
     unsigned char buffer[512];

     // Llena los 512 bytes con un texto legible claramente en HxD:
     // " [NODO 1 - SECTOR 2500 - GUARDADO OK] " (repetido hasta completar 512 bytes)
     const char patron[] = " [NODO 1 - SECTOR 2500 - GUARDADO OK] ";
     unsigned int len_patron = sizeof(patron) - 1;
     for (i = 0; i < 512; i++) {
         data_to_write[i] = patron[i % len_patron];
     }

    // Escribe el patron en varios sectores de referencia.
     // 1. Sector 2500 (LBA 2500)
     checkEscSD = SD_Write_Block(data_to_write, 2500);

     // 2. Sector 100 (Sector bajo)
     SD_Write_Block(data_to_write, 100);

     // 3. Sector 1 (Primer sector despues del MBR)
     SD_Write_Block(data_to_write, 1);

     // 4. Sector 4 (Por si es tarjeta SDSC con direccionamiento por byte: 2500 / 512 = 4)
     SD_Write_Block(data_to_write, 4);

     if (checkEscSD == DATA_ACCEPTED){
         // 1 parpadeo largo = escritura OK
         LED_patron(1);
     } else {
         // Error: parpadeo ultra rapido permanente
         while(1){
            asm CLRWDT;
            TEST = 1; Delay_ms(50);
            TEST = 0; Delay_ms(50);
         }
     }

     // ---- LECTURA del sector 2500 ----
     sector = 2500;
     checkLecSD = SD_Read_Block(buffer, sector);
     if (checkLecSD == 0) {
        // 2 parpadeos largos = lectura OK
        LED_patron(2);
     } else {
        // Error de lectura: parpadeo ultra rapido permanente
        while(1){
            asm CLRWDT;
            TEST = 1; Delay_ms(50);
            TEST = 0; Delay_ms(50);
         }
     }

     // ---- REESCRITURA: escribe lo leido en el sector 2501 ----
     checkEscSD = SD_Write_Block(buffer, sector+1);
     if (checkEscSD == DATA_ACCEPTED){
        // 3 parpadeos largos = todo funciono!
        LED_patron(3);
     } else {
        // Error de reescritura
        while(1){
            asm CLRWDT;
            TEST = 1; Delay_ms(50);
            TEST = 0; Delay_ms(50);
         }
     }
}

//******************************************************************************
// Enciende el LED (RA2 = TEST) un numero de veces (1s ON, 500ms OFF)
//******************************************************************************
void LED_patron(int veces){
     unsigned int k;
     for (k = 0; k < veces; k++){
         asm CLRWDT;
         TEST = 1; Delay_ms(1000);
         asm CLRWDT;
         TEST = 0; Delay_ms(500);
     }
     asm CLRWDT;
     Delay_ms(1000);
}


//******************************************************************************
// CONFIGURACION PRINCIPAL
//******************************************************************************
void ConfiguracionPrincipal(){

     // Configuracion del reloj (FRC+PLL, sin cristal): ~80MHz
     CLKDIVbits.FRCDIV = 0;
     CLKDIVbits.PLLPOST = 0;
     CLKDIVbits.PLLPRE = 5;
     PLLFBDbits.PLLDIV = 150;

    // Deshabilita las funciones analogicas.
     ANSELA = 0;
     ANSELB = 0;

     // *** CONFIGURACION PPS (Peripheral Pin Select) ***
     ConfigurarPPS_SPI1();

     // Delay de estabilizacion reducido (500ms en vez de 4s)
     // con CLRWDT para evitar reset por Watchdog
     for (i = 0; i < 5; i++) {
         asm CLRWDT;
         Delay_ms(100);
     }

     // Configuracion de puertos:
     TEST_Direction = 0;       // RA2 salida (LED)
     CsADXL_Direction = 0;    // RA3 salida (CS Acelerometro)
     sd_CS_tris = 0;          // RB0 salida (CS SD)
     sd_detect_tris = 1;      // RA4 entrada (Card Detect)
     MSRS485_Direction = 0;   // RB12 salida (RS485)

     // Valores iniciales:
     TEST = 0;
     CsADXL = 1;              // CS ADXL inactivo
     sd_CS_lat = 1;           // CS SD inactivo (alto)
     MSRS485 = 0;             // RS485 en modo lectura

     // Pull-up interna en RA4 (Card Detect ausente en socket de 8 pines)
     CNPUA = CNPUA | (1 << 4);   // Pull-up en RA4
     // Pull-up interna en RB9 (MISO / SDI1) para evitar lecturas falsas si la SD no esta insertada
     CNPUB = CNPUB | (1 << 9);   // Pull-up en RB9

     // Limpia las banderas de la SD:
     sdflags.detected = false;
     sdflags.init_ok = false;
     sdflags.saving = false;

     Delay_ms(200);
}

//******************************************************************************
// CONFIGURACION PPS (Peripheral Pin Select) para SPI1 en dsPIC33EP256MC202
// Utiliza escritura por puntero al registro SFR RPINR20 (0x06C8).
//
// Conexiones segun esquema del nodo:
//   RB7 (RP39) -> SCK1  (reloj SPI, salida al pin CLK de la SD)
//   RB8 (RP40) -> SDO1  (datos del PIC a la SD, salida al pin DI/MOSI)
//   RB9 (RP41) -> SDI1  (datos de la SD al PIC, entrada desde pin DO/MISO)
//******************************************************************************
void ConfigurarPPS_SPI1(){

     // === Unlock IOLOCK (Desbloquea los registros PPS del dsPIC33EP) ===
     asm {
         MOV #0x46, W0
         MOV #0x57, W1
         MOV #0x0742, W2
         MOV.b W0, [W2]
         MOV.b W1, [W2]
         BCLR OSCCON, #6
     }

     // === Mapeo de SALIDAS SPI1 ===

     // RB7 (RP39) -> SCK1OUT (funcion de salida #6: Clock SPI)
     // RPOR2 bits[13:8] controlan RP39 (RB7)
     RPOR2 = (RPOR2 & 0x00FF) | 0x0600;

     // RB8 (RP40) -> SDO1 (funcion de salida #5: Data Out MOSI)
     // RPOR3 bits[5:0] controlan RP40 (RB8)
     RPOR3 = (RPOR3 & 0xFF00) | 0x0005;

     // === Mapeo de ENTRADA SPI1 ===

     // SDI1 <- RB9 (RP41) (Data In MISO)
     // RPINR20 bits[7:0] = SDI1R
     RPINR20 = (RPINR20 & 0xFF00) | 41;

     // === Direcciones TRIS de los pines SPI ===
     TRISBbits.TRISB7 = 0;    // RB7 (SCK1) salida
     TRISBbits.TRISB8 = 0;    // RB8 (SDO1) salida
     TRISBbits.TRISB9 = 1;    // RB9 (SDI1) entrada
}
