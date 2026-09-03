
nodo2_RS485_EsperarFinTx:

;rs485.c,37 :: 		static void RS485_EsperarFinTx(unsigned short puerto){
;rs485.c,39 :: 		if (puerto==RS485_PUERTO_UART1){
	CP.B	W10, #1
	BRA Z	L_nodo2_RS485_EsperarFinTx61
	GOTO	L_nodo2_RS485_EsperarFinTx0
L_nodo2_RS485_EsperarFinTx61:
;rs485.c,40 :: 		while(!U1STAbits.TRMT);
L_nodo2_RS485_EsperarFinTx1:
	BTSC	U1STAbits, #8
	GOTO	L_nodo2_RS485_EsperarFinTx2
	GOTO	L_nodo2_RS485_EsperarFinTx1
L_nodo2_RS485_EsperarFinTx2:
;rs485.c,41 :: 		} else {
	GOTO	L_nodo2_RS485_EsperarFinTx3
L_nodo2_RS485_EsperarFinTx0:
;rs485.c,42 :: 		while(!U2STAbits.TRMT);
L_nodo2_RS485_EsperarFinTx4:
	BTSC	U2STAbits, #8
	GOTO	L_nodo2_RS485_EsperarFinTx5
	GOTO	L_nodo2_RS485_EsperarFinTx4
L_nodo2_RS485_EsperarFinTx5:
;rs485.c,43 :: 		}
L_nodo2_RS485_EsperarFinTx3:
;rs485.c,45 :: 		}
L_end_RS485_EsperarFinTx:
	RETURN
; end of nodo2_RS485_EsperarFinTx

nodo2_RS485_EscribirByte:

;rs485.c,51 :: 		static void RS485_EscribirByte(unsigned short puerto, unsigned char dato){
;rs485.c,53 :: 		if (puerto==RS485_PUERTO_UART1){
	PUSH	W10
	CP.B	W10, #1
	BRA Z	L_nodo2_RS485_EscribirByte63
	GOTO	L_nodo2_RS485_EscribirByte6
L_nodo2_RS485_EscribirByte63:
;rs485.c,54 :: 		UART1_Write(dato);
	ZE	W11, W10
	CALL	_UART1_Write
;rs485.c,55 :: 		} else {
	GOTO	L_nodo2_RS485_EscribirByte7
L_nodo2_RS485_EscribirByte6:
;rs485.c,56 :: 		UART2_Write(dato);
	ZE	W11, W10
	CALL	_UART2_Write
;rs485.c,57 :: 		}
L_nodo2_RS485_EscribirByte7:
;rs485.c,59 :: 		}
L_end_RS485_EscribirByte:
	POP	W10
	RETURN
; end of nodo2_RS485_EscribirByte

_EnviarTramaRS485:
	LNK	#0

;rs485.c,74 :: 		void EnviarTramaRS485(unsigned short puerto, unsigned short direccion, unsigned short funcion, unsigned int numDatos, unsigned char *payload){
	PUSH	W11
; payload start address is: 2 (W1)
	MOV	[W14-8], W1
;rs485.c,79 :: 		ptrNumDatos = (unsigned char *) &numDatos;
	MOV	#lo_addr(W13), W0
; ptrNumDatos start address is: 4 (W2)
	MOV	W0, W2
;rs485.c,81 :: 		MSRS485 = 1;                                    // Habilita el MAX485 de datos en modo transmision (DE/RE = 1)
	BSET	LATB12_bit, BitPos(LATB12_bit+0)
;rs485.c,82 :: 		Delay_us(10);                                    // Tiempo de establecimiento del transceptor (margen sobre datasheet MAX485)
	MOV	#80, W7
L_EnviarTramaRS4858:
	DEC	W7
	BRA NZ	L_EnviarTramaRS4858
	NOP
	NOP
;rs485.c,84 :: 		RS485_EscribirByte(puerto, RS485_BYTE_INICIO);   // [0] Cabecera
	PUSH	W11
	MOV.B	#58, W11
	CALL	nodo2_RS485_EscribirByte
	POP	W11
;rs485.c,85 :: 		RS485_EscribirByte(puerto, direccion);           // [1] Direccion del nodo destino
	CALL	nodo2_RS485_EscribirByte
;rs485.c,86 :: 		RS485_EscribirByte(puerto, funcion);             // [2] Funcion solicitada
	MOV.B	W12, W11
	CALL	nodo2_RS485_EscribirByte
;rs485.c,87 :: 		RS485_EscribirByte(puerto, *(ptrNumDatos));      // [3] numDatos LSB
	MOV.B	[W2], W11
	CALL	nodo2_RS485_EscribirByte
;rs485.c,88 :: 		RS485_EscribirByte(puerto, *(ptrNumDatos+1));    // [4] numDatos MSB
	ADD	W2, #1, W0
; ptrNumDatos end address is: 4 (W2)
	MOV.B	[W0], W11
	CALL	nodo2_RS485_EscribirByte
;rs485.c,90 :: 		for (k=0; k<numDatos; k++){
; k start address is: 4 (W2)
	CLR	W2
; k end address is: 4 (W2)
L_EnviarTramaRS48510:
; k start address is: 4 (W2)
; payload start address is: 2 (W1)
; payload end address is: 2 (W1)
	CP	W2, W13
	BRA LTU	L__EnviarTramaRS48565
	GOTO	L_EnviarTramaRS48511
L__EnviarTramaRS48565:
; payload end address is: 2 (W1)
;rs485.c,91 :: 		RS485_EscribirByte(puerto, payload[k]);      // [5..N] Payload
; payload start address is: 2 (W1)
	ADD	W1, W2, W0
	PUSH	W11
	MOV.B	[W0], W11
	CALL	nodo2_RS485_EscribirByte
	POP	W11
;rs485.c,90 :: 		for (k=0; k<numDatos; k++){
	INC	W2
;rs485.c,92 :: 		}
; payload end address is: 2 (W1)
; k end address is: 4 (W2)
	GOTO	L_EnviarTramaRS48510
L_EnviarTramaRS48511:
;rs485.c,94 :: 		RS485_EsperarFinTx(puerto);                      // Espera a que el ultimo byte salga fisicamente por la linea
	CALL	nodo2_RS485_EsperarFinTx
;rs485.c,95 :: 		Delay_us(10);
	MOV	#80, W7
L_EnviarTramaRS48513:
	DEC	W7
	BRA NZ	L_EnviarTramaRS48513
	NOP
	NOP
;rs485.c,96 :: 		MSRS485 = 0;                                     // Regresa el MAX485 a modo recepcion para escuchar la respuesta del nodo
	BCLR	LATB12_bit, BitPos(LATB12_bit+0)
;rs485.c,98 :: 		}
L_end_EnviarTramaRS485:
	POP	W11
	ULNK
	RETURN
; end of _EnviarTramaRS485

_EnviarTramaPruebaRS485:

;rs485.c,107 :: 		void EnviarTramaPruebaRS485(unsigned short direccion){
;rs485.c,109 :: 		EnviarTramaRS485(RS485_PUERTO_UART2, direccion, 0xF3, 10, tramaPruebaRS485);
	PUSH	W10
	PUSH	W11
	PUSH	W12
	PUSH	W13
	MOV	#10, W13
	MOV.B	#243, W12
	MOV.B	W10, W11
	MOV.B	#2, W10
	MOV	#lo_addr(_tramaPruebaRS485), W0
	PUSH	W0
	CALL	_EnviarTramaRS485
	SUB	#2, W15
;rs485.c,111 :: 		}
L_end_EnviarTramaPruebaRS485:
	POP	W13
	POP	W12
	POP	W11
	POP	W10
	RETURN
; end of _EnviarTramaPruebaRS485

_main:
	MOV	#2048, W15
	MOV	#6142, W0
	MOV	WREG, 32
	MOV	#1, W0
	MOV	WREG, 50
	MOV	#4, W0
	IOR	68

;nodo2.c,65 :: 		void main() {
;nodo2.c,67 :: 		ConfiguracionPrincipal();
	CALL	_ConfiguracionPrincipal
;nodo2.c,68 :: 		TEST1 = 0;
	BCLR	LATA2_bit, BitPos(LATA2_bit+0)
;nodo2.c,70 :: 		i = 0; j = 0; x = 0;
	CLR	W0
	MOV	W0, _i
	CLR	W0
	MOV	W0, _j
	CLR	W0
	MOV	W0, _x
;nodo2.c,72 :: 		banRSI = 0; banRSC = 0;
	MOV	#lo_addr(_banRSI), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banRSC), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,73 :: 		byteRS485 = 0; i_rs485 = 0;
	MOV	#lo_addr(_byteRS485), W1
	CLR	W0
	MOV.B	W0, [W1]
	CLR	W0
	MOV	W0, _i_rs485
;nodo2.c,74 :: 		funcionRS485 = 0; subFuncionRS485 = 0;
	MOV	#lo_addr(_funcionRS485), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_subFuncionRS485), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,75 :: 		numDatosRS485 = 0;
	CLR	W0
	MOV	W0, _numDatosRS485
;nodo2.c,76 :: 		ptrnumDatosRS485 = (unsigned char *) &numDatosRS485;
	MOV	#lo_addr(_numDatosRS485), W0
	MOV	W0, _ptrnumDatosRS485
;nodo2.c,77 :: 		contTMR2 = 0;
	MOV	#lo_addr(_contTMR2), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,79 :: 		MSRS485 = 0;                                                               //MAX485 de datos en modo lectura (escucha)
	BCLR	LATB12_bit, BitPos(LATB12_bit+0)
;nodo2.c,81 :: 		while(1){
L_main15:
;nodo2.c,82 :: 		asm CLRWDT;
	CLRWDT
;nodo2.c,83 :: 		Delay_ms(100);
	MOV	#13, W8
	MOV	#13575, W7
L_main17:
	DEC	W7
	BRA NZ	L_main17
	DEC	W8
	BRA NZ	L_main17
;nodo2.c,84 :: 		}
	GOTO	L_main15
;nodo2.c,86 :: 		}
L_end_main:
L__main_end_loop:
	BRA	L__main_end_loop
; end of _main

_ConfiguracionPrincipal:

;nodo2.c,92 :: 		void ConfiguracionPrincipal(){
;nodo2.c,95 :: 		CLKDIVbits.FRCDIV = 0;
	PUSH	W10
	PUSH	W11
	PUSH	W12
	PUSH	W13
	MOV	CLKDIVbits, W1
	MOV	#63743, W0
	AND	W1, W0, W0
	MOV	WREG, CLKDIVbits
;nodo2.c,96 :: 		CLKDIVbits.PLLPOST = 0;
	MOV	#lo_addr(CLKDIVbits), W0
	MOV.B	[W0], W1
	MOV.B	#63, W0
	AND.B	W1, W0, W1
	MOV	#lo_addr(CLKDIVbits), W0
	MOV.B	W1, [W0]
;nodo2.c,97 :: 		CLKDIVbits.PLLPRE = 5;
	MOV.B	#5, W0
	MOV.B	W0, W1
	MOV	#lo_addr(CLKDIVbits), W0
	XOR.B	W1, [W0], W1
	AND.B	W1, #31, W1
	MOV	#lo_addr(CLKDIVbits), W0
	XOR.B	W1, [W0], W1
	MOV	#lo_addr(CLKDIVbits), W0
	MOV.B	W1, [W0]
;nodo2.c,98 :: 		PLLFBDbits.PLLDIV = 150;
	MOV	#150, W0
	MOV	W0, W1
	MOV	#lo_addr(PLLFBDbits), W0
	XOR	W1, [W0], W1
	MOV	#511, W0
	AND	W1, W0, W1
	MOV	#lo_addr(PLLFBDbits), W0
	XOR	W1, [W0], W1
	MOV	W1, PLLFBDbits
;nodo2.c,101 :: 		ANSELA = 0;
	CLR	ANSELA
;nodo2.c,102 :: 		ANSELB = 0;
	CLR	ANSELB
;nodo2.c,103 :: 		TEST1_Direction = 0;
	BCLR	TRISA2_bit, BitPos(TRISA2_bit+0)
;nodo2.c,104 :: 		MSRS485_Direction = 0;
	BCLR	TRISB12_bit, BitPos(TRISB12_bit+0)
;nodo2.c,105 :: 		TRISB14_bit = 1;                                                           //RB14/RPI46 = INT_SINC, entrada desde el MAX485 receptor
	BSET	TRISB14_bit, BitPos(TRISB14_bit+0)
;nodo2.c,107 :: 		INTCON2.GIE = 1;                                                           //Habilita interrupciones globales
	BSET	INTCON2, #15
;nodo2.c,110 :: 		RPINR18bits.U1RXR = 0x2F;                                                  //Rx1 en RB15/RPI47   [VERIFICAR]
	MOV.B	#47, W0
	MOV.B	W0, W1
	MOV	#lo_addr(RPINR18bits), W0
	XOR.B	W1, [W0], W1
	MOV.B	#127, W0
	AND.B	W1, W0, W1
	MOV	#lo_addr(RPINR18bits), W0
	XOR.B	W1, [W0], W1
	MOV	#lo_addr(RPINR18bits), W0
	MOV.B	W1, [W0]
;nodo2.c,111 :: 		RPOR1bits.RP36R = 0x01;                                                    //Tx1 en RB4/RP36     [VERIFICAR]
	MOV.B	#1, W0
	MOV.B	W0, W1
	MOV	#lo_addr(RPOR1bits), W0
	XOR.B	W1, [W0], W1
	MOV.B	#63, W0
	AND.B	W1, W0, W1
	MOV	#lo_addr(RPOR1bits), W0
	XOR.B	W1, [W0], W1
	MOV	#lo_addr(RPOR1bits), W0
	MOV.B	W1, [W0]
;nodo2.c,112 :: 		U1RXIE_bit = 1;
	BSET	U1RXIE_bit, BitPos(U1RXIE_bit+0)
;nodo2.c,113 :: 		U1STAbits.URXISEL = 0x00;
	MOV	#lo_addr(U1STAbits), W0
	MOV.B	[W0], W1
	MOV.B	#63, W0
	AND.B	W1, W0, W1
	MOV	#lo_addr(U1STAbits), W0
	MOV.B	W1, [W0]
;nodo2.c,114 :: 		U1RXIF_bit = 0;
	BCLR	U1RXIF_bit, BitPos(U1RXIF_bit+0)
;nodo2.c,115 :: 		IPC2bits.U1RXIP = 0x04;
	MOV	#16384, W0
	MOV	W0, W1
	MOV	#lo_addr(IPC2bits), W0
	XOR	W1, [W0], W1
	MOV	#28672, W0
	AND	W1, W0, W1
	MOV	#lo_addr(IPC2bits), W0
	XOR	W1, [W0], W1
	MOV	W1, IPC2bits
;nodo2.c,116 :: 		UART1_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED);
	CLR	W13
	CLR	W12
	MOV	#33920, W10
	MOV	#30, W11
	MOV	#1, W0
	PUSH	W0
	CALL	_UART1_Init_Advanced
	SUB	#2, W15
;nodo2.c,119 :: 		RPINR0 = 0x2E00;                                                           //INT1 <- RB14/RPI46 (INT_SINC)   [VERIFICAR]
	MOV	#11776, W0
	MOV	WREG, RPINR0
;nodo2.c,120 :: 		INT1IE_bit = 1;
	BSET	INT1IE_bit, BitPos(INT1IE_bit+0)
;nodo2.c,121 :: 		INT1IF_bit = 0;
	BCLR	INT1IF_bit, BitPos(INT1IF_bit+0)
;nodo2.c,122 :: 		IPC5bits.INT1IP = 0x01;
	MOV.B	#1, W0
	MOV.B	W0, W1
	MOV	#lo_addr(IPC5bits), W0
	XOR.B	W1, [W0], W1
	AND.B	W1, #7, W1
	MOV	#lo_addr(IPC5bits), W0
	XOR.B	W1, [W0], W1
	MOV	#lo_addr(IPC5bits), W0
	MOV.B	W1, [W0]
;nodo2.c,125 :: 		T2CON = 0x0030;
	MOV	#48, W0
	MOV	WREG, T2CON
;nodo2.c,126 :: 		T2CON.TON = 0;
	BCLR	T2CON, #15
;nodo2.c,127 :: 		T2IE_bit = 1;
	BSET	T2IE_bit, BitPos(T2IE_bit+0)
;nodo2.c,128 :: 		T2IF_bit = 0;
	BCLR	T2IF_bit, BitPos(T2IF_bit+0)
;nodo2.c,129 :: 		PR2 = 46875;                                                               //300ms
	MOV	#46875, W0
	MOV	WREG, PR2
;nodo2.c,130 :: 		IPC1bits.T2IP = 0x02;
	MOV	#8192, W0
	MOV	W0, W1
	MOV	#lo_addr(IPC1bits), W0
	XOR	W1, [W0], W1
	MOV	#28672, W0
	AND	W1, W0, W1
	MOV	#lo_addr(IPC1bits), W0
	XOR	W1, [W0], W1
	MOV	W1, IPC1bits
;nodo2.c,132 :: 		Delay_ms(200);
	MOV	#25, W8
	MOV	#27150, W7
L_ConfiguracionPrincipal19:
	DEC	W7
	BRA NZ	L_ConfiguracionPrincipal19
	DEC	W8
	BRA NZ	L_ConfiguracionPrincipal19
	NOP
;nodo2.c,134 :: 		}
L_end_ConfiguracionPrincipal:
	POP	W13
	POP	W12
	POP	W11
	POP	W10
	RETURN
; end of _ConfiguracionPrincipal

_int_1:
	PUSH	DSWPAG
	PUSH	50
	PUSH	RCOUNT
	PUSH	W0
	MOV	#2, W0
	REPEAT	#12
	PUSH	[W0++]

;nodo2.c,146 :: 		void int_1() org IVT_ADDR_INT1INTERRUPT {
;nodo2.c,148 :: 		INT1IF_bit = 0;
	BCLR	INT1IF_bit, BitPos(INT1IF_bit+0)
;nodo2.c,150 :: 		TEST1 = ~TEST1;                                                           //D3: heartbeat de sincronismo recibido
	BTG	LATA2_bit, BitPos(LATA2_bit+0)
;nodo2.c,152 :: 		}
L_end_int_1:
	MOV	#26, W0
	REPEAT	#12
	POP	[W0--]
	POP	W0
	POP	RCOUNT
	POP	50
	POP	DSWPAG
	RETFIE
; end of _int_1

_Timer2Int:
	PUSH	DSWPAG
	PUSH	50
	PUSH	RCOUNT
	PUSH	W0
	MOV	#2, W0
	REPEAT	#12
	PUSH	[W0++]

;nodo2.c,157 :: 		void Timer2Int() org IVT_ADDR_T2INTERRUPT{
;nodo2.c,159 :: 		T2IF_bit = 0;
	PUSH	W10
	PUSH	W11
	PUSH	W12
	PUSH	W13
	BCLR	T2IF_bit, BitPos(T2IF_bit+0)
;nodo2.c,160 :: 		contTMR2++;
	MOV.B	#1, W1
	MOV	#lo_addr(_contTMR2), W0
	ADD.B	W1, [W0], [W0]
;nodo2.c,162 :: 		if (contTMR2==4){
	MOV	#lo_addr(_contTMR2), W0
	MOV.B	[W0], W0
	CP.B	W0, #4
	BRA Z	L__Timer2Int72
	GOTO	L_Timer2Int21
L__Timer2Int72:
;nodo2.c,163 :: 		T2CON.TON = 0;
	BCLR	T2CON, #15
;nodo2.c,164 :: 		TMR2 = 0;
	CLR	TMR2
;nodo2.c,165 :: 		contTMR2 = 0;
	MOV	#lo_addr(_contTMR2), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,166 :: 		banRSI = 0;
	MOV	#lo_addr(_banRSI), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,167 :: 		banRSC = 0;
	MOV	#lo_addr(_banRSC), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,168 :: 		i_rs485 = 0;
	CLR	W0
	MOV	W0, _i_rs485
;nodo2.c,169 :: 		UART1_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED);
	CLR	W13
	CLR	W12
	MOV	#33920, W10
	MOV	#30, W11
	MOV	#1, W0
	PUSH	W0
	CALL	_UART1_Init_Advanced
	SUB	#2, W15
;nodo2.c,170 :: 		}
L_Timer2Int21:
;nodo2.c,172 :: 		}
L_end_Timer2Int:
	POP	W13
	POP	W12
	POP	W11
	POP	W10
	MOV	#26, W0
	REPEAT	#12
	POP	[W0--]
	POP	W0
	POP	RCOUNT
	POP	50
	POP	DSWPAG
	RETFIE
; end of _Timer2Int

_urx_1:
	PUSH	DSWPAG
	PUSH	50
	PUSH	RCOUNT
	PUSH	W0
	MOV	#2, W0
	REPEAT	#12
	PUSH	[W0++]

;nodo2.c,178 :: 		void urx_1() org  IVT_ADDR_U1RXINTERRUPT {
;nodo2.c,180 :: 		U1RXIF_bit = 0;
	PUSH	W10
	PUSH	W11
	PUSH	W12
	PUSH	W13
	BCLR	U1RXIF_bit, BitPos(U1RXIF_bit+0)
;nodo2.c,181 :: 		byteRS485 = U1RXREG;
	MOV	#lo_addr(_byteRS485), W1
	MOV.B	U1RXREG, WREG
	MOV.B	W0, [W1]
;nodo2.c,182 :: 		OERR_bit = 0;
	BCLR	OERR_bit, BitPos(OERR_bit+0)
;nodo2.c,184 :: 		if (banRSI==2){
	MOV	#lo_addr(_banRSI), W0
	MOV.B	[W0], W0
	CP.B	W0, #2
	BRA Z	L__urx_174
	GOTO	L_urx_122
L__urx_174:
;nodo2.c,185 :: 		if (i_rs485<(numDatosRS485)){
	MOV	_i_rs485, W1
	MOV	#lo_addr(_numDatosRS485), W0
	CP	W1, [W0]
	BRA LTU	L__urx_175
	GOTO	L_urx_123
L__urx_175:
;nodo2.c,186 :: 		inputPyloadRS485[i_rs485] = byteRS485;
	MOV	#lo_addr(_inputPyloadRS485), W1
	MOV	#lo_addr(_i_rs485), W0
	ADD	W1, [W0], W1
	MOV	#lo_addr(_byteRS485), W0
	MOV.B	[W0], [W1]
;nodo2.c,187 :: 		i_rs485++;
	MOV	#1, W1
	MOV	#lo_addr(_i_rs485), W0
	ADD	W1, [W0], [W0]
;nodo2.c,188 :: 		} else {
	GOTO	L_urx_124
L_urx_123:
;nodo2.c,189 :: 		banRSI = 0;
	MOV	#lo_addr(_banRSI), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,190 :: 		banRSC = 1;
	MOV	#lo_addr(_banRSC), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
;nodo2.c,191 :: 		}
L_urx_124:
;nodo2.c,192 :: 		}
L_urx_122:
;nodo2.c,194 :: 		if ((banRSI==0)&&(banRSC==0)){
	MOV	#lo_addr(_banRSI), W0
	MOV.B	[W0], W0
	CP.B	W0, #0
	BRA Z	L__urx_176
	GOTO	L__urx_151
L__urx_176:
	MOV	#lo_addr(_banRSC), W0
	MOV.B	[W0], W0
	CP.B	W0, #0
	BRA Z	L__urx_177
	GOTO	L__urx_150
L__urx_177:
L__urx_149:
;nodo2.c,195 :: 		if (byteRS485==RS485_BYTE_INICIO){
	MOV	#lo_addr(_byteRS485), W0
	MOV.B	[W0], W1
	MOV.B	#58, W0
	CP.B	W1, W0
	BRA Z	L__urx_178
	GOTO	L_urx_128
L__urx_178:
;nodo2.c,196 :: 		banRSI = 1;
	MOV	#lo_addr(_banRSI), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
;nodo2.c,197 :: 		i_rs485 = 0;
	CLR	W0
	MOV	W0, _i_rs485
;nodo2.c,198 :: 		}
L_urx_128:
;nodo2.c,194 :: 		if ((banRSI==0)&&(banRSC==0)){
L__urx_151:
L__urx_150:
;nodo2.c,200 :: 		if ((banRSI==1)&&(i_rs485<5)){
	MOV	#lo_addr(_banRSI), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__urx_179
	GOTO	L__urx_153
L__urx_179:
	MOV	_i_rs485, W0
	CP	W0, #5
	BRA LTU	L__urx_180
	GOTO	L__urx_152
L__urx_180:
L__urx_148:
;nodo2.c,201 :: 		tramaCabeceraRS485[i_rs485] = byteRS485;
	MOV	#lo_addr(_tramaCabeceraRS485), W1
	MOV	#lo_addr(_i_rs485), W0
	ADD	W1, [W0], W1
	MOV	#lo_addr(_byteRS485), W0
	MOV.B	[W0], [W1]
;nodo2.c,202 :: 		i_rs485++;
	MOV	#1, W1
	MOV	#lo_addr(_i_rs485), W0
	ADD	W1, [W0], [W0]
;nodo2.c,200 :: 		if ((banRSI==1)&&(i_rs485<5)){
L__urx_153:
L__urx_152:
;nodo2.c,204 :: 		if ((banRSI==1)&&(i_rs485==5)){
	MOV	#lo_addr(_banRSI), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__urx_181
	GOTO	L__urx_157
L__urx_181:
	MOV	_i_rs485, W0
	CP	W0, #5
	BRA Z	L__urx_182
	GOTO	L__urx_156
L__urx_182:
L__urx_147:
;nodo2.c,205 :: 		if ((tramaCabeceraRS485[1]==IDNODO)||(tramaCabeceraRS485[1]==RS485_DIR_BROADCAST)){
	MOV	#lo_addr(_tramaCabeceraRS485+1), W0
	MOV.B	[W0], W0
	CP.B	W0, #2
	BRA NZ	L__urx_183
	GOTO	L__urx_155
L__urx_183:
	MOV	#lo_addr(_tramaCabeceraRS485+1), W0
	MOV.B	[W0], W1
	MOV.B	#255, W0
	CP.B	W1, W0
	BRA NZ	L__urx_184
	GOTO	L__urx_154
L__urx_184:
	GOTO	L_urx_137
L__urx_155:
L__urx_154:
;nodo2.c,206 :: 		funcionRS485 = tramaCabeceraRS485[2];
	MOV	#lo_addr(_funcionRS485), W1
	MOV	#lo_addr(_tramaCabeceraRS485+2), W0
	MOV.B	[W0], [W1]
;nodo2.c,207 :: 		*(ptrnumDatosRS485) = tramaCabeceraRS485[3];
	MOV	#lo_addr(_tramaCabeceraRS485+3), W1
	MOV	_ptrnumDatosRS485, W0
	MOV.B	[W1], [W0]
;nodo2.c,208 :: 		*(ptrnumDatosRS485+1) = tramaCabeceraRS485[4];
	MOV	_ptrnumDatosRS485, W0
	ADD	W0, #1, W1
	MOV	#lo_addr(_tramaCabeceraRS485+4), W0
	MOV.B	[W0], [W1]
;nodo2.c,209 :: 		banRSI = 2;
	MOV	#lo_addr(_banRSI), W1
	MOV.B	#2, W0
	MOV.B	W0, [W1]
;nodo2.c,210 :: 		i_rs485 = 0;
	CLR	W0
	MOV	W0, _i_rs485
;nodo2.c,211 :: 		} else {
	GOTO	L_urx_138
L_urx_137:
;nodo2.c,213 :: 		banRSI = 0;
	MOV	#lo_addr(_banRSI), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,214 :: 		banRSC = 0;
	MOV	#lo_addr(_banRSC), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,215 :: 		i_rs485 = 0;
	CLR	W0
	MOV	W0, _i_rs485
;nodo2.c,216 :: 		T2CON.TON = 1;
	BSET	T2CON, #15
;nodo2.c,217 :: 		TMR2 = 0;
	CLR	TMR2
;nodo2.c,218 :: 		contTMR2 = 0;
	MOV	#lo_addr(_contTMR2), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,219 :: 		U1MODE.UARTEN = 0;
	BCLR	U1MODE, #15
;nodo2.c,220 :: 		}
L_urx_138:
;nodo2.c,204 :: 		if ((banRSI==1)&&(i_rs485==5)){
L__urx_157:
L__urx_156:
;nodo2.c,223 :: 		if (banRSC==1){
	MOV	#lo_addr(_banRSC), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__urx_185
	GOTO	L_urx_139
L__urx_185:
;nodo2.c,224 :: 		subFuncionRS485 = inputPyloadRS485[0];
	MOV	#lo_addr(_subFuncionRS485), W1
	MOV	#lo_addr(_inputPyloadRS485), W0
	MOV.B	[W0], [W1]
;nodo2.c,227 :: 		if ((funcionRS485==0xF1)&&(subFuncionRS485==0xD2)){
	MOV	#lo_addr(_funcionRS485), W0
	MOV.B	[W0], W1
	MOV.B	#241, W0
	CP.B	W1, W0
	BRA Z	L__urx_186
	GOTO	L__urx_159
L__urx_186:
	MOV	#lo_addr(_subFuncionRS485), W0
	MOV.B	[W0], W1
	MOV.B	#210, W0
	CP.B	W1, W0
	BRA Z	L__urx_187
	GOTO	L__urx_158
L__urx_187:
L__urx_145:
;nodo2.c,228 :: 		outputPyloadRS485[0] = 0xD2;
	MOV	#lo_addr(_outputPyloadRS485), W1
	MOV.B	#210, W0
	MOV.B	W0, [W1]
;nodo2.c,229 :: 		outputPyloadRS485[1] = IDNODO;
	MOV	#lo_addr(_outputPyloadRS485+1), W1
	MOV.B	#2, W0
	MOV.B	W0, [W1]
;nodo2.c,230 :: 		delay_ms(10);
	MOV	#2, W8
	MOV	#14464, W7
L_urx_143:
	DEC	W7
	BRA NZ	L_urx_143
	DEC	W8
	BRA NZ	L_urx_143
	NOP
	NOP
;nodo2.c,231 :: 		EnviarTramaRS485(RS485_PUERTO_UART1, IDNODO, 0xF1, 2, outputPyloadRS485);
	MOV	#2, W13
	MOV.B	#241, W12
	MOV.B	#2, W11
	MOV.B	#1, W10
	MOV	#lo_addr(_outputPyloadRS485), W0
	PUSH	W0
	CALL	_EnviarTramaRS485
	SUB	#2, W15
;nodo2.c,227 :: 		if ((funcionRS485==0xF1)&&(subFuncionRS485==0xD2)){
L__urx_159:
L__urx_158:
;nodo2.c,234 :: 		banRSC = 0;
	MOV	#lo_addr(_banRSC), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,235 :: 		banRSI = 0;
	MOV	#lo_addr(_banRSI), W1
	CLR	W0
	MOV.B	W0, [W1]
;nodo2.c,236 :: 		}
L_urx_139:
;nodo2.c,238 :: 		}
L_end_urx_1:
	POP	W13
	POP	W12
	POP	W11
	POP	W10
	MOV	#26, W0
	REPEAT	#12
	POP	[W0--]
	POP	W0
	POP	RCOUNT
	POP	50
	POP	DSWPAG
	RETFIE
; end of _urx_1
