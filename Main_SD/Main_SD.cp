#line 1 "C:/Users/User/Downloads/Main_SD/Main_SD/Main_SD.c"
#line 1 "c:/users/user/downloads/main_sd/main_sd/spisd.h"
#line 12 "c:/users/user/downloads/main_sd/main_sd/spisd.h"
void SPISD_Init(unsigned char speed);
unsigned char SPISD_Write(unsigned char datos);
#line 1 "c:/users/user/downloads/main_sd/main_sd/sdcard.h"
#line 1 "c:/users/public/documents/mikroelektronika/mikroc pro for dspic/include/stdbool.h"



 typedef char _Bool;
#line 13 "c:/users/user/downloads/main_sd/main_sd/sdcard.h"
struct sdflags {
 unsigned char init_ok:1;
 unsigned char detected:1;
 unsigned char saving:1;
};
#line 93 "c:/users/user/downloads/main_sd/main_sd/sdcard.h"
unsigned char SD_Init(void);
unsigned char SD_Init_Try(unsigned char);
unsigned char SD_Write_Block(unsigned char*,unsigned long);
unsigned char SD_Read_Block(unsigned char*,unsigned long);
unsigned char SD_Read(unsigned char*,unsigned int);
void SD_Send_Command(unsigned char, unsigned long, unsigned char);
unsigned char R1_Response(void);
unsigned int R2_Response(void);
unsigned long Response_32b(void);
unsigned char SD_Ready(void);
void Select_SD(void);
void Release_SD(void);
 _Bool  Detect_SD (void);
unsigned char SD_Detect(void);
void SD_Check(void);
#line 1 "c:/users/public/documents/mikroelektronika/mikroc pro for dspic/include/stdbool.h"
#line 1 "c:/users/public/documents/mikroelektronika/mikroc pro for dspic/include/stdlib.h"







 typedef struct divstruct {
 int quot;
 int rem;
 } div_t;

 typedef struct ldivstruct {
 long quot;
 long rem;
 } ldiv_t;

 typedef struct uldivstruct {
 unsigned long quot;
 unsigned long rem;
 } uldiv_t;

int abs(int a);
float atof(char * s);
int atoi(char * s);
long atol(char * s);
div_t div(int number, int denom);
ldiv_t ldiv(long number, long denom);
uldiv_t uldiv(unsigned long number, unsigned long denom);
long labs(long x);
int max(int a, int b);
int min(int a, int b);
void srand(unsigned x);
int rand();
int xtoi(char * s);
#line 20 "C:/Users/User/Downloads/Main_SD/Main_SD/Main_SD.c"
unsigned int i, j, x, y;


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


unsigned short inicioSistema;
unsigned long horaSistema, fechaSistema;


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



void ConfiguracionPrincipal();
void ConfigurarPPS_SPI1();
void GuardarTramaSD();
void GuardarBufferSD(unsigned char* bufferLleno, unsigned long sector);
void GuardarInfoSector(unsigned long datoSector, unsigned long localizacionSector);

void Ejemplo_uso_SD();
void LED(int veces, unsigned char tiempo_seg);
void LED_Error(unsigned char codigo);

void main() {
 ConfiguracionPrincipal();
 TEST = 0;
 TEST = 1;
 Delay_ms(1000);
 TEST = 0;

 i = 0;
 j = 0;
 x = 0;
 y = 0;


 inicioSistema = 0;


 horaSistema = 0;
 fechaSistema = 0;


 PSEC = 0;
 sectorSD = 0;
 sectorLec = 0;
 checkEscSD = 0;
 checkLecSD = 0;
 MSRS485 = 0;
 banInsSec = 0;


 switch ( 16 ){
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
 infoPrimerSector = PSF+ 97952 -2;
 infoUltimoSector = PSF+ 97952 -1;
 PSE = PSF+ 97952 ;


 horaSistema = 86100;
 fechaSistema = 200228;
#line 138 "C:/Users/User/Downloads/Main_SD/Main_SD/Main_SD.c"
 sdflags.detected =  1 ;



 if (sdflags.detected && !sdflags.init_ok) {
 checkEscSD = SD_Init_Try(10);
 if (checkEscSD ==  0xAA ) {
 sdflags.init_ok =  1 ;
 inicioSistema = 1;
 TEST = 1;
 } else {
 sdflags.init_ok =  0 ;
 INT1IE_bit = 0;
 U1MODE.UARTEN = 0;
 inicioSistema = 0;
 LED_Error(checkEscSD);
 }
 }


 Ejemplo_uso_SD();


}





void Ejemplo_uso_SD(){
#line 172 "C:/Users/User/Downloads/Main_SD/Main_SD/Main_SD.c"
 unsigned char data_to_write[512];
 unsigned long sector;
 unsigned char valor;
 unsigned char buffer[512];


 valor = 1;
 for (i = 0; i < 512; i++) {
 data_to_write[i] = valor;
 valor++;

 if (valor == 256) {
 valor = 1;
 }
 }


 sector = 2500;
 checkEscSD = SD_Write_Block(data_to_write,sector);
 if (checkEscSD ==  22 ){
 LED(1,1);
 } else {
 LED_Error(10);
 }


 checkLecSD = SD_Read_Block(buffer, sector);
 if (checkLecSD==0) {
 LED(1,1);
 } else {
 LED_Error(11);
 }




 checkEscSD = SD_Write_Block(buffer,sector+1);
 if (checkEscSD ==  22 ){
 LED(1,1);
 } else {
 LED_Error(12);
 }

}


void LED(int veces, unsigned char tiempo_seg){
#line 222 "C:/Users/User/Downloads/Main_SD/Main_SD/Main_SD.c"
 unsigned int i;
 TEST_Direction = 0;
 if (tiempo_seg==1){
 for (i=0; i<veces; i++){
 TEST = 1;
 Delay_ms(1000);
 TEST = 0;
 Delay_ms(1000);
 }
 }else if (tiempo_seg==2){
 for (i=0; i<veces; i++){
 TEST = 1;
 Delay_ms(2000);
 TEST = 0;
 Delay_ms(2000);
 }
 }
}


void LED_Error(unsigned char codigo){
 unsigned char parpadeos;
 parpadeos = codigo;
 if (parpadeos > 9) parpadeos = parpadeos - 9;
 while (1){
 for (i = 0; i < parpadeos; i++){
 TEST = 1;
 Delay_ms(500);
 TEST = 0;
 Delay_ms(500);
 }
 Delay_ms(3000);
 }
}






void ConfiguracionPrincipal(){
 Delay_ms(4000);


 ANSELA = 0;
 ANSELB = 0;
 ConfigurarPPS_SPI1();
 TEST_Direction = 0;
 TEST = 0;
 sd_CS_tris = 0;
 sd_detect_tris = 1;


 sdflags.detected =  0 ;
 sdflags.init_ok =  0 ;
 sdflags.saving =  0 ;

 Delay_ms(200);
}


void ConfigurarPPS_SPI1(){
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
  (*((volatile unsigned int*)0x06C8))  = ( (*((volatile unsigned int*)0x06C8))  & 0xFF00) | 41;

 TRISBbits.TRISB7 = 0;
 TRISBbits.TRISB8 = 0;
 TRISBbits.TRISB9 = 1;
}




void GuardarBufferSD(unsigned char* bufferLleno, unsigned long sector){

 for (x=0;x<5;x++){
 checkEscSD = SD_Write_Block(bufferLleno,sector);
 if (checkEscSD ==  22 ){
 break;
 }
 Delay_us(10);
 }
}




void GuardarTramaSD(){


 unsigned char bufferSD[512];

 sectorSD=40000;
 for (sectorSD=40000; sectorSD<40020;sectorSD++){

 for (x=0;x<6;x++){
 bufferSD[x] = 1;
 }

 for (x=0;x<6;x++){
 bufferSD[6+x] = 2;
 }

 for (x=0;x<500;x++){
 bufferSD[12+x] = 3;
 }


 GuardarBufferSD(bufferSD, sectorSD);

 sectorSD++;



 for (x=0;x<512;x++){
 bufferSD[x] = 4;
 }
 GuardarBufferSD(bufferSD, sectorSD);
 sectorSD++;


 for (x=0;x<512;x++){
 bufferSD[x] = 5;
 }
 GuardarBufferSD(bufferSD, sectorSD);
 sectorSD++;


 for (x=0;x<512;x++){
 bufferSD[x] = 6;
 }
 GuardarBufferSD(bufferSD, sectorSD);
 sectorSD++;


 for (x=0;x<512;x++){
 if (x<464){
 bufferSD[x] = 7;
 } else {
 bufferSD[x] = 0;
 }
 }

 sectorSD++;


 LED(3,1);
 }








}




void GuardarInfoSector(unsigned long datoSector, unsigned long localizacionSector){



 unsigned char bufferSectores[512];
 bufferSectores[0] = (datoSector>>24)&0xFF;
 bufferSectores[1] = (datoSector>>16)&0xFF;
 bufferSectores[2] = (datoSector>>8)&0xFF;
 bufferSectores[3] = (datoSector)&0xFF;
 for (x=4;x<512;x++){
 bufferSectores[x] = 0;
 }


 for (x=0;x<5;x++){
 checkEscSD = SD_Write_Block(bufferSectores,localizacionSector);
 if (checkEscSD ==  22 ){

 break;
 }
 Delay_us(10);
 }

}
