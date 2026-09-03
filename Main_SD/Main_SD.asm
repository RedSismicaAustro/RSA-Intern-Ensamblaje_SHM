
_main:
	MOV	#2048, W15
	MOV	#6142, W0
	MOV	WREG, 32
	MOV	#1, W0
	MOV	WREG, 50
	MOV	#4, W0
	IOR	68

;Main_SD.c,67 :: 		void main() {
;Main_SD.c,68 :: 		ConfiguracionPrincipal();
	PUSH	W10
	CALL	_ConfiguracionPrincipal
;Main_SD.c,69 :: 		TEST = 0;
	BCLR	LATA2_bit, BitPos(LATA2_bit+0)
;Main_SD.c,70 :: 		TEST = 1;
	BSET	LATA2_bit, BitPos(LATA2_bit+0)
;Main_SD.c,71 :: 		Delay_ms(1000);
	MOV	#123, W8
	MOV	#4681, W7
L_main0:
	DEC	W7
	BRA NZ	L_main0
	DEC	W8
	BRA NZ	L_main0
;Main_SD.c,72 :: 		TEST = 0;
	BCLR	LATA2_bit, BitPos(LATA2_bit+0)
;Main_SD.c,74 :: 		i = 0;
	CLR	W0
	MOV	W0, _i
;Main_SD.c,75 :: 		j = 0;
	CLR	W0
	MOV	W0, _j
;Main_SD.c,76 :: 		x = 0;
	CLR	W0
	MOV	W0, _x
;Main_SD.c,77 :: 		y = 0;
	CLR	W0
	MOV	W0, _y
;Main_SD.c,80 :: 		inicioSistema = 0;
	MOV	#lo_addr(_inicioSistema), W1
	CLR	W0
	MOV.B	W0, [W1]
;Main_SD.c,83 :: 		horaSistema = 0;
	CLR	W0
	CLR	W1
	MOV	W0, _horaSistema
	MOV	W1, _horaSistema+2
;Main_SD.c,84 :: 		fechaSistema = 0;
	CLR	W0
	CLR	W1
	MOV	W0, _fechaSistema
	MOV	W1, _fechaSistema+2
;Main_SD.c,87 :: 		PSEC = 0;
	CLR	W0
	CLR	W1
	MOV	W0, _PSEC
	MOV	W1, _PSEC+2
;Main_SD.c,88 :: 		sectorSD = 0;
	CLR	W0
	CLR	W1
	MOV	W0, _sectorSD
	MOV	W1, _sectorSD+2
;Main_SD.c,89 :: 		sectorLec = 0;
	CLR	W0
	CLR	W1
	MOV	W0, _sectorLec
	MOV	W1, _sectorLec+2
;Main_SD.c,90 :: 		checkEscSD = 0;
	MOV	#lo_addr(_checkEscSD), W1
	CLR	W0
	MOV.B	W0, [W1]
;Main_SD.c,91 :: 		checkLecSD = 0;
	MOV	#lo_addr(_checkLecSD), W1
	CLR	W0
	MOV.B	W0, [W1]
;Main_SD.c,92 :: 		MSRS485 = 0;                                                               //Establece el Max485 en modo lectura
	BCLR	LATB12_bit, BitPos(LATB12_bit+0)
;Main_SD.c,93 :: 		banInsSec = 0;
	MOV	#lo_addr(_banInsSec), W1
	CLR	W0
	MOV.B	W0, [W1]
;Main_SD.c,96 :: 		switch (SIZESD){
	GOTO	L_main2
;Main_SD.c,110 :: 		case 16:
L_main7:
;Main_SD.c,111 :: 		PSF = 2048;
	MOV	#2048, W0
	MOV	#0, W1
	MOV	W0, _PSF
	MOV	W1, _PSF+2
;Main_SD.c,112 :: 		USF = 31115263;
	MOV	#51199, W0
	MOV	#474, W1
	MOV	W0, _USF
	MOV	W1, _USF+2
;Main_SD.c,113 :: 		break;
	GOTO	L_main3
;Main_SD.c,114 :: 		}
L_main2:
	GOTO	L_main7
L_main3:
;Main_SD.c,115 :: 		infoPrimerSector = PSF+DELTASECTOR-2;                                      //Calcula el sector donde se alamcena la informacion del primer sector escrito
	MOV	_PSF, W2
	MOV	_PSF+2, W3
	MOV	#32416, W0
	MOV	#1, W1
	ADD	W2, W0, W2
	ADDC	W3, W1, W3
	MOV	#lo_addr(_infoPrimerSector), W0
	SUB	W2, #2, [W0++]
	SUBB	W3, #0, [W0--]
;Main_SD.c,116 :: 		infoUltimoSector = PSF+DELTASECTOR-1;                                      //Calcula el sector donde se alamcena la informacion del ultimo sector escrito
	MOV	#lo_addr(_infoUltimoSector), W0
	SUB	W2, #1, [W0++]
	SUBB	W3, #0, [W0--]
;Main_SD.c,117 :: 		PSE = PSF+DELTASECTOR;
	MOV	W2, _PSE
	MOV	W3, _PSE+2
;Main_SD.c,120 :: 		horaSistema = 86100;        //23:55:00
	MOV	#20564, W0
	MOV	#1, W1
	MOV	W0, _horaSistema
	MOV	W1, _horaSistema+2
;Main_SD.c,121 :: 		fechaSistema = 200228;      //aa/mm/dd
	MOV	#3620, W0
	MOV	#3, W1
	MOV	W0, _fechaSistema
	MOV	W1, _fechaSistema+2
;Main_SD.c,138 :: 		sdflags.detected = true;
	MOV	#lo_addr(_sdflags), W0
	BSET.B	[W0], #1
;Main_SD.c,142 :: 		if (sdflags.detected && !sdflags.init_ok) {
	MOV	#lo_addr(_sdflags), W0
	MOV.B	[W0], W0
	BTSS.B	W0, #1
	GOTO	L__main101
	MOV	#lo_addr(_sdflags), W0
	MOV.B	[W0], W0
	BTSC.B	W0, #0
	GOTO	L__main100
L__main99:
;Main_SD.c,143 :: 		checkEscSD = SD_Init_Try(10);
	MOV.B	#10, W10
	CALL	_SD_Init_Try
	MOV	#lo_addr(_checkEscSD), W1
	MOV.B	W0, [W1]
;Main_SD.c,144 :: 		if (checkEscSD == SUCCESSFUL_INIT) {
	MOV.B	#170, W1
	CP.B	W0, W1
	BRA Z	L__main103
	GOTO	L_main11
L__main103:
;Main_SD.c,145 :: 		sdflags.init_ok = true;
	MOV	#lo_addr(_sdflags), W0
	BSET.B	[W0], #0
;Main_SD.c,146 :: 		inicioSistema = 1;                                                //Activa la bandera para permitir el inicio del sistema
	MOV	#lo_addr(_inicioSistema), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
;Main_SD.c,147 :: 		TEST = 1;
	BSET	LATA2_bit, BitPos(LATA2_bit+0)
;Main_SD.c,148 :: 		} else {
	GOTO	L_main12
L_main11:
;Main_SD.c,149 :: 		sdflags.init_ok = false;
	MOV	#lo_addr(_sdflags), W0
	BCLR.B	[W0], #0
;Main_SD.c,150 :: 		INT1IE_bit = 0;                                                   //Desabilita la interrupcion externa INT1
	BCLR	INT1IE_bit, BitPos(INT1IE_bit+0)
;Main_SD.c,151 :: 		U1MODE.UARTEN = 0;                                                //Desabilita el UART
	BCLR	U1MODE, #15
;Main_SD.c,152 :: 		inicioSistema = 0;                                                //Apaga la bandera de inicio del sistema
	MOV	#lo_addr(_inicioSistema), W1
	CLR	W0
	MOV.B	W0, [W1]
;Main_SD.c,153 :: 		LED_Error(checkEscSD);
	MOV	#lo_addr(_checkEscSD), W0
	MOV.B	[W0], W10
	CALL	_LED_Error
;Main_SD.c,154 :: 		}
L_main12:
;Main_SD.c,142 :: 		if (sdflags.detected && !sdflags.init_ok) {
L__main101:
L__main100:
;Main_SD.c,158 :: 		Ejemplo_uso_SD();
	CALL	_Ejemplo_uso_SD
;Main_SD.c,161 :: 		}
L_end_main:
	POP	W10
L__main_end_loop:
	BRA	L__main_end_loop
; end of _main

_Ejemplo_uso_SD:
	LNK	#1024

;Main_SD.c,167 :: 		void Ejemplo_uso_SD(){
;Main_SD.c,178 :: 		valor = 1; // Iniciar con el valor 1
	PUSH	W10
	PUSH	W11
	PUSH	W12
; valor start address is: 4 (W2)
	MOV.B	#1, W2
;Main_SD.c,179 :: 		for (i = 0; i < 512; i++) {
	CLR	W0
	MOV	W0, _i
; valor end address is: 4 (W2)
L_Ejemplo_uso_SD13:
; valor start address is: 4 (W2)
	MOV	_i, W1
	MOV	#512, W0
	CP	W1, W0
	BRA LTU	L__Ejemplo_uso_SD106
	GOTO	L_Ejemplo_uso_SD14
L__Ejemplo_uso_SD106:
;Main_SD.c,180 :: 		data_to_write[i] = valor;
	ADD	W14, #0, W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], W0
	MOV.B	W2, [W0]
;Main_SD.c,181 :: 		valor++; // Incrementa el valor para la pr?xima iteraci?n
	ADD.B	W2, #1, W0
	MOV.B	W0, W2
;Main_SD.c,183 :: 		if (valor == 256) {
	ZE	W0, W1
	MOV	#256, W0
	CP	W1, W0
	BRA Z	L__Ejemplo_uso_SD107
	GOTO	L__Ejemplo_uso_SD98
L__Ejemplo_uso_SD107:
;Main_SD.c,184 :: 		valor = 1;
	MOV.B	#1, W2
; valor end address is: 4 (W2)
;Main_SD.c,185 :: 		}
	GOTO	L_Ejemplo_uso_SD16
L__Ejemplo_uso_SD98:
;Main_SD.c,183 :: 		if (valor == 256) {
;Main_SD.c,185 :: 		}
L_Ejemplo_uso_SD16:
;Main_SD.c,179 :: 		for (i = 0; i < 512; i++) {
; valor start address is: 4 (W2)
	MOV	#1, W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,186 :: 		}
; valor end address is: 4 (W2)
	GOTO	L_Ejemplo_uso_SD13
L_Ejemplo_uso_SD14:
;Main_SD.c,189 :: 		sector = 2500;  // Sector donde se va a escribir
; sector start address is: 6 (W3)
	MOV	#2500, W3
	MOV	#0, W4
;Main_SD.c,190 :: 		checkEscSD = SD_Write_Block(data_to_write,sector);
	ADD	W14, #0, W0
	PUSH	W3
	PUSH	W4
	MOV	#2500, W11
	MOV	#0, W12
	MOV	W0, W10
	CALL	_SD_Write_Block
	POP	W4
	POP	W3
	MOV	#lo_addr(_checkEscSD), W1
	MOV.B	W0, [W1]
;Main_SD.c,191 :: 		if (checkEscSD == DATA_ACCEPTED){
	CP.B	W0, #22
	BRA Z	L__Ejemplo_uso_SD108
	GOTO	L_Ejemplo_uso_SD17
L__Ejemplo_uso_SD108:
;Main_SD.c,192 :: 		LED(1,1);
	MOV.B	#1, W11
	MOV	#1, W10
	CALL	_LED
;Main_SD.c,193 :: 		} else {
	GOTO	L_Ejemplo_uso_SD18
L_Ejemplo_uso_SD17:
;Main_SD.c,194 :: 		LED_Error(10);
	MOV.B	#10, W10
	CALL	_LED_Error
;Main_SD.c,195 :: 		}
L_Ejemplo_uso_SD18:
;Main_SD.c,198 :: 		checkLecSD = SD_Read_Block(buffer, sector);
	MOV	#512, W0
	ADD	W14, W0, W0
	PUSH	W3
	PUSH	W4
	MOV	W3, W11
	MOV	W4, W12
	MOV	W0, W10
	CALL	_SD_Read_Block
	POP	W4
	POP	W3
	MOV	#lo_addr(_checkLecSD), W1
	MOV.B	W0, [W1]
;Main_SD.c,199 :: 		if (checkLecSD==0) {
	CP.B	W0, #0
	BRA Z	L__Ejemplo_uso_SD109
	GOTO	L_Ejemplo_uso_SD19
L__Ejemplo_uso_SD109:
;Main_SD.c,200 :: 		LED(1,1);
	MOV.B	#1, W11
	MOV	#1, W10
	CALL	_LED
;Main_SD.c,201 :: 		} else {
	GOTO	L_Ejemplo_uso_SD20
L_Ejemplo_uso_SD19:
;Main_SD.c,202 :: 		LED_Error(11);
	MOV.B	#11, W10
	CALL	_LED_Error
;Main_SD.c,203 :: 		}
L_Ejemplo_uso_SD20:
;Main_SD.c,208 :: 		checkEscSD = SD_Write_Block(buffer,sector+1);
	ADD	W3, #1, W1
	ADDC	W4, #0, W2
; sector end address is: 6 (W3)
	MOV	#512, W0
	ADD	W14, W0, W0
	MOV	W1, W11
	MOV	W2, W12
	MOV	W0, W10
	CALL	_SD_Write_Block
	MOV	#lo_addr(_checkEscSD), W1
	MOV.B	W0, [W1]
;Main_SD.c,209 :: 		if (checkEscSD == DATA_ACCEPTED){
	CP.B	W0, #22
	BRA Z	L__Ejemplo_uso_SD110
	GOTO	L_Ejemplo_uso_SD21
L__Ejemplo_uso_SD110:
;Main_SD.c,210 :: 		LED(1,1);
	MOV.B	#1, W11
	MOV	#1, W10
	CALL	_LED
;Main_SD.c,211 :: 		} else {
	GOTO	L_Ejemplo_uso_SD22
L_Ejemplo_uso_SD21:
;Main_SD.c,212 :: 		LED_Error(12);
	MOV.B	#12, W10
	CALL	_LED_Error
;Main_SD.c,213 :: 		}
L_Ejemplo_uso_SD22:
;Main_SD.c,215 :: 		}
L_end_Ejemplo_uso_SD:
	POP	W12
	POP	W11
	POP	W10
	ULNK
	RETURN
; end of _Ejemplo_uso_SD

_LED:

;Main_SD.c,218 :: 		void LED(int veces, unsigned char tiempo_seg){
;Main_SD.c,223 :: 		TEST_Direction = 0; // Configura RA2 como salida
	BCLR	TRISA2_bit, BitPos(TRISA2_bit+0)
;Main_SD.c,224 :: 		if (tiempo_seg==1){
	CP.B	W11, #1
	BRA Z	L__LED112
	GOTO	L_LED23
L__LED112:
;Main_SD.c,225 :: 		for (i=0; i<veces; i++){
; i start address is: 0 (W0)
	CLR	W0
; i end address is: 0 (W0)
L_LED24:
; i start address is: 0 (W0)
	CP	W0, W10
	BRA LTU	L__LED113
	GOTO	L_LED25
L__LED113:
;Main_SD.c,226 :: 		TEST = 1; // Enciende el LED
	BSET	LATA2_bit, BitPos(LATA2_bit+0)
;Main_SD.c,227 :: 		Delay_ms(1000);   // Espera
	MOV	#123, W8
	MOV	#4681, W7
L_LED27:
	DEC	W7
	BRA NZ	L_LED27
	DEC	W8
	BRA NZ	L_LED27
;Main_SD.c,228 :: 		TEST = 0; // Apaga el LED
	BCLR	LATA2_bit, BitPos(LATA2_bit+0)
;Main_SD.c,229 :: 		Delay_ms(1000);   // Espera
	MOV	#123, W8
	MOV	#4681, W7
L_LED29:
	DEC	W7
	BRA NZ	L_LED29
	DEC	W8
	BRA NZ	L_LED29
;Main_SD.c,225 :: 		for (i=0; i<veces; i++){
	INC	W0
;Main_SD.c,230 :: 		}
; i end address is: 0 (W0)
	GOTO	L_LED24
L_LED25:
;Main_SD.c,231 :: 		}else if (tiempo_seg==2){
	GOTO	L_LED31
L_LED23:
	CP.B	W11, #2
	BRA Z	L__LED114
	GOTO	L_LED32
L__LED114:
;Main_SD.c,232 :: 		for (i=0; i<veces; i++){
; i start address is: 0 (W0)
	CLR	W0
; i end address is: 0 (W0)
L_LED33:
; i start address is: 0 (W0)
	CP	W0, W10
	BRA LTU	L__LED115
	GOTO	L_LED34
L__LED115:
;Main_SD.c,233 :: 		TEST = 1; // Enciende el LED
	BSET	LATA2_bit, BitPos(LATA2_bit+0)
;Main_SD.c,234 :: 		Delay_ms(2000);   // Espera
	MOV	#245, W8
	MOV	#9362, W7
L_LED36:
	DEC	W7
	BRA NZ	L_LED36
	DEC	W8
	BRA NZ	L_LED36
	NOP
;Main_SD.c,235 :: 		TEST = 0; // Apaga el LED
	BCLR	LATA2_bit, BitPos(LATA2_bit+0)
;Main_SD.c,236 :: 		Delay_ms(2000);   // Espera
	MOV	#245, W8
	MOV	#9362, W7
L_LED38:
	DEC	W7
	BRA NZ	L_LED38
	DEC	W8
	BRA NZ	L_LED38
	NOP
;Main_SD.c,232 :: 		for (i=0; i<veces; i++){
	INC	W0
;Main_SD.c,237 :: 		}
; i end address is: 0 (W0)
	GOTO	L_LED33
L_LED34:
;Main_SD.c,238 :: 		}
L_LED32:
L_LED31:
;Main_SD.c,239 :: 		}
L_end_LED:
	RETURN
; end of _LED

_LED_Error:

;Main_SD.c,242 :: 		void LED_Error(unsigned char codigo){
;Main_SD.c,244 :: 		parpadeos = codigo;
; parpadeos start address is: 0 (W0)
	MOV.B	W10, W0
;Main_SD.c,245 :: 		if (parpadeos > 9) parpadeos = parpadeos - 9;
	CP.B	W10, #9
	BRA GTU	L__LED_Error117
	GOTO	L__LED_Error97
L__LED_Error117:
; parpadeos start address is: 0 (W0)
	SUB.B	W0, #9, W0
; parpadeos end address is: 0 (W0)
; parpadeos end address is: 0 (W0)
	MOV.B	W0, W2
	GOTO	L_LED_Error40
L__LED_Error97:
	MOV.B	W0, W2
L_LED_Error40:
;Main_SD.c,246 :: 		while (1){
; parpadeos start address is: 4 (W2)
; parpadeos end address is: 4 (W2)
L_LED_Error41:
;Main_SD.c,247 :: 		for (i = 0; i < parpadeos; i++){
; parpadeos start address is: 4 (W2)
	CLR	W0
	MOV	W0, _i
; parpadeos end address is: 4 (W2)
L_LED_Error43:
; parpadeos start address is: 4 (W2)
	ZE	W2, W1
	MOV	#lo_addr(_i), W0
	CP	W1, [W0]
	BRA GTU	L__LED_Error118
	GOTO	L_LED_Error44
L__LED_Error118:
;Main_SD.c,248 :: 		TEST = 1;
	BSET	LATA2_bit, BitPos(LATA2_bit+0)
;Main_SD.c,249 :: 		Delay_ms(500);
	MOV	#62, W8
	MOV	#2340, W7
L_LED_Error46:
	DEC	W7
	BRA NZ	L_LED_Error46
	DEC	W8
	BRA NZ	L_LED_Error46
	NOP
	NOP
;Main_SD.c,250 :: 		TEST = 0;
	BCLR	LATA2_bit, BitPos(LATA2_bit+0)
;Main_SD.c,251 :: 		Delay_ms(500);
	MOV	#62, W8
	MOV	#2340, W7
L_LED_Error48:
	DEC	W7
	BRA NZ	L_LED_Error48
	DEC	W8
	BRA NZ	L_LED_Error48
	NOP
	NOP
;Main_SD.c,247 :: 		for (i = 0; i < parpadeos; i++){
	MOV	#1, W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,252 :: 		}
	GOTO	L_LED_Error43
L_LED_Error44:
;Main_SD.c,253 :: 		Delay_ms(3000);
	MOV	#367, W8
	MOV	#14043, W7
L_LED_Error50:
	DEC	W7
	BRA NZ	L_LED_Error50
	DEC	W8
	BRA NZ	L_LED_Error50
	NOP
	NOP
;Main_SD.c,254 :: 		}
; parpadeos end address is: 4 (W2)
	GOTO	L_LED_Error41
;Main_SD.c,255 :: 		}
L_end_LED_Error:
	RETURN
; end of _LED_Error

_ConfiguracionPrincipal:

;Main_SD.c,262 :: 		void ConfiguracionPrincipal(){
;Main_SD.c,263 :: 		Delay_ms(4000);   // Espera
	MOV	#489, W8
	MOV	#18724, W7
L_ConfiguracionPrincipal52:
	DEC	W7
	BRA NZ	L_ConfiguracionPrincipal52
	DEC	W8
	BRA NZ	L_ConfiguracionPrincipal52
	NOP
	NOP
	NOP
;Main_SD.c,266 :: 		ANSELA = 0;
	CLR	ANSELA
;Main_SD.c,267 :: 		ANSELB = 0;
	CLR	ANSELB
;Main_SD.c,268 :: 		ConfigurarPPS_SPI1();
	CALL	_ConfigurarPPS_SPI1
;Main_SD.c,269 :: 		TEST_Direction = 0;                                                        // LED como salida
	BCLR	TRISA2_bit, BitPos(TRISA2_bit+0)
;Main_SD.c,270 :: 		TEST = 0;
	BCLR	LATA2_bit, BitPos(LATA2_bit+0)
;Main_SD.c,271 :: 		sd_CS_tris = 0;                                                            //CS SD
	BCLR	TRISB0_bit, BitPos(TRISB0_bit+0)
;Main_SD.c,272 :: 		sd_detect_tris = 1;                                                        //Pin detection SD
	BSET	TRISA4_bit, BitPos(TRISA4_bit+0)
;Main_SD.c,275 :: 		sdflags.detected = false;
	MOV	#lo_addr(_sdflags), W0
	BCLR.B	[W0], #1
;Main_SD.c,276 :: 		sdflags.init_ok = false;
	MOV	#lo_addr(_sdflags), W0
	BCLR.B	[W0], #0
;Main_SD.c,277 :: 		sdflags.saving = false;
	MOV	#lo_addr(_sdflags), W0
	BCLR.B	[W0], #2
;Main_SD.c,279 :: 		Delay_ms(200);                                                             //Espera hasta que se estabilicen los cambios
	MOV	#25, W8
	MOV	#27150, W7
L_ConfiguracionPrincipal54:
	DEC	W7
	BRA NZ	L_ConfiguracionPrincipal54
	DEC	W8
	BRA NZ	L_ConfiguracionPrincipal54
	NOP
;Main_SD.c,280 :: 		}
L_end_ConfiguracionPrincipal:
	RETURN
; end of _ConfiguracionPrincipal

_ConfigurarPPS_SPI1:

;Main_SD.c,283 :: 		void ConfigurarPPS_SPI1(){
;Main_SD.c,285 :: 		MOV #0x46, W0
	MOV	#70, W0
;Main_SD.c,286 :: 		MOV #0x57, W1
	MOV	#87, W1
;Main_SD.c,287 :: 		MOV #0x0742, W2
	MOV	#1858, W2
;Main_SD.c,288 :: 		MOV.b W0, [W2]
	MOV.B	W0, [W2]
;Main_SD.c,289 :: 		MOV.b W1, [W2]
	MOV.B	W1, [W2]
;Main_SD.c,290 :: 		BCLR OSCCON, #6
	BCLR	OSCCON, #6
;Main_SD.c,293 :: 		RPOR2 = (RPOR2 & 0x00FF) | 0x0600;
	MOV	#255, W1
	MOV	#lo_addr(RPOR2), W0
	AND	W1, [W0], W2
	MOV	#1536, W1
	MOV	#lo_addr(RPOR2), W0
	IOR	W2, W1, [W0]
;Main_SD.c,294 :: 		RPOR3 = (RPOR3 & 0xFF00) | 0x0005;
	MOV	RPOR3, W1
	MOV	#65280, W0
	AND	W1, W0, W1
	MOV	#lo_addr(RPOR3), W0
	IOR	W1, #5, [W0]
;Main_SD.c,295 :: 		RPINR20 = (RPINR20 & 0xFF00) | 41;
	MOV	1736, W1
	MOV	#65280, W0
	AND	W1, W0, W2
	MOV	#41, W1
	MOV	#1736, W0
	IOR	W2, W1, [W0]
;Main_SD.c,297 :: 		TRISBbits.TRISB7 = 0;
	BCLR.B	TRISBbits, #7
;Main_SD.c,298 :: 		TRISBbits.TRISB8 = 0;
	BCLR	TRISBbits, #8
;Main_SD.c,299 :: 		TRISBbits.TRISB9 = 1;
	BSET	TRISBbits, #9
;Main_SD.c,300 :: 		}
L_end_ConfigurarPPS_SPI1:
	RETURN
; end of _ConfigurarPPS_SPI1

_GuardarBufferSD:

;Main_SD.c,305 :: 		void GuardarBufferSD(unsigned char* bufferLleno, unsigned long sector){
;Main_SD.c,307 :: 		for (x=0;x<5;x++){
	CLR	W0
	MOV	W0, _x
L_GuardarBufferSD56:
	MOV	_x, W0
	CP	W0, #5
	BRA LTU	L__GuardarBufferSD122
	GOTO	L_GuardarBufferSD57
L__GuardarBufferSD122:
;Main_SD.c,308 :: 		checkEscSD = SD_Write_Block(bufferLleno,sector);
	PUSH	W11
	PUSH	W12
	PUSH	W10
	CALL	_SD_Write_Block
	POP	W10
	POP	W12
	POP	W11
	MOV	#lo_addr(_checkEscSD), W1
	MOV.B	W0, [W1]
;Main_SD.c,309 :: 		if (checkEscSD == DATA_ACCEPTED){
	CP.B	W0, #22
	BRA Z	L__GuardarBufferSD123
	GOTO	L_GuardarBufferSD59
L__GuardarBufferSD123:
;Main_SD.c,310 :: 		break;
	GOTO	L_GuardarBufferSD57
;Main_SD.c,311 :: 		}
L_GuardarBufferSD59:
;Main_SD.c,312 :: 		Delay_us(10);
	MOV	#80, W7
L_GuardarBufferSD60:
	DEC	W7
	BRA NZ	L_GuardarBufferSD60
	NOP
	NOP
;Main_SD.c,307 :: 		for (x=0;x<5;x++){
	MOV	#1, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,313 :: 		}
	GOTO	L_GuardarBufferSD56
L_GuardarBufferSD57:
;Main_SD.c,314 :: 		}
L_end_GuardarBufferSD:
	RETURN
; end of _GuardarBufferSD

_GuardarTramaSD:
	LNK	#512

;Main_SD.c,319 :: 		void GuardarTramaSD(){
;Main_SD.c,324 :: 		sectorSD=40000;
	PUSH	W10
	PUSH	W11
	PUSH	W12
	MOV	#40000, W0
	MOV	#0, W1
	MOV	W0, _sectorSD
	MOV	W1, _sectorSD+2
;Main_SD.c,325 :: 		for (sectorSD=40000; sectorSD<40020;sectorSD++){
	MOV	#40000, W0
	MOV	#0, W1
	MOV	W0, _sectorSD
	MOV	W1, _sectorSD+2
L_GuardarTramaSD62:
	MOV	#40020, W1
	MOV	#0, W2
	MOV	#lo_addr(_sectorSD), W0
	CP	W1, [W0++]
	CPB	W2, [W0--]
	BRA GTU	L__GuardarTramaSD125
	GOTO	L_GuardarTramaSD63
L__GuardarTramaSD125:
;Main_SD.c,327 :: 		for (x=0;x<6;x++){
	CLR	W0
	MOV	W0, _x
L_GuardarTramaSD65:
	MOV	_x, W0
	CP	W0, #6
	BRA LTU	L__GuardarTramaSD126
	GOTO	L_GuardarTramaSD66
L__GuardarTramaSD126:
;Main_SD.c,328 :: 		bufferSD[x] = 1;
	ADD	W14, #0, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
;Main_SD.c,327 :: 		for (x=0;x<6;x++){
	MOV	#1, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,329 :: 		}
	GOTO	L_GuardarTramaSD65
L_GuardarTramaSD66:
;Main_SD.c,331 :: 		for (x=0;x<6;x++){
	CLR	W0
	MOV	W0, _x
L_GuardarTramaSD68:
	MOV	_x, W0
	CP	W0, #6
	BRA LTU	L__GuardarTramaSD127
	GOTO	L_GuardarTramaSD69
L__GuardarTramaSD127:
;Main_SD.c,332 :: 		bufferSD[6+x] = 2;
	MOV	_x, W0
	ADD	W0, #6, W1
	ADD	W14, #0, W0
	ADD	W0, W1, W1
	MOV.B	#2, W0
	MOV.B	W0, [W1]
;Main_SD.c,331 :: 		for (x=0;x<6;x++){
	MOV	#1, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,333 :: 		}
	GOTO	L_GuardarTramaSD68
L_GuardarTramaSD69:
;Main_SD.c,335 :: 		for (x=0;x<500;x++){
	CLR	W0
	MOV	W0, _x
L_GuardarTramaSD71:
	MOV	_x, W1
	MOV	#500, W0
	CP	W1, W0
	BRA LTU	L__GuardarTramaSD128
	GOTO	L_GuardarTramaSD72
L__GuardarTramaSD128:
;Main_SD.c,336 :: 		bufferSD[12+x] = 3;
	MOV	_x, W0
	ADD	W0, #12, W1
	ADD	W14, #0, W0
	ADD	W0, W1, W1
	MOV.B	#3, W0
	MOV.B	W0, [W1]
;Main_SD.c,335 :: 		for (x=0;x<500;x++){
	MOV	#1, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,337 :: 		}
	GOTO	L_GuardarTramaSD71
L_GuardarTramaSD72:
;Main_SD.c,340 :: 		GuardarBufferSD(bufferSD, sectorSD);
	ADD	W14, #0, W0
	MOV	_sectorSD, W11
	MOV	_sectorSD+2, W12
	MOV	W0, W10
	CALL	_GuardarBufferSD
;Main_SD.c,342 :: 		sectorSD++;
	MOV	#1, W1
	MOV	#0, W2
	MOV	#lo_addr(_sectorSD), W0
	ADD	W1, [W0], [W0++]
	ADDC	W2, [W0], [W0--]
;Main_SD.c,346 :: 		for (x=0;x<512;x++){
	CLR	W0
	MOV	W0, _x
L_GuardarTramaSD74:
	MOV	_x, W1
	MOV	#512, W0
	CP	W1, W0
	BRA LTU	L__GuardarTramaSD129
	GOTO	L_GuardarTramaSD75
L__GuardarTramaSD129:
;Main_SD.c,347 :: 		bufferSD[x] = 4;
	ADD	W14, #0, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], W1
	MOV.B	#4, W0
	MOV.B	W0, [W1]
;Main_SD.c,346 :: 		for (x=0;x<512;x++){
	MOV	#1, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,348 :: 		}
	GOTO	L_GuardarTramaSD74
L_GuardarTramaSD75:
;Main_SD.c,349 :: 		GuardarBufferSD(bufferSD, sectorSD);
	ADD	W14, #0, W0
	MOV	_sectorSD, W11
	MOV	_sectorSD+2, W12
	MOV	W0, W10
	CALL	_GuardarBufferSD
;Main_SD.c,350 :: 		sectorSD++;
	MOV	#1, W1
	MOV	#0, W2
	MOV	#lo_addr(_sectorSD), W0
	ADD	W1, [W0], [W0++]
	ADDC	W2, [W0], [W0--]
;Main_SD.c,353 :: 		for (x=0;x<512;x++){
	CLR	W0
	MOV	W0, _x
L_GuardarTramaSD77:
	MOV	_x, W1
	MOV	#512, W0
	CP	W1, W0
	BRA LTU	L__GuardarTramaSD130
	GOTO	L_GuardarTramaSD78
L__GuardarTramaSD130:
;Main_SD.c,354 :: 		bufferSD[x] = 5;
	ADD	W14, #0, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], W1
	MOV.B	#5, W0
	MOV.B	W0, [W1]
;Main_SD.c,353 :: 		for (x=0;x<512;x++){
	MOV	#1, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,355 :: 		}
	GOTO	L_GuardarTramaSD77
L_GuardarTramaSD78:
;Main_SD.c,356 :: 		GuardarBufferSD(bufferSD, sectorSD);
	ADD	W14, #0, W0
	MOV	_sectorSD, W11
	MOV	_sectorSD+2, W12
	MOV	W0, W10
	CALL	_GuardarBufferSD
;Main_SD.c,357 :: 		sectorSD++;
	MOV	#1, W1
	MOV	#0, W2
	MOV	#lo_addr(_sectorSD), W0
	ADD	W1, [W0], [W0++]
	ADDC	W2, [W0], [W0--]
;Main_SD.c,360 :: 		for (x=0;x<512;x++){
	CLR	W0
	MOV	W0, _x
L_GuardarTramaSD80:
	MOV	_x, W1
	MOV	#512, W0
	CP	W1, W0
	BRA LTU	L__GuardarTramaSD131
	GOTO	L_GuardarTramaSD81
L__GuardarTramaSD131:
;Main_SD.c,361 :: 		bufferSD[x] = 6;
	ADD	W14, #0, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], W1
	MOV.B	#6, W0
	MOV.B	W0, [W1]
;Main_SD.c,360 :: 		for (x=0;x<512;x++){
	MOV	#1, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,362 :: 		}
	GOTO	L_GuardarTramaSD80
L_GuardarTramaSD81:
;Main_SD.c,363 :: 		GuardarBufferSD(bufferSD, sectorSD);
	ADD	W14, #0, W0
	MOV	_sectorSD, W11
	MOV	_sectorSD+2, W12
	MOV	W0, W10
	CALL	_GuardarBufferSD
;Main_SD.c,364 :: 		sectorSD++;
	MOV	#1, W1
	MOV	#0, W2
	MOV	#lo_addr(_sectorSD), W0
	ADD	W1, [W0], [W0++]
	ADDC	W2, [W0], [W0--]
;Main_SD.c,367 :: 		for (x=0;x<512;x++){
	CLR	W0
	MOV	W0, _x
L_GuardarTramaSD83:
	MOV	_x, W1
	MOV	#512, W0
	CP	W1, W0
	BRA LTU	L__GuardarTramaSD132
	GOTO	L_GuardarTramaSD84
L__GuardarTramaSD132:
;Main_SD.c,368 :: 		if (x<464){
	MOV	_x, W1
	MOV	#464, W0
	CP	W1, W0
	BRA LTU	L__GuardarTramaSD133
	GOTO	L_GuardarTramaSD86
L__GuardarTramaSD133:
;Main_SD.c,369 :: 		bufferSD[x] = 7;
	ADD	W14, #0, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], W1
	MOV.B	#7, W0
	MOV.B	W0, [W1]
;Main_SD.c,370 :: 		} else {
	GOTO	L_GuardarTramaSD87
L_GuardarTramaSD86:
;Main_SD.c,371 :: 		bufferSD[x] = 0;
	ADD	W14, #0, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], W1
	CLR	W0
	MOV.B	W0, [W1]
;Main_SD.c,372 :: 		}
L_GuardarTramaSD87:
;Main_SD.c,367 :: 		for (x=0;x<512;x++){
	MOV	#1, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,373 :: 		}
	GOTO	L_GuardarTramaSD83
L_GuardarTramaSD84:
;Main_SD.c,375 :: 		sectorSD++;
	MOV	#1, W1
	MOV	#0, W2
	MOV	#lo_addr(_sectorSD), W0
	ADD	W1, [W0], [W0++]
	ADDC	W2, [W0], [W0--]
;Main_SD.c,378 :: 		LED(3,1);
	MOV.B	#1, W11
	MOV	#3, W10
	CALL	_LED
;Main_SD.c,325 :: 		for (sectorSD=40000; sectorSD<40020;sectorSD++){
	MOV	#1, W1
	MOV	#0, W2
	MOV	#lo_addr(_sectorSD), W0
	ADD	W1, [W0], [W0++]
	ADDC	W2, [W0], [W0--]
;Main_SD.c,379 :: 		}
	GOTO	L_GuardarTramaSD62
L_GuardarTramaSD63:
;Main_SD.c,388 :: 		}
L_end_GuardarTramaSD:
	POP	W12
	POP	W11
	POP	W10
	ULNK
	RETURN
; end of _GuardarTramaSD

_GuardarInfoSector:
	LNK	#512

;Main_SD.c,393 :: 		void GuardarInfoSector(unsigned long datoSector, unsigned long localizacionSector){
;Main_SD.c,398 :: 		bufferSectores[0] = (datoSector>>24)&0xFF;                                     //MSB variable sector
	ADD	W14, #0, W5
	LSR	W11, #8, W2
	CLR	W3
	MOV	#255, W0
	MOV	#0, W1
	AND	W2, W0, W0
	MOV.B	W0, [W5]
;Main_SD.c,399 :: 		bufferSectores[1] = (datoSector>>16)&0xFF;
	ADD	W5, #1, W4
	MOV	W11, W2
	CLR	W3
	MOV	#255, W0
	MOV	#0, W1
	AND	W2, W0, W0
	MOV.B	W0, [W4]
;Main_SD.c,400 :: 		bufferSectores[2] = (datoSector>>8)&0xFF;
	ADD	W5, #2, W4
	LSR	W10, #8, W2
	SL	W11, #8, W3
	IOR	W2, W3, W2
	LSR	W11, #8, W3
	MOV	#255, W0
	MOV	#0, W1
	AND	W2, W0, W0
	MOV.B	W0, [W4]
;Main_SD.c,401 :: 		bufferSectores[3] = (datoSector)&0xFF;                                         //LSD variable sector
	ADD	W5, #3, W2
	MOV	#255, W0
	MOV	#0, W1
	AND	W10, W0, W0
	MOV.B	W0, [W2]
;Main_SD.c,402 :: 		for (x=4;x<512;x++){
	MOV	#4, W0
	MOV	W0, _x
L_GuardarInfoSector88:
	MOV	_x, W1
	MOV	#512, W0
	CP	W1, W0
	BRA LTU	L__GuardarInfoSector135
	GOTO	L_GuardarInfoSector89
L__GuardarInfoSector135:
;Main_SD.c,403 :: 		bufferSectores[x] = 0;                                                 //Rellena de ceros el resto del buffer
	ADD	W14, #0, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], W1
	CLR	W0
	MOV.B	W0, [W1]
;Main_SD.c,402 :: 		for (x=4;x<512;x++){
	MOV	#1, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,404 :: 		}
	GOTO	L_GuardarInfoSector88
L_GuardarInfoSector89:
;Main_SD.c,407 :: 		for (x=0;x<5;x++){
	CLR	W0
	MOV	W0, _x
L_GuardarInfoSector91:
	MOV	_x, W0
	CP	W0, #5
	BRA LTU	L__GuardarInfoSector136
	GOTO	L_GuardarInfoSector92
L__GuardarInfoSector136:
;Main_SD.c,408 :: 		checkEscSD = SD_Write_Block(bufferSectores,localizacionSector);
	ADD	W14, #0, W0
	PUSH.D	W12
	PUSH.D	W10
	MOV	W12, W11
	MOV	W13, W12
	MOV	W0, W10
	CALL	_SD_Write_Block
	POP.D	W10
	POP.D	W12
	MOV	#lo_addr(_checkEscSD), W1
	MOV.B	W0, [W1]
;Main_SD.c,409 :: 		if (checkEscSD == DATA_ACCEPTED){
	CP.B	W0, #22
	BRA Z	L__GuardarInfoSector137
	GOTO	L_GuardarInfoSector94
L__GuardarInfoSector137:
;Main_SD.c,411 :: 		break;
	GOTO	L_GuardarInfoSector92
;Main_SD.c,412 :: 		}
L_GuardarInfoSector94:
;Main_SD.c,413 :: 		Delay_us(10);
	MOV	#80, W7
L_GuardarInfoSector95:
	DEC	W7
	BRA NZ	L_GuardarInfoSector95
	NOP
	NOP
;Main_SD.c,407 :: 		for (x=0;x<5;x++){
	MOV	#1, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], [W0]
;Main_SD.c,414 :: 		}
	GOTO	L_GuardarInfoSector91
L_GuardarInfoSector92:
;Main_SD.c,416 :: 		}
L_end_GuardarInfoSector:
	ULNK
	RETURN
; end of _GuardarInfoSector
