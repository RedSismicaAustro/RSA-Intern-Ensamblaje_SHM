#line 1 "C:/Users/User/Downloads/Main_SD/Main_SD/sdcard.c"
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
#line 68 "C:/Users/User/Downloads/Main_SD/Main_SD/sdcard.c"
extern sfr sbit sd_CS_lat;



extern sfr sbit sd_CS_tris;
#line 86 "C:/Users/User/Downloads/Main_SD/Main_SD/sdcard.c"
extern sfr sbit sd_detect_port;
extern sfr sbit sd_detect_tris;





extern struct sdflags sdflags;



unsigned char ccs;


void LEDcard(int veces, unsigned char tiempo_seg){
 unsigned int i;
 TRISAbits.TRISA1 = 0;
 if (tiempo_seg==1){
 for (i=0; i<veces; i++){
 LATAbits.LATA1 = 1;
 Delay_ms(1000);
 LATAbits.LATA1 = 0;
 Delay_ms(1000);
 }
 }else if (tiempo_seg==2){
 for (i=0; i<veces; i++){
 LATAbits.LATA1 = 1;
 Delay_ms(350);
 LATAbits.LATA1 = 0;
 Delay_ms(350);
 }
 }
}





unsigned char SD_Read(unsigned char *Buffer, unsigned int nbytes){
 unsigned int i;
 unsigned char temp;
 for(i = 0; i <  2000 ; i++){
 temp = SPISD_Write(0xFF);
 if(temp == 0xFE) break;
 if(i ==  2000 -1) return  21 ;

 }
 for(i = 0; i < nbytes; i++){
 Buffer[i] = SPISD_Write(0xFF);
 }
 temp = SPISD_Write(0xFF);
 temp = SPISD_Write(0xFF);
 return 0x00;
}










unsigned char SD_Read_Block(unsigned char *Buffer, unsigned long Address){
 unsigned char temp;
 Select_SD();

 if(ccs == 0x02) Address<<=9;
 if(SD_Ready() == 0) return  17 ;
 SD_Send_Command( 0x11 ,Address,0xFF);
 temp = R1_Response();
 if(temp == 0xFF){LEDcard(10,2);}
 if(temp != 0x00) return temp;
 temp = SD_Read(Buffer,512);

 Release_SD();
 return temp;
}










unsigned char SD_Write_Block(unsigned char *Buffer, unsigned long Address){
 unsigned char temp;
 unsigned int i;


 Select_SD();

 if(ccs == 0x02) Address<<=9;
 if(SD_Ready() == 0) return  17 ;
 SD_Send_Command( 0x18 ,Address,0xFF);
 temp = R1_Response();
 if(temp != 0x00) return temp;
 temp = SPISD_Write(0xFE);
 for(i = 0; i < 512; i++){
 temp = SPISD_Write(Buffer[i]);
 }
 temp = SPISD_Write(0xFF);
 temp = SPISD_Write(0xFF);
 temp = SPISD_Write(0xFF);
 temp = (temp&0x0E)>>1;
 if(SD_Ready() == 0) return  17 ;


 Release_SD();
 if(temp == 0x02) return  22 ;
 else if(temp == 0x05) return  23 ;
 else if(temp == 0x06) return  24 ;
 else return  10 ;
}
#line 215 "C:/Users/User/Downloads/Main_SD/Main_SD/sdcard.c"
unsigned char SD_Init_Try(unsigned char try_value){
 unsigned char i,init_status;
 if(try_value == 0) try_value = 1;
 for(i = 0; i < try_value; i++){
 init_status = SD_Init();
 if(init_status ==  0xAA ) break;
 Release_SD();
 Delay_ms(10);
 }
 return init_status;
}










unsigned char SD_Init(void){

 unsigned int i;
 unsigned int j;
 unsigned char temp;
 unsigned long temp_long;


 sd_CS_tris = 0;


 Release_SD();


 SPISD_Init( 0 );



 for(i = 0; i < 80; i++) SPISD_Write(0xFF);


 Select_SD();


 for(i = 0; i < 16; i++) SPISD_Write(0xFF);
#line 272 "C:/Users/User/Downloads/Main_SD/Main_SD/sdcard.c"
 for(i = 0; i <  2000 ; i++){
 SD_Send_Command( 0x00 ,0x00000000,0x4A);
 temp = R1_Response();
 if(temp == (1<< 0 )) {
 break;
 }
 if(i==( 2000 -1)) return  16 ;
 }




 if(SD_Ready() == 0){
 return  17 ;
 }

 SD_Send_Command( 0x08 ,0x000001AA,0x43);
 temp = R1_Response();

 if(temp != (1<< 0 )){

 for(i = 0; i <  2000 ; i++){
 if(SD_Ready() == 0) return  17 ;
 SD_Send_Command( 0x01 ,0x00000000,0x7C);
 temp = R1_Response();
 if(temp == 0x00) break;
 if(i==( 2000 -1)) return  18 ;
 }

 } else if (temp == (1<< 0 )){

 temp_long = Response_32b();
 temp = (temp_long &  0x000000FF );
 if(temp != 0xAA) return  19 ;
 temp = ((temp_long &  0x00000F00 )>>8);
 if(temp != 0x01) return  20 ;


 if(SD_Ready() == 0) return  17 ;
 SD_Send_Command( 0x3A ,0x00000000,0x7E);
 temp = R1_Response();
 if(temp != (1<< 0 )) return temp;
 temp_long = Response_32b();
 if((temp_long &  0x00FF8000 ) !=  0x00FF8000 )
 return  20 ;


 if(SD_Ready() == 0) return  17 ;
 SD_Send_Command( 0x3B ,0x00000001,0x48);
 temp = R1_Response();
 if(temp != (1<< 0 )) return temp;


 for(i = 0; i <  2000 ; i++){
 if(SD_Ready() == 0) return  17 ;
 SD_Send_Command( 0x37 ,0x00000000,0x32);
 temp = R1_Response();
 if(SD_Ready() == 0) return  17 ;


 for(j = 0; j < 16; j++) SPISD_Write(0xFF);

 SD_Send_Command( 0x29 ,0x40000000,0x3B);
 temp = R1_Response();
 if(temp == 0x00) break;
 if(i==( 2000 -1)) return  18 ;
 }
 }
 else return temp;


 if(SD_Ready() == 0) return  17 ;
 SD_Send_Command( 0x3B ,0x00000000,0x48);
 temp = R1_Response();
 if(temp != 0x00) return temp;


 if(SD_Ready() == 0) return  17 ;
 SD_Send_Command( 0x10 ,0x00000200,0x0A);
 temp = R1_Response();
 if(temp != 0x00) return temp;


 if(SD_Ready() == 0) return  17 ;
 SD_Send_Command( 0x3A ,0x00000000,0x7E);
 temp = R1_Response();
 if(temp != 0x00) return temp;
 temp_long = Response_32b();
 ccs = (long)(temp_long >> 30);


 Release_SD();


 for(i = 0; i < 16; i++) SPISD_Write(0xFF);


 SPISD_Init( 1 );

 return  0xAA ;
}








unsigned char R1_Response(void){
 unsigned char temp;
 temp = SPISD_Write(0xFF);
 temp = SPISD_Write(0xFF);
 return temp;
}








unsigned int R2_Response(void){
 unsigned char temp;
 unsigned int response;
 temp = SPISD_Write(0xFF);
 response = SPISD_Write(0xFF);
 temp = SPISD_Write(0xFF);
 response = (response<<8)|temp;
 return response;
}








unsigned long Response_32b(void){
 unsigned char temp;
 unsigned long response;
 response = SPISD_Write(0xFF);
 temp = SPISD_Write(0xFF);
 response = (response<<8)|temp;
 temp = SPISD_Write(0xFF);
 response = (response<<8)|temp;
 temp = SPISD_Write(0xFF);
 response = (response<<8)|temp;
 return response;
}








void SD_Send_Command(unsigned char command,unsigned long argument, unsigned char crc){


 SPISD_Write(command |= 0x40);


 SPISD_Write((unsigned char)(argument>>24));
 SPISD_Write((unsigned char)(argument>>16));
 SPISD_Write((unsigned char)(argument>>8));
 SPISD_Write((unsigned char)(argument));


 SPISD_Write((crc<<1)|0x01);
}










unsigned char SD_Ready(void){
 unsigned int i;
 unsigned char temp;
 for(i = 0; i <  2000 ; i++){
 temp = SPISD_Write(0xFF);
 if(temp == 0xFF) break;
 if(i == ( 2000 -1)) return 0x00;
 }
 return temp;
}










void Release_SD(void){

 sd_CS_lat = 1;
 asm nop;
}










void Select_SD(void){

 sd_CS_lat = 0;
 asm nop;
}
#line 507 "C:/Users/User/Downloads/Main_SD/Main_SD/sdcard.c"
unsigned char SD_Detect(void) {

 if (sd_detect_port == 0) {
 return  0xDE ;

 } else {
 return 0;
 }
}
