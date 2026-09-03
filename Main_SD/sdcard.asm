
_LEDcard:

;sdcard.c,100 :: 		void LEDcard(int veces, unsigned char tiempo_seg){
;sdcard.c,102 :: 		TRISAbits.TRISA1 = 0; // Configura RA1 como salida
	BCLR.B	TRISAbits, #1
;sdcard.c,103 :: 		if (tiempo_seg==1){
	CP.B	W11, #1
	BRA Z	L__LEDcard105
	GOTO	L_LEDcard0
L__LEDcard105:
;sdcard.c,104 :: 		for (i=0; i<veces; i++){
; i start address is: 0 (W0)
	CLR	W0
; i end address is: 0 (W0)
L_LEDcard1:
; i start address is: 0 (W0)
	CP	W0, W10
	BRA LTU	L__LEDcard106
	GOTO	L_LEDcard2
L__LEDcard106:
;sdcard.c,105 :: 		LATAbits.LATA1 = 1; // Enciende el LED
	BSET.B	LATAbits, #1
;sdcard.c,106 :: 		Delay_ms(1000);   // Espera
	MOV	#123, W8
	MOV	#4681, W7
L_LEDcard4:
	DEC	W7
	BRA NZ	L_LEDcard4
	DEC	W8
	BRA NZ	L_LEDcard4
;sdcard.c,107 :: 		LATAbits.LATA1 = 0; // Apaga el LED
	BCLR.B	LATAbits, #1
;sdcard.c,108 :: 		Delay_ms(1000);   // Espera
	MOV	#123, W8
	MOV	#4681, W7
L_LEDcard6:
	DEC	W7
	BRA NZ	L_LEDcard6
	DEC	W8
	BRA NZ	L_LEDcard6
;sdcard.c,104 :: 		for (i=0; i<veces; i++){
	INC	W0
;sdcard.c,109 :: 		}
; i end address is: 0 (W0)
	GOTO	L_LEDcard1
L_LEDcard2:
;sdcard.c,110 :: 		}else if (tiempo_seg==2){
	GOTO	L_LEDcard8
L_LEDcard0:
	CP.B	W11, #2
	BRA Z	L__LEDcard107
	GOTO	L_LEDcard9
L__LEDcard107:
;sdcard.c,111 :: 		for (i=0; i<veces; i++){
; i start address is: 0 (W0)
	CLR	W0
; i end address is: 0 (W0)
L_LEDcard10:
; i start address is: 0 (W0)
	CP	W0, W10
	BRA LTU	L__LEDcard108
	GOTO	L_LEDcard11
L__LEDcard108:
;sdcard.c,112 :: 		LATAbits.LATA1 = 1; // Enciende el LED
	BSET.B	LATAbits, #1
;sdcard.c,113 :: 		Delay_ms(350);   // Espera
	MOV	#43, W8
	MOV	#47513, W7
L_LEDcard13:
	DEC	W7
	BRA NZ	L_LEDcard13
	DEC	W8
	BRA NZ	L_LEDcard13
;sdcard.c,114 :: 		LATAbits.LATA1 = 0; // Apaga el LED
	BCLR.B	LATAbits, #1
;sdcard.c,115 :: 		Delay_ms(350);   // Espera
	MOV	#43, W8
	MOV	#47513, W7
L_LEDcard15:
	DEC	W7
	BRA NZ	L_LEDcard15
	DEC	W8
	BRA NZ	L_LEDcard15
;sdcard.c,111 :: 		for (i=0; i<veces; i++){
	INC	W0
;sdcard.c,116 :: 		}
; i end address is: 0 (W0)
	GOTO	L_LEDcard10
L_LEDcard11:
;sdcard.c,117 :: 		}
L_LEDcard9:
L_LEDcard8:
;sdcard.c,118 :: 		}
L_end_LEDcard:
	RETURN
; end of _LEDcard

_SD_Read:
	LNK	#2

;sdcard.c,124 :: 		unsigned char SD_Read(unsigned char *Buffer, unsigned int nbytes){
;sdcard.c,127 :: 		for(i = 0; i < SD_TIME_OUT; i++){
; i start address is: 4 (W2)
	CLR	W2
; i end address is: 4 (W2)
L_SD_Read17:
; i start address is: 4 (W2)
	MOV	#2000, W0
	CP	W2, W0
	BRA LTU	L__SD_Read110
	GOTO	L_SD_Read18
L__SD_Read110:
;sdcard.c,128 :: 		temp = SPISD_Write(0xFF);
	PUSH	W2
	PUSH.D	W10
	MOV.B	#255, W10
	CALL	_SPISD_Write
	POP.D	W10
	POP	W2
;sdcard.c,129 :: 		if(temp == 0xFE) break;
	MOV.B	#254, W1
	CP.B	W0, W1
	BRA Z	L__SD_Read111
	GOTO	L_SD_Read20
L__SD_Read111:
; i end address is: 4 (W2)
	GOTO	L_SD_Read18
L_SD_Read20:
;sdcard.c,130 :: 		if(i == SD_TIME_OUT-1) return TOKEN_NOT_RECEIVED;
; i start address is: 4 (W2)
	MOV	#1999, W0
	CP	W2, W0
	BRA Z	L__SD_Read112
	GOTO	L_SD_Read21
L__SD_Read112:
; i end address is: 4 (W2)
	MOV.B	#21, W0
	GOTO	L_end_SD_Read
L_SD_Read21:
;sdcard.c,127 :: 		for(i = 0; i < SD_TIME_OUT; i++){
; i start address is: 4 (W2)
	INC	W2
;sdcard.c,132 :: 		}
; i end address is: 4 (W2)
	GOTO	L_SD_Read17
L_SD_Read18:
;sdcard.c,133 :: 		for(i = 0; i < nbytes; i++){
; i start address is: 4 (W2)
	CLR	W2
; i end address is: 4 (W2)
L_SD_Read22:
; i start address is: 4 (W2)
	CP	W2, W11
	BRA LTU	L__SD_Read113
	GOTO	L_SD_Read23
L__SD_Read113:
;sdcard.c,134 :: 		Buffer[i] = SPISD_Write(0xFF);
	ADD	W10, W2, W0
	MOV	W0, [W14+0]
	PUSH	W2
	PUSH.D	W10
	MOV.B	#255, W10
	CALL	_SPISD_Write
	POP.D	W10
	POP	W2
	MOV	[W14+0], W1
	MOV.B	W0, [W1]
;sdcard.c,133 :: 		for(i = 0; i < nbytes; i++){
	INC	W2
;sdcard.c,135 :: 		}
; i end address is: 4 (W2)
	GOTO	L_SD_Read22
L_SD_Read23:
;sdcard.c,136 :: 		temp = SPISD_Write(0xFF);     // Read 16bits of CRC
	PUSH.D	W10
	MOV.B	#255, W10
	CALL	_SPISD_Write
;sdcard.c,137 :: 		temp = SPISD_Write(0xFF);     //
	MOV.B	#255, W10
	CALL	_SPISD_Write
	POP.D	W10
;sdcard.c,138 :: 		return 0x00;                  // Successful read
	CLR	W0
;sdcard.c,139 :: 		}
L_end_SD_Read:
	ULNK
	RETURN
; end of _SD_Read

_SD_Read_Block:

;sdcard.c,150 :: 		unsigned char SD_Read_Block(unsigned char *Buffer, unsigned long Address){
;sdcard.c,152 :: 		Select_SD();
	PUSH	W11
	PUSH	W12
	PUSH	W13
	CALL	_Select_SD
;sdcard.c,154 :: 		if(ccs == 0x02) Address<<=9;                      // Address * 512 for SDSC cards
	MOV	#lo_addr(_ccs), W0
	MOV.B	[W0], W0
	CP.B	W0, #2
	BRA Z	L__SD_Read_Block115
	GOTO	L_SD_Read_Block25
L__SD_Read_Block115:
	SL	W12, #9, W1
	LSR	W11, #7, W0
	IOR	W0, W1, W1
	SL	W11, #9, W0
	MOV	W0, W11
	MOV	W1, W12
L_SD_Read_Block25:
;sdcard.c,155 :: 		if(SD_Ready() == 0) return SD_NOT_READY;
	PUSH	W11
	PUSH	W12
	PUSH	W10
	CALL	_SD_Ready
	POP	W10
	POP	W12
	POP	W11
	CP.B	W0, #0
	BRA Z	L__SD_Read_Block116
	GOTO	L_SD_Read_Block26
L__SD_Read_Block116:
	MOV.B	#17, W0
	GOTO	L_end_SD_Read_Block
L_SD_Read_Block26:
;sdcard.c,156 :: 		SD_Send_Command(READ_SINGLE_BLOCK,Address,0xFF);
	PUSH	W10
	MOV.B	#255, W13
	MOV.B	#17, W10
	CALL	_SD_Send_Command
;sdcard.c,157 :: 		temp = R1_Response();
	CALL	_R1_Response
	POP	W10
; temp start address is: 4 (W2)
	MOV.B	W0, W2
;sdcard.c,158 :: 		if(temp == 0xFF){LEDcard(10,2);}
	MOV.B	#255, W1
	CP.B	W0, W1
	BRA Z	L__SD_Read_Block117
	GOTO	L_SD_Read_Block27
L__SD_Read_Block117:
	PUSH	W10
	MOV.B	#2, W11
	MOV	#10, W10
	CALL	_LEDcard
	POP	W10
L_SD_Read_Block27:
;sdcard.c,159 :: 		if(temp != 0x00) return temp;
	CP.B	W2, #0
	BRA NZ	L__SD_Read_Block118
	GOTO	L_SD_Read_Block28
L__SD_Read_Block118:
	MOV.B	W2, W0
; temp end address is: 4 (W2)
	GOTO	L_end_SD_Read_Block
L_SD_Read_Block28:
;sdcard.c,160 :: 		temp = SD_Read(Buffer,512);
	MOV	#512, W11
	CALL	_SD_Read
; temp start address is: 2 (W1)
	MOV.B	W0, W1
;sdcard.c,162 :: 		Release_SD();
	CALL	_Release_SD
;sdcard.c,163 :: 		return temp;
	MOV.B	W1, W0
; temp end address is: 2 (W1)
;sdcard.c,164 :: 		}
;sdcard.c,163 :: 		return temp;
;sdcard.c,164 :: 		}
L_end_SD_Read_Block:
	POP	W13
	POP	W12
	POP	W11
	RETURN
; end of _SD_Read_Block

_SD_Write_Block:

;sdcard.c,175 :: 		unsigned char SD_Write_Block(unsigned char *Buffer, unsigned long Address){
;sdcard.c,180 :: 		Select_SD();
	PUSH	W13
	CALL	_Select_SD
;sdcard.c,182 :: 		if(ccs == 0x02) Address<<=9;        // Address * 512 for SDSC cards
	MOV	#lo_addr(_ccs), W0
	MOV.B	[W0], W0
	CP.B	W0, #2
	BRA Z	L__SD_Write_Block120
	GOTO	L_SD_Write_Block29
L__SD_Write_Block120:
	SL	W12, #9, W1
	LSR	W11, #7, W0
	IOR	W0, W1, W1
	SL	W11, #9, W0
	MOV	W0, W11
	MOV	W1, W12
L_SD_Write_Block29:
;sdcard.c,183 :: 		if(SD_Ready() == 0) return SD_NOT_READY;
	PUSH	W11
	PUSH	W12
	PUSH	W10
	CALL	_SD_Ready
	POP	W10
	POP	W12
	POP	W11
	CP.B	W0, #0
	BRA Z	L__SD_Write_Block121
	GOTO	L_SD_Write_Block30
L__SD_Write_Block121:
	MOV.B	#17, W0
	GOTO	L_end_SD_Write_Block
L_SD_Write_Block30:
;sdcard.c,184 :: 		SD_Send_Command(WRITE_BLOCK,Address,0xFF);
	PUSH	W10
	MOV.B	#255, W13
	MOV.B	#24, W10
	CALL	_SD_Send_Command
;sdcard.c,185 :: 		temp = R1_Response();
	CALL	_R1_Response
	POP	W10
; temp start address is: 2 (W1)
	MOV.B	W0, W1
;sdcard.c,186 :: 		if(temp != 0x00) return temp;
	CP.B	W0, #0
	BRA NZ	L__SD_Write_Block122
	GOTO	L_SD_Write_Block31
L__SD_Write_Block122:
	MOV.B	W1, W0
; temp end address is: 2 (W1)
	GOTO	L_end_SD_Write_Block
L_SD_Write_Block31:
;sdcard.c,187 :: 		temp = SPISD_Write(0xFE);    // Send Start Block Token;
	PUSH	W10
	MOV.B	#254, W10
	CALL	_SPISD_Write
	POP	W10
;sdcard.c,188 :: 		for(i = 0; i < 512; i++){
; i start address is: 2 (W1)
	CLR	W1
; i end address is: 2 (W1)
L_SD_Write_Block32:
; i start address is: 2 (W1)
	MOV	#512, W0
	CP	W1, W0
	BRA LTU	L__SD_Write_Block123
	GOTO	L_SD_Write_Block33
L__SD_Write_Block123:
;sdcard.c,189 :: 		temp = SPISD_Write(Buffer[i]);
	ADD	W10, W1, W0
	PUSH	W1
	PUSH	W11
	PUSH	W12
	PUSH	W10
	MOV.B	[W0], W10
	CALL	_SPISD_Write
	POP	W10
	POP	W12
	POP	W11
	POP	W1
;sdcard.c,188 :: 		for(i = 0; i < 512; i++){
	INC	W1
;sdcard.c,190 :: 		}
; i end address is: 2 (W1)
	GOTO	L_SD_Write_Block32
L_SD_Write_Block33:
;sdcard.c,191 :: 		temp = SPISD_Write(0xFF);        // Send dummy 16bits CRC
	PUSH	W11
	PUSH	W12
	PUSH	W10
	MOV.B	#255, W10
	CALL	_SPISD_Write
;sdcard.c,192 :: 		temp = SPISD_Write(0xFF);
	MOV.B	#255, W10
	CALL	_SPISD_Write
;sdcard.c,193 :: 		temp = SPISD_Write(0xFF); // Read Response token (xxx0:status(3b):1)
	MOV.B	#255, W10
	CALL	_SPISD_Write
	POP	W10
	POP	W12
	POP	W11
;sdcard.c,194 :: 		temp = (temp&0x0E)>>1;
	ZE	W0, W0
	AND	W0, #14, W0
	ASR	W0, #1, W0
; temp start address is: 2 (W1)
	MOV.B	W0, W1
;sdcard.c,195 :: 		if(SD_Ready() == 0) return SD_NOT_READY;
	PUSH	W1
	PUSH	W11
	PUSH	W12
	PUSH	W10
	CALL	_SD_Ready
	POP	W10
	POP	W12
	POP	W11
	POP	W1
	CP.B	W0, #0
	BRA Z	L__SD_Write_Block124
	GOTO	L_SD_Write_Block35
L__SD_Write_Block124:
; temp end address is: 2 (W1)
	MOV.B	#17, W0
	GOTO	L_end_SD_Write_Block
L_SD_Write_Block35:
;sdcard.c,198 :: 		Release_SD();
; temp start address is: 2 (W1)
	CALL	_Release_SD
;sdcard.c,199 :: 		if(temp == 0x02) return DATA_ACCEPTED;
	CP.B	W1, #2
	BRA Z	L__SD_Write_Block125
	GOTO	L_SD_Write_Block36
L__SD_Write_Block125:
; temp end address is: 2 (W1)
	MOV.B	#22, W0
	GOTO	L_end_SD_Write_Block
L_SD_Write_Block36:
;sdcard.c,200 :: 		else if(temp == 0x05) return DATA_REJECTED_CRC_ERROR;
; temp start address is: 2 (W1)
	CP.B	W1, #5
	BRA Z	L__SD_Write_Block126
	GOTO	L_SD_Write_Block38
L__SD_Write_Block126:
; temp end address is: 2 (W1)
	MOV.B	#23, W0
	GOTO	L_end_SD_Write_Block
L_SD_Write_Block38:
;sdcard.c,201 :: 		else if(temp == 0x06) return DATA_REJECTED_WR_ERROR;
; temp start address is: 2 (W1)
	CP.B	W1, #6
	BRA Z	L__SD_Write_Block127
	GOTO	L_SD_Write_Block40
L__SD_Write_Block127:
; temp end address is: 2 (W1)
	MOV.B	#24, W0
	GOTO	L_end_SD_Write_Block
L_SD_Write_Block40:
;sdcard.c,202 :: 		else return ERROR;
	MOV.B	#10, W0
;sdcard.c,203 :: 		}
;sdcard.c,202 :: 		else return ERROR;
;sdcard.c,203 :: 		}
L_end_SD_Write_Block:
	POP	W13
	RETURN
; end of _SD_Write_Block

_SD_Init_Try:
	LNK	#2

;sdcard.c,215 :: 		unsigned char SD_Init_Try(unsigned char try_value){
;sdcard.c,217 :: 		if(try_value == 0) try_value = 1;
	CP.B	W10, #0
	BRA Z	L__SD_Init_Try129
	GOTO	L_SD_Init_Try42
L__SD_Init_Try129:
	MOV.B	#1, W10
L_SD_Init_Try42:
;sdcard.c,218 :: 		for(i = 0; i < try_value; i++){
; i start address is: 4 (W2)
	CLR	W2
; i end address is: 4 (W2)
L_SD_Init_Try43:
; i start address is: 4 (W2)
	CP.B	W2, W10
	BRA LTU	L__SD_Init_Try130
	GOTO	L_SD_Init_Try44
L__SD_Init_Try130:
;sdcard.c,219 :: 		init_status = SD_Init();
	PUSH	W2
	PUSH	W10
	CALL	_SD_Init
	POP	W10
	POP	W2
	MOV.B	W0, [W14+0]
;sdcard.c,220 :: 		if(init_status == SUCCESSFUL_INIT) break;
	MOV.B	#170, W1
	CP.B	W0, W1
	BRA Z	L__SD_Init_Try131
	GOTO	L_SD_Init_Try46
L__SD_Init_Try131:
; i end address is: 4 (W2)
	GOTO	L_SD_Init_Try44
L_SD_Init_Try46:
;sdcard.c,221 :: 		Release_SD();
; i start address is: 4 (W2)
	CALL	_Release_SD
;sdcard.c,222 :: 		Delay_ms(10);
	MOV	#2, W8
	MOV	#14464, W7
L_SD_Init_Try47:
	DEC	W7
	BRA NZ	L_SD_Init_Try47
	DEC	W8
	BRA NZ	L_SD_Init_Try47
	NOP
	NOP
;sdcard.c,218 :: 		for(i = 0; i < try_value; i++){
	INC.B	W2
;sdcard.c,223 :: 		}
; i end address is: 4 (W2)
	GOTO	L_SD_Init_Try43
L_SD_Init_Try44:
;sdcard.c,224 :: 		return init_status;
	MOV.B	[W14+0], W0
;sdcard.c,225 :: 		}
L_end_SD_Init_Try:
	ULNK
	RETURN
; end of _SD_Init_Try

_SD_Init:
	LNK	#2

;sdcard.c,236 :: 		unsigned char SD_Init(void){
;sdcard.c,244 :: 		sd_CS_tris = 0;
	PUSH	W10
	PUSH	W11
	PUSH	W12
	PUSH	W13
	BCLR	sd_CS_tris, BitPos(sd_CS_tris+0)
;sdcard.c,247 :: 		Release_SD();
	CALL	_Release_SD
;sdcard.c,250 :: 		SPISD_Init(SLOW);
	CLR	W10
	CALL	_SPISD_Init
;sdcard.c,254 :: 		for(i = 0; i < 80; i++) SPISD_Write(0xFF);
	CLR	W0
	MOV	W0, [W14+0]
L_SD_Init49:
	MOV	#80, W1
	ADD	W14, #0, W0
	CP	W1, [W0]
	BRA GTU	L__SD_Init133
	GOTO	L_SD_Init50
L__SD_Init133:
	MOV.B	#255, W10
	CALL	_SPISD_Write
	MOV	[W14+0], W1
	ADD	W14, #0, W0
	ADD	W1, #1, [W0]
	GOTO	L_SD_Init49
L_SD_Init50:
;sdcard.c,257 :: 		Select_SD(); // Se coloca en 1 el CHIP SELECT CS
	CALL	_Select_SD
;sdcard.c,260 :: 		for(i = 0; i < 16; i++) SPISD_Write(0xFF);
	CLR	W0
	MOV	W0, [W14+0]
L_SD_Init52:
	MOV	[W14+0], W0
	CP	W0, #16
	BRA LTU	L__SD_Init134
	GOTO	L_SD_Init53
L__SD_Init134:
	MOV.B	#255, W10
	CALL	_SPISD_Write
	MOV	[W14+0], W1
	ADD	W14, #0, W0
	ADD	W1, #1, [W0]
	GOTO	L_SD_Init52
L_SD_Init53:
;sdcard.c,272 :: 		for(i = 0; i < SD_TIME_OUT; i++){
	CLR	W0
	MOV	W0, [W14+0]
L_SD_Init55:
	MOV	[W14+0], W1
	MOV	#2000, W0
	CP	W1, W0
	BRA LTU	L__SD_Init135
	GOTO	L_SD_Init56
L__SD_Init135:
;sdcard.c,273 :: 		SD_Send_Command(GO_IDLE_STATE,0x00000000,0x4A);     // CMD0
	MOV.B	#74, W13
	CLR	W11
	CLR	W12
	CLR	W10
	CALL	_SD_Send_Command
;sdcard.c,274 :: 		temp = R1_Response();
	CALL	_R1_Response
;sdcard.c,275 :: 		if(temp == (1<<IDLE_STATE)) {
	ZE	W0, W0
	CP	W0, #1
	BRA Z	L__SD_Init136
	GOTO	L_SD_Init58
L__SD_Init136:
;sdcard.c,276 :: 		break;
	GOTO	L_SD_Init56
;sdcard.c,277 :: 		}
L_SD_Init58:
;sdcard.c,278 :: 		if(i==(SD_TIME_OUT-1)) return CARD_NOT_INSERTED;
	MOV	[W14+0], W1
	MOV	#1999, W0
	CP	W1, W0
	BRA Z	L__SD_Init137
	GOTO	L_SD_Init59
L__SD_Init137:
	MOV.B	#16, W0
	GOTO	L_end_SD_Init
L_SD_Init59:
;sdcard.c,272 :: 		for(i = 0; i < SD_TIME_OUT; i++){
	MOV	[W14+0], W1
	ADD	W14, #0, W0
	ADD	W1, #1, [W0]
;sdcard.c,279 :: 		}
	GOTO	L_SD_Init55
L_SD_Init56:
;sdcard.c,284 :: 		if(SD_Ready() == 0){
	CALL	_SD_Ready
	CP.B	W0, #0
	BRA Z	L__SD_Init138
	GOTO	L_SD_Init60
L__SD_Init138:
;sdcard.c,285 :: 		return SD_NOT_READY;
	MOV.B	#17, W0
	GOTO	L_end_SD_Init
;sdcard.c,286 :: 		}
L_SD_Init60:
;sdcard.c,288 :: 		SD_Send_Command(SEND_IF_COND,0x000001AA,0x43);          // CMD8
	MOV.B	#67, W13
	MOV	#426, W11
	MOV	#0, W12
	MOV.B	#8, W10
	CALL	_SD_Send_Command
;sdcard.c,289 :: 		temp = R1_Response();
	CALL	_R1_Response
; temp start address is: 2 (W1)
	MOV.B	W0, W1
;sdcard.c,291 :: 		if(temp != (1<<IDLE_STATE)){
	ZE	W0, W0
	CP	W0, #1
	BRA NZ	L__SD_Init139
	GOTO	L_SD_Init61
L__SD_Init139:
; temp end address is: 2 (W1)
;sdcard.c,293 :: 		for(i = 0; i < SD_TIME_OUT; i++){
	CLR	W0
	MOV	W0, [W14+0]
L_SD_Init62:
	MOV	[W14+0], W1
	MOV	#2000, W0
	CP	W1, W0
	BRA LTU	L__SD_Init140
	GOTO	L_SD_Init63
L__SD_Init140:
;sdcard.c,294 :: 		if(SD_Ready() == 0) return SD_NOT_READY;
	CALL	_SD_Ready
	CP.B	W0, #0
	BRA Z	L__SD_Init141
	GOTO	L_SD_Init65
L__SD_Init141:
	MOV.B	#17, W0
	GOTO	L_end_SD_Init
L_SD_Init65:
;sdcard.c,295 :: 		SD_Send_Command(SEND_OP_COND,0x00000000,0x7C);  // CMD1
	MOV.B	#124, W13
	CLR	W11
	CLR	W12
	MOV.B	#1, W10
	CALL	_SD_Send_Command
;sdcard.c,296 :: 		temp = R1_Response();
	CALL	_R1_Response
;sdcard.c,297 :: 		if(temp == 0x00) break;
	CP.B	W0, #0
	BRA Z	L__SD_Init142
	GOTO	L_SD_Init66
L__SD_Init142:
	GOTO	L_SD_Init63
L_SD_Init66:
;sdcard.c,298 :: 		if(i==(SD_TIME_OUT-1)) return UNUSABLE_CARD;
	MOV	[W14+0], W1
	MOV	#1999, W0
	CP	W1, W0
	BRA Z	L__SD_Init143
	GOTO	L_SD_Init67
L__SD_Init143:
	MOV.B	#18, W0
	GOTO	L_end_SD_Init
L_SD_Init67:
;sdcard.c,293 :: 		for(i = 0; i < SD_TIME_OUT; i++){
	MOV	[W14+0], W1
	ADD	W14, #0, W0
	ADD	W1, #1, [W0]
;sdcard.c,299 :: 		}
	GOTO	L_SD_Init62
L_SD_Init63:
;sdcard.c,301 :: 		} else if (temp == (1<<IDLE_STATE)){
	GOTO	L_SD_Init68
L_SD_Init61:
; temp start address is: 2 (W1)
	ZE	W1, W0
	CP	W0, #1
	BRA Z	L__SD_Init144
	GOTO	L_SD_Init69
L__SD_Init144:
; temp end address is: 2 (W1)
;sdcard.c,303 :: 		temp_long = Response_32b();
	CALL	_Response_32b
; temp_long start address is: 8 (W4)
	MOV.D	W0, W4
;sdcard.c,304 :: 		temp = (temp_long & ECHO_BACK_MASK);
	MOV	#255, W2
	MOV	#0, W3
	AND	W0, W2, W2
;sdcard.c,305 :: 		if(temp != 0xAA) return ECHO_BACK_ERROR;
	MOV.B	#170, W0
	CP.B	W2, W0
	BRA NZ	L__SD_Init145
	GOTO	L_SD_Init70
L__SD_Init145:
; temp_long end address is: 8 (W4)
	MOV.B	#19, W0
	GOTO	L_end_SD_Init
L_SD_Init70:
;sdcard.c,306 :: 		temp = ((temp_long & VOLTAGE_ACCEPTED_MASK)>>8);
; temp_long start address is: 8 (W4)
	MOV	#3840, W0
	MOV	#0, W1
	AND	W4, W0, W2
	AND	W5, W1, W3
; temp_long end address is: 8 (W4)
	LSR	W2, #8, W0
	SL	W3, #8, W1
	IOR	W0, W1, W0
	LSR	W3, #8, W1
;sdcard.c,307 :: 		if(temp != 0x01) return INCOMPATIBLE_VOLTAGE;
	CP.B	W0, #1
	BRA NZ	L__SD_Init146
	GOTO	L_SD_Init71
L__SD_Init146:
	MOV.B	#20, W0
	GOTO	L_end_SD_Init
L_SD_Init71:
;sdcard.c,310 :: 		if(SD_Ready() == 0) return SD_NOT_READY;
	CALL	_SD_Ready
	CP.B	W0, #0
	BRA Z	L__SD_Init147
	GOTO	L_SD_Init72
L__SD_Init147:
	MOV.B	#17, W0
	GOTO	L_end_SD_Init
L_SD_Init72:
;sdcard.c,311 :: 		SD_Send_Command(READ_OCR,0x00000000,0x7E);          // CMD58
	MOV.B	#126, W13
	CLR	W11
	CLR	W12
	MOV.B	#58, W10
	CALL	_SD_Send_Command
;sdcard.c,312 :: 		temp = R1_Response();                               // Parte de la respuesta R3
	CALL	_R1_Response
; temp start address is: 2 (W1)
	MOV.B	W0, W1
;sdcard.c,313 :: 		if(temp != (1<<IDLE_STATE)) return temp;
	ZE	W0, W0
	CP	W0, #1
	BRA NZ	L__SD_Init148
	GOTO	L_SD_Init73
L__SD_Init148:
	MOV.B	W1, W0
; temp end address is: 2 (W1)
	GOTO	L_end_SD_Init
L_SD_Init73:
;sdcard.c,314 :: 		temp_long = Response_32b();                         // Parte de la repuesta R3
	CALL	_Response_32b
;sdcard.c,315 :: 		if((temp_long & VOLTAGE_RANGE_MASK) != VOLTAGE_RANGE_MASK)
	MOV	#32768, W2
	MOV	#255, W3
	AND	W0, W2, W2
	AND	W1, W3, W3
	MOV	#32768, W0
	MOV	#255, W1
	CP	W2, W0
	CPB	W3, W1
	BRA NZ	L__SD_Init149
	GOTO	L_SD_Init74
L__SD_Init149:
;sdcard.c,316 :: 		return INCOMPATIBLE_VOLTAGE;
	MOV.B	#20, W0
	GOTO	L_end_SD_Init
L_SD_Init74:
;sdcard.c,319 :: 		if(SD_Ready() == 0) return SD_NOT_READY;
	CALL	_SD_Ready
	CP.B	W0, #0
	BRA Z	L__SD_Init150
	GOTO	L_SD_Init75
L__SD_Init150:
	MOV.B	#17, W0
	GOTO	L_end_SD_Init
L_SD_Init75:
;sdcard.c,320 :: 		SD_Send_Command(CRC_ON_OFF,0x00000001,0x48);        // CMD59, CRC final 0x91
	MOV.B	#72, W13
	MOV	#1, W11
	MOV	#0, W12
	MOV.B	#59, W10
	CALL	_SD_Send_Command
;sdcard.c,321 :: 		temp = R1_Response();
	CALL	_R1_Response
; temp start address is: 2 (W1)
	MOV.B	W0, W1
;sdcard.c,322 :: 		if(temp != (1<<IDLE_STATE)) return temp;
	ZE	W0, W0
	CP	W0, #1
	BRA NZ	L__SD_Init151
	GOTO	L_SD_Init76
L__SD_Init151:
	MOV.B	W1, W0
; temp end address is: 2 (W1)
	GOTO	L_end_SD_Init
L_SD_Init76:
;sdcard.c,325 :: 		for(i = 0; i < SD_TIME_OUT; i++){
	CLR	W0
	MOV	W0, [W14+0]
L_SD_Init77:
	MOV	[W14+0], W1
	MOV	#2000, W0
	CP	W1, W0
	BRA LTU	L__SD_Init152
	GOTO	L_SD_Init78
L__SD_Init152:
;sdcard.c,326 :: 		if(SD_Ready() == 0) return SD_NOT_READY;
	CALL	_SD_Ready
	CP.B	W0, #0
	BRA Z	L__SD_Init153
	GOTO	L_SD_Init80
L__SD_Init153:
	MOV.B	#17, W0
	GOTO	L_end_SD_Init
L_SD_Init80:
;sdcard.c,327 :: 		SD_Send_Command(APP_CMD,0x00000000,0x32);           // CMD55
	MOV.B	#50, W13
	CLR	W11
	CLR	W12
	MOV.B	#55, W10
	CALL	_SD_Send_Command
;sdcard.c,328 :: 		temp = R1_Response();
	CALL	_R1_Response
;sdcard.c,329 :: 		if(SD_Ready() == 0) return SD_NOT_READY;
	CALL	_SD_Ready
	CP.B	W0, #0
	BRA Z	L__SD_Init154
	GOTO	L_SD_Init81
L__SD_Init154:
	MOV.B	#17, W0
	GOTO	L_end_SD_Init
L_SD_Init81:
;sdcard.c,332 :: 		for(j = 0; j < 16; j++) SPISD_Write(0xFF);
; j start address is: 0 (W0)
	CLR	W0
; j end address is: 0 (W0)
L_SD_Init82:
; j start address is: 0 (W0)
	CP	W0, #16
	BRA LTU	L__SD_Init155
	GOTO	L_SD_Init83
L__SD_Init155:
	PUSH	W0
	MOV.B	#255, W10
	CALL	_SPISD_Write
	POP	W0
; j start address is: 2 (W1)
	ADD	W0, #1, W1
; j end address is: 0 (W0)
	MOV	W1, W0
; j end address is: 2 (W1)
	GOTO	L_SD_Init82
L_SD_Init83:
;sdcard.c,334 :: 		SD_Send_Command(SD_SEND_OP_COND,0x40000000,0x3B);   // ACMD41
	MOV.B	#59, W13
	MOV	#0, W11
	MOV	#16384, W12
	MOV.B	#41, W10
	CALL	_SD_Send_Command
;sdcard.c,335 :: 		temp = R1_Response();
	CALL	_R1_Response
;sdcard.c,336 :: 		if(temp == 0x00) break;  // Initialization done
	CP.B	W0, #0
	BRA Z	L__SD_Init156
	GOTO	L_SD_Init85
L__SD_Init156:
	GOTO	L_SD_Init78
L_SD_Init85:
;sdcard.c,337 :: 		if(i==(SD_TIME_OUT-1)) return UNUSABLE_CARD;
	MOV	[W14+0], W1
	MOV	#1999, W0
	CP	W1, W0
	BRA Z	L__SD_Init157
	GOTO	L_SD_Init86
L__SD_Init157:
	MOV.B	#18, W0
	GOTO	L_end_SD_Init
L_SD_Init86:
;sdcard.c,325 :: 		for(i = 0; i < SD_TIME_OUT; i++){
	MOV	[W14+0], W1
	ADD	W14, #0, W0
	ADD	W1, #1, [W0]
;sdcard.c,338 :: 		}
	GOTO	L_SD_Init77
L_SD_Init78:
;sdcard.c,339 :: 		}
	GOTO	L_SD_Init87
L_SD_Init69:
;sdcard.c,340 :: 		else return temp;   // Some error of the R1 response type
; temp start address is: 2 (W1)
	MOV.B	W1, W0
; temp end address is: 2 (W1)
	GOTO	L_end_SD_Init
L_SD_Init87:
L_SD_Init68:
;sdcard.c,343 :: 		if(SD_Ready() == 0) return SD_NOT_READY;
	CALL	_SD_Ready
	CP.B	W0, #0
	BRA Z	L__SD_Init158
	GOTO	L_SD_Init88
L__SD_Init158:
	MOV.B	#17, W0
	GOTO	L_end_SD_Init
L_SD_Init88:
;sdcard.c,344 :: 		SD_Send_Command(CRC_ON_OFF,0x00000000,0x48);        // CMD59
	MOV.B	#72, W13
	CLR	W11
	CLR	W12
	MOV.B	#59, W10
	CALL	_SD_Send_Command
;sdcard.c,345 :: 		temp = R1_Response();
	CALL	_R1_Response
; temp start address is: 2 (W1)
	MOV.B	W0, W1
;sdcard.c,346 :: 		if(temp != 0x00) return temp;
	CP.B	W0, #0
	BRA NZ	L__SD_Init159
	GOTO	L_SD_Init89
L__SD_Init159:
	MOV.B	W1, W0
; temp end address is: 2 (W1)
	GOTO	L_end_SD_Init
L_SD_Init89:
;sdcard.c,349 :: 		if(SD_Ready() == 0) return SD_NOT_READY;
	CALL	_SD_Ready
	CP.B	W0, #0
	BRA Z	L__SD_Init160
	GOTO	L_SD_Init90
L__SD_Init160:
	MOV.B	#17, W0
	GOTO	L_end_SD_Init
L_SD_Init90:
;sdcard.c,350 :: 		SD_Send_Command(SET_BLOCKLEN,0x00000200,0x0A);      // CMD16
	MOV.B	#10, W13
	MOV	#512, W11
	MOV	#0, W12
	MOV.B	#16, W10
	CALL	_SD_Send_Command
;sdcard.c,351 :: 		temp = R1_Response();
	CALL	_R1_Response
; temp start address is: 2 (W1)
	MOV.B	W0, W1
;sdcard.c,352 :: 		if(temp != 0x00) return temp;
	CP.B	W0, #0
	BRA NZ	L__SD_Init161
	GOTO	L_SD_Init91
L__SD_Init161:
	MOV.B	W1, W0
; temp end address is: 2 (W1)
	GOTO	L_end_SD_Init
L_SD_Init91:
;sdcard.c,355 :: 		if(SD_Ready() == 0) return SD_NOT_READY;
	CALL	_SD_Ready
	CP.B	W0, #0
	BRA Z	L__SD_Init162
	GOTO	L_SD_Init92
L__SD_Init162:
	MOV.B	#17, W0
	GOTO	L_end_SD_Init
L_SD_Init92:
;sdcard.c,356 :: 		SD_Send_Command(READ_OCR,0x00000000,0x7E);          // CMD58
	MOV.B	#126, W13
	CLR	W11
	CLR	W12
	MOV.B	#58, W10
	CALL	_SD_Send_Command
;sdcard.c,357 :: 		temp = R1_Response();
	CALL	_R1_Response
; temp start address is: 2 (W1)
	MOV.B	W0, W1
;sdcard.c,358 :: 		if(temp != 0x00) return temp;
	CP.B	W0, #0
	BRA NZ	L__SD_Init163
	GOTO	L_SD_Init93
L__SD_Init163:
	MOV.B	W1, W0
; temp end address is: 2 (W1)
	GOTO	L_end_SD_Init
L_SD_Init93:
;sdcard.c,359 :: 		temp_long = Response_32b();
	CALL	_Response_32b
;sdcard.c,360 :: 		ccs = (long)(temp_long >> 30);
	LSR	W1, #14, W2
	CLR	W3
	MOV	#lo_addr(_ccs), W0
	MOV.B	W2, [W0]
;sdcard.c,363 :: 		Release_SD();
	CALL	_Release_SD
;sdcard.c,366 :: 		for(i = 0; i < 16; i++) SPISD_Write(0xFF);
	CLR	W0
	MOV	W0, [W14+0]
L_SD_Init94:
	MOV	[W14+0], W0
	CP	W0, #16
	BRA LTU	L__SD_Init164
	GOTO	L_SD_Init95
L__SD_Init164:
	MOV.B	#255, W10
	CALL	_SPISD_Write
	MOV	[W14+0], W1
	ADD	W14, #0, W0
	ADD	W1, #1, [W0]
	GOTO	L_SD_Init94
L_SD_Init95:
;sdcard.c,369 :: 		SPISD_Init(FAST);
	MOV.B	#1, W10
	CALL	_SPISD_Init
;sdcard.c,371 :: 		return SUCCESSFUL_INIT;
	MOV.B	#170, W0
;sdcard.c,372 :: 		}
;sdcard.c,371 :: 		return SUCCESSFUL_INIT;
;sdcard.c,372 :: 		}
L_end_SD_Init:
	POP	W13
	POP	W12
	POP	W11
	POP	W10
	ULNK
	RETURN
; end of _SD_Init

_R1_Response:

;sdcard.c,381 :: 		unsigned char R1_Response(void){
;sdcard.c,383 :: 		temp = SPISD_Write(0xFF);
	PUSH	W10
	MOV.B	#255, W10
	CALL	_SPISD_Write
;sdcard.c,384 :: 		temp = SPISD_Write(0xFF);
	MOV.B	#255, W10
	CALL	_SPISD_Write
;sdcard.c,385 :: 		return temp;
;sdcard.c,386 :: 		}
;sdcard.c,385 :: 		return temp;
;sdcard.c,386 :: 		}
L_end_R1_Response:
	POP	W10
	RETURN
; end of _R1_Response

_R2_Response:
	LNK	#2

;sdcard.c,395 :: 		unsigned int R2_Response(void){
;sdcard.c,398 :: 		temp = SPISD_Write(0xFF);
	PUSH	W10
	MOV.B	#255, W10
	CALL	_SPISD_Write
;sdcard.c,399 :: 		response = SPISD_Write(0xFF);
	MOV.B	#255, W10
	CALL	_SPISD_Write
	ZE	W0, W0
	MOV	W0, [W14+0]
;sdcard.c,400 :: 		temp = SPISD_Write(0xFF);
	MOV.B	#255, W10
	CALL	_SPISD_Write
;sdcard.c,401 :: 		response = (response<<8)|temp;
	MOV	[W14+0], W1
	SL	W1, #8, W1
	ZE	W0, W0
	IOR	W1, W0, W0
;sdcard.c,402 :: 		return response;
;sdcard.c,403 :: 		}
;sdcard.c,402 :: 		return response;
;sdcard.c,403 :: 		}
L_end_R2_Response:
	POP	W10
	ULNK
	RETURN
; end of _R2_Response

_Response_32b:
	LNK	#4

;sdcard.c,412 :: 		unsigned long Response_32b(void){
;sdcard.c,415 :: 		response = SPISD_Write(0xFF);
	PUSH	W10
	MOV.B	#255, W10
	CALL	_SPISD_Write
	ZE	W0, W0
	CLR	W1
	MOV	W0, [W14+0]
	MOV	W1, [W14+2]
;sdcard.c,416 :: 		temp = SPISD_Write(0xFF);
	MOV.B	#255, W10
	CALL	_SPISD_Write
;sdcard.c,417 :: 		response = (response<<8)|temp;
	MOV	[W14+0], W1
	MOV	[W14+2], W2
	SL	W2, #8, W4
	LSR	W1, #8, W3
	IOR	W3, W4, W4
	SL	W1, #8, W3
	ZE	W0, W1
	CLR	W2
	ADD	W14, #0, W0
	IOR	W3, W1, [W0++]
	IOR	W4, W2, [W0--]
;sdcard.c,418 :: 		temp = SPISD_Write(0xFF);
	MOV.B	#255, W10
	CALL	_SPISD_Write
;sdcard.c,419 :: 		response = (response<<8)|temp;
	MOV	[W14+0], W1
	MOV	[W14+2], W2
	SL	W2, #8, W4
	LSR	W1, #8, W3
	IOR	W3, W4, W4
	SL	W1, #8, W3
	ZE	W0, W1
	CLR	W2
	ADD	W14, #0, W0
	IOR	W3, W1, [W0++]
	IOR	W4, W2, [W0--]
;sdcard.c,420 :: 		temp = SPISD_Write(0xFF);
	MOV.B	#255, W10
	CALL	_SPISD_Write
;sdcard.c,421 :: 		response = (response<<8)|temp;
	MOV	[W14+0], W4
	MOV	[W14+2], W5
	SL	W5, #8, W3
	LSR	W4, #8, W2
	IOR	W2, W3, W3
	SL	W4, #8, W2
	ZE	W0, W0
	CLR	W1
	IOR	W2, W0, W0
	IOR	W3, W1, W1
;sdcard.c,422 :: 		return response;
;sdcard.c,423 :: 		}
;sdcard.c,422 :: 		return response;
;sdcard.c,423 :: 		}
L_end_Response_32b:
	POP	W10
	ULNK
	RETURN
; end of _Response_32b

_SD_Send_Command:

;sdcard.c,432 :: 		void SD_Send_Command(unsigned char command,unsigned long argument, unsigned char crc){
;sdcard.c,435 :: 		SPISD_Write(command |= 0x40);
	PUSH	W10
	ZE	W10, W1
	MOV	#64, W0
	IOR	W1, W0, W0
	MOV.B	W0, W10
	PUSH	W13
	PUSH	W11
	PUSH	W12
	MOV.B	W0, W10
	CALL	_SPISD_Write
	POP	W12
	POP	W11
;sdcard.c,438 :: 		SPISD_Write((unsigned char)(argument>>24));
	LSR	W12, #8, W0
	CLR	W1
	PUSH	W11
	PUSH	W12
	MOV.B	W0, W10
	CALL	_SPISD_Write
	POP	W12
	POP	W11
;sdcard.c,439 :: 		SPISD_Write((unsigned char)(argument>>16));
	MOV	W12, W0
	CLR	W1
	PUSH	W11
	PUSH	W12
	MOV.B	W0, W10
	CALL	_SPISD_Write
	POP	W12
	POP	W11
;sdcard.c,440 :: 		SPISD_Write((unsigned char)(argument>>8));
	LSR	W11, #8, W0
	SL	W12, #8, W1
	IOR	W0, W1, W0
	LSR	W12, #8, W1
	PUSH	W11
	PUSH	W12
	MOV.B	W0, W10
	CALL	_SPISD_Write
	POP	W12
	POP	W11
;sdcard.c,441 :: 		SPISD_Write((unsigned char)(argument));
	MOV.B	W11, W10
	CALL	_SPISD_Write
	POP	W13
;sdcard.c,444 :: 		SPISD_Write((crc<<1)|0x01);
	ZE	W13, W0
	SL	W0, #1, W0
	IOR	W0, #1, W0
	MOV.B	W0, W10
	CALL	_SPISD_Write
;sdcard.c,445 :: 		}
L_end_SD_Send_Command:
	POP	W10
	RETURN
; end of _SD_Send_Command

_SD_Ready:
	LNK	#2

;sdcard.c,456 :: 		unsigned char SD_Ready(void){
;sdcard.c,459 :: 		for(i = 0; i < SD_TIME_OUT; i++){
	PUSH	W10
; i start address is: 4 (W2)
	CLR	W2
; i end address is: 4 (W2)
L_SD_Ready97:
; i start address is: 4 (W2)
	MOV	#2000, W0
	CP	W2, W0
	BRA LTU	L__SD_Ready170
	GOTO	L_SD_Ready98
L__SD_Ready170:
;sdcard.c,460 :: 		temp = SPISD_Write(0xFF);
	PUSH	W2
	MOV.B	#255, W10
	CALL	_SPISD_Write
	POP	W2
	MOV.B	W0, [W14+0]
;sdcard.c,461 :: 		if(temp == 0xFF) break;
	MOV.B	#255, W1
	CP.B	W0, W1
	BRA Z	L__SD_Ready171
	GOTO	L_SD_Ready100
L__SD_Ready171:
; i end address is: 4 (W2)
	GOTO	L_SD_Ready98
L_SD_Ready100:
;sdcard.c,462 :: 		if(i == (SD_TIME_OUT-1)) return 0x00;
; i start address is: 4 (W2)
	MOV	#1999, W0
	CP	W2, W0
	BRA Z	L__SD_Ready172
	GOTO	L_SD_Ready101
L__SD_Ready172:
; i end address is: 4 (W2)
	CLR	W0
	GOTO	L_end_SD_Ready
L_SD_Ready101:
;sdcard.c,459 :: 		for(i = 0; i < SD_TIME_OUT; i++){
; i start address is: 4 (W2)
	INC	W2
;sdcard.c,463 :: 		}
; i end address is: 4 (W2)
	GOTO	L_SD_Ready97
L_SD_Ready98:
;sdcard.c,464 :: 		return temp;
	MOV.B	[W14+0], W0
;sdcard.c,465 :: 		}
;sdcard.c,464 :: 		return temp;
;sdcard.c,465 :: 		}
L_end_SD_Ready:
	POP	W10
	ULNK
	RETURN
; end of _SD_Ready

_Release_SD:

;sdcard.c,476 :: 		void Release_SD(void){
;sdcard.c,478 :: 		sd_CS_lat = 1;
	BSET	sd_CS_lat, BitPos(sd_CS_lat+0)
;sdcard.c,479 :: 		asm nop;  // Genera un retraso para asegurar el cambio dew estado
	NOP
;sdcard.c,480 :: 		}
L_end_Release_SD:
	RETURN
; end of _Release_SD

_Select_SD:

;sdcard.c,491 :: 		void Select_SD(void){
;sdcard.c,493 :: 		sd_CS_lat = 0;
	BCLR	sd_CS_lat, BitPos(sd_CS_lat+0)
;sdcard.c,494 :: 		asm nop;
	NOP
;sdcard.c,495 :: 		}
L_end_Select_SD:
	RETURN
; end of _Select_SD

_SD_Detect:

;sdcard.c,507 :: 		unsigned char SD_Detect(void) {
;sdcard.c,509 :: 		if (sd_detect_port == 0) {
	BTSC	sd_detect_port, BitPos(sd_detect_port+0)
	GOTO	L_SD_Detect102
;sdcard.c,510 :: 		return DETECTED;
	MOV.B	#222, W0
	GOTO	L_end_SD_Detect
;sdcard.c,512 :: 		} else {
L_SD_Detect102:
;sdcard.c,513 :: 		return 0;
	CLR	W0
;sdcard.c,515 :: 		}
L_end_SD_Detect:
	RETURN
; end of _SD_Detect
