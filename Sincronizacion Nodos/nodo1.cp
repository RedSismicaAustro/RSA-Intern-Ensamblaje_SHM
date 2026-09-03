#line 1 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/nodo1.c"
#line 30 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/nodo1.c"
sbit Mmc_Chip_Select at LATB0_bit;
sbit Mmc_Chip_Select_Direction at TRISB0_bit;


sbit TEST1 at LATA2_bit;
sbit TEST1_Direction at TRISA2_bit;



void main(void) {

 unsigned int err;
 unsigned int i;
 unsigned char bufOut[512];
 unsigned char bufIn[512];
 unsigned char ok;


 CLKDIVbits.FRCDIV = 0;
 CLKDIVbits.PLLPOST = 0;
 CLKDIVbits.PLLPRE = 5;
 PLLFBDbits.PLLDIV = 150;

 ANSELA = 0;
 ANSELB = 0;

 TEST1_Direction = 0;
 TEST1 = 0;


 Mmc_Chip_Select_Direction = 0;
 Mmc_Chip_Select = 1;

 TRISB7_bit = 0;
 TRISB8_bit = 0;
 TRISB9_bit = 1;
#line 78 "C:/Users/Public/Documents/Mikroelektronika/mikroC PRO for dsPIC/Examples/nodo1.c"
 RPINR20bits.SDI1R = 41;
 RPOR19bits.RP39R = 8;
 RPOR20bits.RP40R = 7;


 SPI1_Init_Advanced(_SPI_MASTER, _SPI_8_BIT, _SPI_PRESCALE_SEC_1, _SPI_PRESCALE_PRI_64,
 _SPI_SS_DISABLE, _SPI_DATA_SAMPLE_MIDDLE, _SPI_CLK_IDLE_HIGH, _SPI_ACTIVE_2_IDLE);


 if (Mmc_Init()) {

 while (1) {
 TEST1 = 1; Delay_ms(200); TEST1 = 0; Delay_ms(800);
 }
 }




 SPI1_Init_Advanced(_SPI_MASTER, _SPI_8_BIT, _SPI_PRESCALE_SEC_1, _SPI_PRESCALE_PRI_4,
 _SPI_SS_DISABLE, _SPI_DATA_SAMPLE_MIDDLE, _SPI_CLK_IDLE_HIGH, _SPI_ACTIVE_2_IDLE);




 bufOut[0]='P'; bufOut[1]='R'; bufOut[2]='U'; bufOut[3]='E';
 bufOut[4]='B'; bufOut[5]='A'; bufOut[6]='_'; bufOut[7]='S';
 bufOut[8]='D'; bufOut[9]='_'; bufOut[10]='O'; bufOut[11]='K';
 bufOut[12]='\r'; bufOut[13]='\n';
 for (i = 14; i < 512; i++) {
 bufOut[i] = (unsigned char)(i & 0xFF);
 }


 err = Mmc_Write_Sector( 200000UL , bufOut);
 if (err != 0) {

 while (1) {
 TEST1 = 1; Delay_ms(300); TEST1 = 0; Delay_ms(300);
 }
 }


 err = Mmc_Read_Sector( 200000UL , bufIn);
 if (err != 0) {

 while (1) {
 TEST1 = 1; Delay_ms(300); TEST1 = 0; Delay_ms(300);
 }
 }


 ok = 1;
 for (i = 0; i < 512; i++) {
 if (bufIn[i] != bufOut[i]) {
 ok = 0;
 break;
 }
 }

 if (ok) {

 for (i = 0; i < 5; i++) {
 TEST1 = 1; Delay_ms(100);
 TEST1 = 0; Delay_ms(100);
 }
 TEST1 = 1;
 } else {

 while (1) {
 TEST1 = 1; Delay_ms(500); TEST1 = 0; Delay_ms(500);
 }
 }

 while (1) {
 asm CLRWDT;
 }
}
