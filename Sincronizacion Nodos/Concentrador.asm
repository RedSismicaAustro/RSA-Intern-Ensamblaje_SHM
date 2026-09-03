
Concentrador_RS485_EsperarFinTx:

;rs485.c,37 :: 		static void RS485_EsperarFinTx(unsigned short puerto){
;rs485.c,39 :: 		if (puerto==RS485_PUERTO_UART1){
	CP.B	W10, #1
	BRA Z	L_Concentrador_RS485_EsperarFinTx199
	GOTO	L_Concentrador_RS485_EsperarFinTx0
L_Concentrador_RS485_EsperarFinTx199:
;rs485.c,40 :: 		while(!U1STAbits.TRMT);
L_Concentrador_RS485_EsperarFinTx1:
	BTSC	U1STAbits, #8
	GOTO	L_Concentrador_RS485_EsperarFinTx2
	GOTO	L_Concentrador_RS485_EsperarFinTx1
L_Concentrador_RS485_EsperarFinTx2:
;rs485.c,41 :: 		} else {
	GOTO	L_Concentrador_RS485_EsperarFinTx3
L_Concentrador_RS485_EsperarFinTx0:
;rs485.c,42 :: 		while(!U2STAbits.TRMT);
L_Concentrador_RS485_EsperarFinTx4:
	BTSC	U2STAbits, #8
	GOTO	L_Concentrador_RS485_EsperarFinTx5
	GOTO	L_Concentrador_RS485_EsperarFinTx4
L_Concentrador_RS485_EsperarFinTx5:
;rs485.c,43 :: 		}
L_Concentrador_RS485_EsperarFinTx3:
;rs485.c,45 :: 		}
L_end_RS485_EsperarFinTx:
	RETURN
; end of Concentrador_RS485_EsperarFinTx

Concentrador_RS485_EscribirByte:

;rs485.c,51 :: 		static void RS485_EscribirByte(unsigned short puerto, unsigned char dato){
;rs485.c,53 :: 		if (puerto==RS485_PUERTO_UART1){
	PUSH	W10
	CP.B	W10, #1
	BRA Z	L_Concentrador_RS485_EscribirByte201
	GOTO	L_Concentrador_RS485_EscribirByte6
L_Concentrador_RS485_EscribirByte201:
;rs485.c,54 :: 		UART1_Write(dato);
	ZE	W11, W10
	CALL	_UART1_Write
;rs485.c,55 :: 		} else {
	GOTO	L_Concentrador_RS485_EscribirByte7
L_Concentrador_RS485_EscribirByte6:
;rs485.c,56 :: 		UART2_Write(dato);
	ZE	W11, W10
	CALL	_UART2_Write
;rs485.c,57 :: 		}
L_Concentrador_RS485_EscribirByte7:
;rs485.c,59 :: 		}
L_end_RS485_EscribirByte:
	POP	W10
	RETURN
; end of Concentrador_RS485_EscribirByte

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
	BSET	LATB11_bit, BitPos(LATB11_bit+0)
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
	CALL	Concentrador_RS485_EscribirByte
	POP	W11
;rs485.c,85 :: 		RS485_EscribirByte(puerto, direccion);           // [1] Direccion del nodo destino
	CALL	Concentrador_RS485_EscribirByte
;rs485.c,86 :: 		RS485_EscribirByte(puerto, funcion);             // [2] Funcion solicitada
	MOV.B	W12, W11
	CALL	Concentrador_RS485_EscribirByte
;rs485.c,87 :: 		RS485_EscribirByte(puerto, *(ptrNumDatos));      // [3] numDatos LSB
	MOV.B	[W2], W11
	CALL	Concentrador_RS485_EscribirByte
;rs485.c,88 :: 		RS485_EscribirByte(puerto, *(ptrNumDatos+1));    // [4] numDatos MSB
	ADD	W2, #1, W0
; ptrNumDatos end address is: 4 (W2)
	MOV.B	[W0], W11
	CALL	Concentrador_RS485_EscribirByte
;rs485.c,90 :: 		for (k=0; k<numDatos; k++){
; k start address is: 4 (W2)
	CLR	W2
; k end address is: 4 (W2)
L_EnviarTramaRS48510:
; k start address is: 4 (W2)
; payload start address is: 2 (W1)
; payload end address is: 2 (W1)
	CP	W2, W13
	BRA LTU	L__EnviarTramaRS485203
	GOTO	L_EnviarTramaRS48511
L__EnviarTramaRS485203:
; payload end address is: 2 (W1)
;rs485.c,91 :: 		RS485_EscribirByte(puerto, payload[k]);      // [5..N] Payload
; payload start address is: 2 (W1)
	ADD	W1, W2, W0
	PUSH	W11
	MOV.B	[W0], W11
	CALL	Concentrador_RS485_EscribirByte
	POP	W11
;rs485.c,90 :: 		for (k=0; k<numDatos; k++){
	INC	W2
;rs485.c,92 :: 		}
; payload end address is: 2 (W1)
; k end address is: 4 (W2)
	GOTO	L_EnviarTramaRS48510
L_EnviarTramaRS48511:
;rs485.c,94 :: 		RS485_EsperarFinTx(puerto);                      // Espera a que el ultimo byte salga fisicamente por la linea
	CALL	Concentrador_RS485_EsperarFinTx
;rs485.c,95 :: 		Delay_us(10);
	MOV	#80, W7
L_EnviarTramaRS48513:
	DEC	W7
	BRA NZ	L_EnviarTramaRS48513
	NOP
	NOP
;rs485.c,96 :: 		MSRS485 = 0;                                     // Regresa el MAX485 a modo recepcion para escuchar la respuesta del nodo
	BCLR	LATB11_bit, BitPos(LATB11_bit+0)
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

;Concentrador.c,97 :: 		void main() {
;Concentrador.c,99 :: 		ConfiguracionPrincipal();
	CALL	_ConfiguracionPrincipal
;Concentrador.c,102 :: 		i = 0; j = 0; x = 0;
	CLR	W0
	MOV	W0, _i
	CLR	W0
	MOV	W0, _j
	CLR	W0
	MOV	W0, _x
;Concentrador.c,105 :: 		banSPI0 = 0; banSPI1 = 0; banSPI2 = 0; banSPI7 = 0; banSPI8 = 0; banSPIA = 0;
	MOV	#lo_addr(_banSPI0), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI1), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI2), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI7), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI8), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPIA), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,106 :: 		banRespuestaPi = 0;
	MOV	#lo_addr(_banRespuestaPi), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,109 :: 		banRSI = 0;
	MOV	#lo_addr(_banRSI), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,110 :: 		banRSC = 0;
	MOV	#lo_addr(_banRSC), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,111 :: 		byteRS485 = 0;
	MOV	#lo_addr(_byteRS485), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,112 :: 		i_rs485 = 0;
	CLR	W0
	MOV	W0, _i_rs485
;Concentrador.c,113 :: 		funcionRS485 = 0;
	MOV	#lo_addr(_funcionRS485), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,114 :: 		subFuncionRS485 = 0;
	MOV	#lo_addr(_subFuncionRS485), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,115 :: 		numDatosRS485 = 0;
	CLR	W0
	MOV	W0, _numDatosRS485
;Concentrador.c,116 :: 		ptrnumDatosRS485 = (unsigned char *) &numDatosRS485;
	MOV	#lo_addr(_numDatosRS485), W0
	MOV	W0, _ptrnumDatosRS485
;Concentrador.c,119 :: 		banInicioMuestreo = 0;
	MOV	#lo_addr(_banInicioMuestreo), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,122 :: 		RP1 = 0;
	BCLR	LATA4_bit, BitPos(LATA4_bit+0)
;Concentrador.c,123 :: 		INT_SINC = 1;                                                              //D5: enciende el testigo de sincronismo (opcional)
	BSET	LATA1_bit, BitPos(LATA1_bit+0)
;Concentrador.c,124 :: 		INT_SINC_1 = 0;                                                            //Linea de sync en reposo
	BCLR	LATA0_bit, BitPos(LATA0_bit+0)
;Concentrador.c,125 :: 		INT_SINC_2 = 0;                                                            //D2: indicador de trafico RS485, apagado al inicio
	BCLR	LATA3_bit, BitPos(LATA3_bit+0)
;Concentrador.c,127 :: 		MSRS485 = 0;                                                               //MAX485 #1 en modo lectura
	BCLR	LATB11_bit, BitPos(LATB11_bit+0)
;Concentrador.c,129 :: 		SPI1BUF = 0x00;
	CLR	SPI1BUF
;Concentrador.c,131 :: 		T3CON.TON = 1;                                                             //Arranca el generador de pulso de sincronizacion
	BSET	T3CON, #15
;Concentrador.c,133 :: 		while(1){
L_main15:
;Concentrador.c,134 :: 		asm CLRWDT;
	CLRWDT
;Concentrador.c,135 :: 		Delay_ms(100);
	MOV	#13, W8
	MOV	#13575, W7
L_main17:
	DEC	W7
	BRA NZ	L_main17
	DEC	W8
	BRA NZ	L_main17
;Concentrador.c,136 :: 		}
	GOTO	L_main15
;Concentrador.c,138 :: 		}
L_end_main:
L__main_end_loop:
	BRA	L__main_end_loop
; end of _main

_ConfiguracionPrincipal:

;Concentrador.c,146 :: 		void ConfiguracionPrincipal(){
;Concentrador.c,149 :: 		CLKDIVbits.FRCDIV = 0;
	PUSH	W10
	PUSH	W11
	PUSH	W12
	PUSH	W13
	MOV	CLKDIVbits, W1
	MOV	#63743, W0
	AND	W1, W0, W0
	MOV	WREG, CLKDIVbits
;Concentrador.c,150 :: 		CLKDIVbits.PLLPOST = 0;
	MOV	#lo_addr(CLKDIVbits), W0
	MOV.B	[W0], W1
	MOV.B	#63, W0
	AND.B	W1, W0, W1
	MOV	#lo_addr(CLKDIVbits), W0
	MOV.B	W1, [W0]
;Concentrador.c,151 :: 		CLKDIVbits.PLLPRE = 5;
	MOV.B	#5, W0
	MOV.B	W0, W1
	MOV	#lo_addr(CLKDIVbits), W0
	XOR.B	W1, [W0], W1
	AND.B	W1, #31, W1
	MOV	#lo_addr(CLKDIVbits), W0
	XOR.B	W1, [W0], W1
	MOV	#lo_addr(CLKDIVbits), W0
	MOV.B	W1, [W0]
;Concentrador.c,152 :: 		PLLFBDbits.PLLDIV = 150;
	MOV	#150, W0
	MOV	W0, W1
	MOV	#lo_addr(PLLFBDbits), W0
	XOR	W1, [W0], W1
	MOV	#511, W0
	AND	W1, W0, W1
	MOV	#lo_addr(PLLFBDbits), W0
	XOR	W1, [W0], W1
	MOV	W1, PLLFBDbits
;Concentrador.c,155 :: 		ANSELA = 0;                                                                //PORTA digital
	CLR	ANSELA
;Concentrador.c,156 :: 		ANSELB = 0;                                                                //PORTB digital
	CLR	ANSELB
;Concentrador.c,158 :: 		INT_SINC_Direction   = 0;                                                  //D5
	BCLR	TRISA1_bit, BitPos(TRISA1_bit+0)
;Concentrador.c,159 :: 		INT_SINC_1_Direction = 0;                                                  //Salida hacia el MAX485 #2 (pulso de sync)
	BCLR	TRISA0_bit, BitPos(TRISA0_bit+0)
;Concentrador.c,160 :: 		INT_SINC_2_Direction = 0;                                                  //D2 (requiere OSC2 Pin Function = I/O en Config Bits)
	BCLR	TRISA3_bit, BitPos(TRISA3_bit+0)
;Concentrador.c,161 :: 		RP1_Direction = 0;                                                         //RP1 (interrupcion hacia la RPi)
	BCLR	TRISA4_bit, BitPos(TRISA4_bit+0)
;Concentrador.c,162 :: 		MSRS485_Direction = 0;                                                     //Control MAX485 #1
	BCLR	TRISB11_bit, BitPos(TRISB11_bit+0)
;Concentrador.c,164 :: 		INTCON2.GIE = 1;                                                           //Habilita las interrupciones globales
	BSET	INTCON2, #15
;Concentrador.c,167 :: 		RPINR19bits.U2RXR = 0x2F;                                                  //Rx2 en RB15/RPI47
	MOV.B	#47, W0
	MOV.B	W0, W1
	MOV	#lo_addr(RPINR19bits), W0
	XOR.B	W1, [W0], W1
	MOV.B	#127, W0
	AND.B	W1, W0, W1
	MOV	#lo_addr(RPINR19bits), W0
	XOR.B	W1, [W0], W1
	MOV	#lo_addr(RPINR19bits), W0
	MOV.B	W1, [W0]
;Concentrador.c,168 :: 		RPOR1bits.RP36R = 0x03;                                                    //Tx2 en RB4/RP36
	MOV.B	#3, W0
	MOV.B	W0, W1
	MOV	#lo_addr(RPOR1bits), W0
	XOR.B	W1, [W0], W1
	MOV.B	#63, W0
	AND.B	W1, W0, W1
	MOV	#lo_addr(RPOR1bits), W0
	XOR.B	W1, [W0], W1
	MOV	#lo_addr(RPOR1bits), W0
	MOV.B	W1, [W0]
;Concentrador.c,169 :: 		U2RXIE_bit = 1;                                                            //Habilita interrupcion UART2 RX
	BSET	U2RXIE_bit, BitPos(U2RXIE_bit+0)
;Concentrador.c,170 :: 		IPC7bits.U2RXIP = 0x04;
	MOV	#1024, W0
	MOV	W0, W1
	MOV	#lo_addr(IPC7bits), W0
	XOR	W1, [W0], W1
	MOV	#1792, W0
	AND	W1, W0, W1
	MOV	#lo_addr(IPC7bits), W0
	XOR	W1, [W0], W1
	MOV	W1, IPC7bits
;Concentrador.c,171 :: 		U2STAbits.URXISEL = 0x00;
	MOV	#lo_addr(U2STAbits), W0
	MOV.B	[W0], W1
	MOV.B	#63, W0
	AND.B	W1, W0, W1
	MOV	#lo_addr(U2STAbits), W0
	MOV.B	W1, [W0]
;Concentrador.c,172 :: 		UART2_Init_Advanced(2000000, _UART_8BIT_NOPARITY, _UART_ONE_STOPBIT, _UART_HI_SPEED);
	CLR	W13
	CLR	W12
	MOV	#33920, W10
	MOV	#30, W11
	MOV	#1, W0
	PUSH	W0
	CALL	_UART2_Init_Advanced
	SUB	#2, W15
;Concentrador.c,175 :: 		SPI1STAT.SPIEN = 1;
	BSET	SPI1STAT, #15
;Concentrador.c,176 :: 		SPI1_Init_Advanced(_SPI_SLAVE, _SPI_8_BIT, _SPI_PRESCALE_SEC_1, _SPI_PRESCALE_PRI_1, _SPI_SS_ENABLE, _SPI_DATA_SAMPLE_END, _SPI_CLK_IDLE_HIGH, _SPI_ACTIVE_2_IDLE);
	MOV	#3, W13
	MOV	#28, W12
	CLR	W11
	CLR	W10
	CLR	W0
	PUSH	W0
	MOV	#64, W0
	PUSH	W0
	MOV	#512, W0
	PUSH	W0
	MOV	#128, W0
	PUSH	W0
	CALL	_SPI1_Init_Advanced
	SUB	#8, W15
;Concentrador.c,177 :: 		SPI1IF_bit = 0;
	BCLR	SPI1IF_bit, BitPos(SPI1IF_bit+0)
;Concentrador.c,178 :: 		IPC2bits.SPI1IP = 0x03;
	MOV	#768, W0
	MOV	W0, W1
	MOV	#lo_addr(IPC2bits), W0
	XOR	W1, [W0], W1
	MOV	#1792, W0
	AND	W1, W0, W1
	MOV	#lo_addr(IPC2bits), W0
	XOR	W1, [W0], W1
	MOV	W1, IPC2bits
;Concentrador.c,181 :: 		T2CON = 0x30;
	MOV	#48, W0
	MOV	WREG, T2CON
;Concentrador.c,182 :: 		T2CON.TON = 0;
	BCLR	T2CON, #15
;Concentrador.c,183 :: 		T2IE_bit = 1;
	BSET	T2IE_bit, BitPos(T2IE_bit+0)
;Concentrador.c,184 :: 		T2IF_bit = 0;
	BCLR	T2IF_bit, BitPos(T2IF_bit+0)
;Concentrador.c,185 :: 		PR2 = 46875;
	MOV	#46875, W0
	MOV	WREG, PR2
;Concentrador.c,186 :: 		IPC1bits.T2IP = 0x02;
	MOV	#8192, W0
	MOV	W0, W1
	MOV	#lo_addr(IPC1bits), W0
	XOR	W1, [W0], W1
	MOV	#28672, W0
	AND	W1, W0, W1
	MOV	#lo_addr(IPC1bits), W0
	XOR	W1, [W0], W1
	MOV	W1, IPC1bits
;Concentrador.c,189 :: 		T3CON = 0x30;                                                              //Prescalador 1:256
	MOV	#48, W0
	MOV	WREG, T3CON
;Concentrador.c,190 :: 		T3CON.TON = 0;                                                             //Se enciende en main() una vez lista la configuracion
	BCLR	T3CON, #15
;Concentrador.c,191 :: 		T3IE_bit = 1;
	BSET	T3IE_bit, BitPos(T3IE_bit+0)
;Concentrador.c,192 :: 		T3IF_bit = 0;
	BCLR	T3IF_bit, BitPos(T3IF_bit+0)
;Concentrador.c,193 :: 		PR3 = 15625;                                                               //Preload equivalente a 100ms base
	MOV	#15625, W0
	MOV	WREG, PR3
;Concentrador.c,194 :: 		IPC2bits.T3IP = 0x02;
	MOV.B	#2, W0
	MOV.B	W0, W1
	MOV	#lo_addr(IPC2bits), W0
	XOR.B	W1, [W0], W1
	AND.B	W1, #7, W1
	MOV	#lo_addr(IPC2bits), W0
	XOR.B	W1, [W0], W1
	MOV	#lo_addr(IPC2bits), W0
	MOV.B	W1, [W0]
;Concentrador.c,197 :: 		SPI1IE_bit = 1;
	BSET	SPI1IE_bit, BitPos(SPI1IE_bit+0)
;Concentrador.c,199 :: 		Delay_ms(200);                                                             //Espera a que se estabilicen los cambios
	MOV	#25, W8
	MOV	#27150, W7
L_ConfiguracionPrincipal19:
	DEC	W7
	BRA NZ	L_ConfiguracionPrincipal19
	DEC	W8
	BRA NZ	L_ConfiguracionPrincipal19
	NOP
;Concentrador.c,201 :: 		}
L_end_ConfiguracionPrincipal:
	POP	W13
	POP	W12
	POP	W11
	POP	W10
	RETURN
; end of _ConfiguracionPrincipal

_InterrupcionP1:

;Concentrador.c,206 :: 		void InterrupcionP1(unsigned short funcionSPI, unsigned short subFuncionSPI, unsigned int numBytesSPI){
;Concentrador.c,208 :: 		if (banRespuestaPi==1){
	MOV	#lo_addr(_banRespuestaPi), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__InterrupcionP1209
	GOTO	L_InterrupcionP121
L__InterrupcionP1209:
;Concentrador.c,209 :: 		ptrnumBytesSPI = (unsigned char *) &numBytesSPI;
	MOV	#lo_addr(W12), W1
	MOV	W1, _ptrnumBytesSPI
;Concentrador.c,210 :: 		tramaSolicitudSPI[0] = funcionSPI;
	MOV	#lo_addr(_tramaSolicitudSPI), W0
	MOV.B	W10, [W0]
;Concentrador.c,211 :: 		tramaSolicitudSPI[1] = subFuncionSPI;
	MOV	#lo_addr(_tramaSolicitudSPI+1), W0
	MOV.B	W11, [W0]
;Concentrador.c,212 :: 		tramaSolicitudSPI[2] = *(ptrnumBytesSPI);
	MOV	#lo_addr(_tramaSolicitudSPI+2), W0
	MOV.B	W12, [W0]
;Concentrador.c,213 :: 		tramaSolicitudSPI[3] = *(ptrnumBytesSPI+1);
	ADD	W1, #1, W0
	MOV.B	[W0], W1
	MOV	#lo_addr(_tramaSolicitudSPI+3), W0
	MOV.B	W1, [W0]
;Concentrador.c,214 :: 		RP1 = 1;
	BSET	LATA4_bit, BitPos(LATA4_bit+0)
;Concentrador.c,215 :: 		Delay_us(20);
	MOV	#160, W7
L_InterrupcionP122:
	DEC	W7
	BRA NZ	L_InterrupcionP122
	NOP
	NOP
;Concentrador.c,216 :: 		RP1 = 0;
	BCLR	LATA4_bit, BitPos(LATA4_bit+0)
;Concentrador.c,217 :: 		banRespuestaPi = 0;
	MOV	#lo_addr(_banRespuestaPi), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,218 :: 		}
L_InterrupcionP121:
;Concentrador.c,219 :: 		}
L_end_InterrupcionP1:
	RETURN
; end of _InterrupcionP1

_CambiarEstadoBandera:

;Concentrador.c,224 :: 		void CambiarEstadoBandera(unsigned short bandera, unsigned short estado){
;Concentrador.c,226 :: 		if (estado==1){
	CP.B	W11, #1
	BRA Z	L__CambiarEstadoBandera211
	GOTO	L_CambiarEstadoBandera24
L__CambiarEstadoBandera211:
;Concentrador.c,227 :: 		banSPI0 = 3; banSPI1 = 3; banSPI2 = 3; banSPI7 = 3; banSPI8 = 3; banSPIA = 3;
	MOV	#lo_addr(_banSPI0), W1
	MOV.B	#3, W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI1), W1
	MOV.B	#3, W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI2), W1
	MOV.B	#3, W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI7), W1
	MOV.B	#3, W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI8), W1
	MOV.B	#3, W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPIA), W1
	MOV.B	#3, W0
	MOV.B	W0, [W1]
;Concentrador.c,228 :: 		switch (bandera){
	GOTO	L_CambiarEstadoBandera25
;Concentrador.c,229 :: 		case 0: banSPI0 = 1; break;
L_CambiarEstadoBandera27:
	MOV	#lo_addr(_banSPI0), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
	GOTO	L_CambiarEstadoBandera26
;Concentrador.c,230 :: 		case 1: banSPI1 = 1; break;
L_CambiarEstadoBandera28:
	MOV	#lo_addr(_banSPI1), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
	GOTO	L_CambiarEstadoBandera26
;Concentrador.c,231 :: 		case 2: banSPI2 = 1; break;
L_CambiarEstadoBandera29:
	MOV	#lo_addr(_banSPI2), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
	GOTO	L_CambiarEstadoBandera26
;Concentrador.c,232 :: 		case 7: banSPI7 = 1; break;
L_CambiarEstadoBandera30:
	MOV	#lo_addr(_banSPI7), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
	GOTO	L_CambiarEstadoBandera26
;Concentrador.c,233 :: 		case 8: banSPI8 = 1; break;
L_CambiarEstadoBandera31:
	MOV	#lo_addr(_banSPI8), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
	GOTO	L_CambiarEstadoBandera26
;Concentrador.c,234 :: 		case 0x0A: banSPIA = 1; break;
L_CambiarEstadoBandera32:
	MOV	#lo_addr(_banSPIA), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
	GOTO	L_CambiarEstadoBandera26
;Concentrador.c,235 :: 		}
L_CambiarEstadoBandera25:
	CP.B	W10, #0
	BRA NZ	L__CambiarEstadoBandera212
	GOTO	L_CambiarEstadoBandera27
L__CambiarEstadoBandera212:
	CP.B	W10, #1
	BRA NZ	L__CambiarEstadoBandera213
	GOTO	L_CambiarEstadoBandera28
L__CambiarEstadoBandera213:
	CP.B	W10, #2
	BRA NZ	L__CambiarEstadoBandera214
	GOTO	L_CambiarEstadoBandera29
L__CambiarEstadoBandera214:
	CP.B	W10, #7
	BRA NZ	L__CambiarEstadoBandera215
	GOTO	L_CambiarEstadoBandera30
L__CambiarEstadoBandera215:
	CP.B	W10, #8
	BRA NZ	L__CambiarEstadoBandera216
	GOTO	L_CambiarEstadoBandera31
L__CambiarEstadoBandera216:
	CP.B	W10, #10
	BRA NZ	L__CambiarEstadoBandera217
	GOTO	L_CambiarEstadoBandera32
L__CambiarEstadoBandera217:
L_CambiarEstadoBandera26:
;Concentrador.c,236 :: 		}
L_CambiarEstadoBandera24:
;Concentrador.c,238 :: 		if (estado==0){
	CP.B	W11, #0
	BRA Z	L__CambiarEstadoBandera218
	GOTO	L_CambiarEstadoBandera33
L__CambiarEstadoBandera218:
;Concentrador.c,239 :: 		banSPI0 = 0; banSPI1 = 0; banSPI2 = 0; banSPI7 = 0; banSPI8 = 0; banSPIA = 0;
	MOV	#lo_addr(_banSPI0), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI1), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI2), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI7), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPI8), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_banSPIA), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,240 :: 		}
L_CambiarEstadoBandera33:
;Concentrador.c,241 :: 		}
L_end_CambiarEstadoBandera:
	RETURN
; end of _CambiarEstadoBandera

_spi_1:
	PUSH	DSWPAG
	PUSH	50
	PUSH	RCOUNT
	PUSH	W0
	MOV	#2, W0
	REPEAT	#12
	PUSH	[W0++]

;Concentrador.c,251 :: 		void spi_1() org  IVT_ADDR_SPI1INTERRUPT {
;Concentrador.c,253 :: 		SPI1IF_bit = 0;
	PUSH	W10
	PUSH	W11
	PUSH	W12
	PUSH	W13
	BCLR	SPI1IF_bit, BitPos(SPI1IF_bit+0)
;Concentrador.c,254 :: 		bufferSPI = SPI1BUF;
	MOV	#lo_addr(_bufferSPI), W1
	MOV.B	SPI1BUF, WREG
	MOV.B	W0, [W1]
;Concentrador.c,257 :: 		if ((banSPI0==0)&&(bufferSPI==0xA0)) {
	MOV	#lo_addr(_banSPI0), W0
	MOV.B	[W0], W0
	CP.B	W0, #0
	BRA Z	L__spi_1220
	GOTO	L__spi_1144
L__spi_1220:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#160, W0
	CP.B	W1, W0
	BRA Z	L__spi_1221
	GOTO	L__spi_1143
L__spi_1221:
L__spi_1142:
;Concentrador.c,258 :: 		CambiarEstadoBandera(0,1);
	MOV.B	#1, W11
	CLR	W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,259 :: 		i = 1;
	MOV	#1, W0
	MOV	W0, _i
;Concentrador.c,260 :: 		SPI1BUF = tramaSolicitudSPI[0];
	MOV	#lo_addr(_tramaSolicitudSPI), W0
	ZE	[W0], W0
	MOV	WREG, SPI1BUF
;Concentrador.c,257 :: 		if ((banSPI0==0)&&(bufferSPI==0xA0)) {
L__spi_1144:
L__spi_1143:
;Concentrador.c,262 :: 		if ((banSPI0==1)&&(bufferSPI!=0xA0)&&(bufferSPI!=0xF0)){
	MOV	#lo_addr(_banSPI0), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1222
	GOTO	L__spi_1147
L__spi_1222:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#160, W0
	CP.B	W1, W0
	BRA NZ	L__spi_1223
	GOTO	L__spi_1146
L__spi_1223:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#240, W0
	CP.B	W1, W0
	BRA NZ	L__spi_1224
	GOTO	L__spi_1145
L__spi_1224:
L__spi_1141:
;Concentrador.c,263 :: 		SPI1BUF = tramaSolicitudSPI[i];
	MOV	#lo_addr(_tramaSolicitudSPI), W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], W0
	MOV.B	[W0], W0
	ZE	W0, W0
	MOV	WREG, SPI1BUF
;Concentrador.c,264 :: 		i++;
	MOV	#1, W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], [W0]
;Concentrador.c,262 :: 		if ((banSPI0==1)&&(bufferSPI!=0xA0)&&(bufferSPI!=0xF0)){
L__spi_1147:
L__spi_1146:
L__spi_1145:
;Concentrador.c,266 :: 		if ((banSPI0==1)&&(bufferSPI==0xF0)){
	MOV	#lo_addr(_banSPI0), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1225
	GOTO	L__spi_1149
L__spi_1225:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#240, W0
	CP.B	W1, W0
	BRA Z	L__spi_1226
	GOTO	L__spi_1148
L__spi_1226:
L__spi_1140:
;Concentrador.c,267 :: 		CambiarEstadoBandera(0,0);
	CLR	W11
	CLR	W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,266 :: 		if ((banSPI0==1)&&(bufferSPI==0xF0)){
L__spi_1149:
L__spi_1148:
;Concentrador.c,271 :: 		if ((banSPI1==0)&&(bufferSPI==0xA1)){
	MOV	#lo_addr(_banSPI1), W0
	MOV.B	[W0], W0
	CP.B	W0, #0
	BRA Z	L__spi_1227
	GOTO	L__spi_1151
L__spi_1227:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#161, W0
	CP.B	W1, W0
	BRA Z	L__spi_1228
	GOTO	L__spi_1150
L__spi_1228:
L__spi_1139:
;Concentrador.c,272 :: 		CambiarEstadoBandera(1,1);
	MOV.B	#1, W11
	MOV.B	#1, W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,273 :: 		i = 0;
	CLR	W0
	MOV	W0, _i
;Concentrador.c,271 :: 		if ((banSPI1==0)&&(bufferSPI==0xA1)){
L__spi_1151:
L__spi_1150:
;Concentrador.c,275 :: 		if ((banSPI1==1)&&(bufferSPI!=0xA1)&&(bufferSPI!=0xF1)){
	MOV	#lo_addr(_banSPI1), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1229
	GOTO	L__spi_1154
L__spi_1229:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#161, W0
	CP.B	W1, W0
	BRA NZ	L__spi_1230
	GOTO	L__spi_1153
L__spi_1230:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#241, W0
	CP.B	W1, W0
	BRA NZ	L__spi_1231
	GOTO	L__spi_1152
L__spi_1231:
L__spi_1138:
;Concentrador.c,276 :: 		tramaSolicitudSPI[i] = bufferSPI;                                       //Direccion del nodo + indicador de sobrescritura SD
	MOV	#lo_addr(_tramaSolicitudSPI), W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], W1
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], [W1]
;Concentrador.c,277 :: 		i++;
	MOV	#1, W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], [W0]
;Concentrador.c,275 :: 		if ((banSPI1==1)&&(bufferSPI!=0xA1)&&(bufferSPI!=0xF1)){
L__spi_1154:
L__spi_1153:
L__spi_1152:
;Concentrador.c,279 :: 		if ((banSPI1==1)&&(bufferSPI==0xF1)){
	MOV	#lo_addr(_banSPI1), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1232
	GOTO	L__spi_1156
L__spi_1232:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#241, W0
	CP.B	W1, W0
	BRA Z	L__spi_1233
	GOTO	L__spi_1155
L__spi_1233:
L__spi_1137:
;Concentrador.c,280 :: 		direccionRS485 = tramaSolicitudSPI[0];
	MOV	#lo_addr(_direccionRS485), W1
	MOV	#lo_addr(_tramaSolicitudSPI), W0
	MOV.B	[W0], [W1]
;Concentrador.c,281 :: 		outputPyloadRS485[0] = 0xD1;
	MOV	#lo_addr(_outputPyloadRS485), W1
	MOV.B	#209, W0
	MOV.B	W0, [W1]
;Concentrador.c,282 :: 		outputPyloadRS485[1] = tramaSolicitudSPI[1];
	MOV	#lo_addr(_outputPyloadRS485+1), W1
	MOV	#lo_addr(_tramaSolicitudSPI+1), W0
	MOV.B	[W0], [W1]
;Concentrador.c,283 :: 		EnviarTramaRS485(RS485_PUERTO_UART2, direccionRS485, 0xF2, 2, outputPyloadRS485);
	MOV	#lo_addr(_tramaSolicitudSPI), W0
	MOV	#2, W13
	MOV.B	#242, W12
	MOV.B	[W0], W11
	MOV.B	#2, W10
	MOV	#lo_addr(_outputPyloadRS485), W0
	PUSH	W0
	CALL	_EnviarTramaRS485
	SUB	#2, W15
;Concentrador.c,284 :: 		CambiarEstadoBandera(1,0);
	CLR	W11
	MOV.B	#1, W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,279 :: 		if ((banSPI1==1)&&(bufferSPI==0xF1)){
L__spi_1156:
L__spi_1155:
;Concentrador.c,288 :: 		if ((banSPI2==0)&&(bufferSPI==0xA2)){
	MOV	#lo_addr(_banSPI2), W0
	MOV.B	[W0], W0
	CP.B	W0, #0
	BRA Z	L__spi_1234
	GOTO	L__spi_1158
L__spi_1234:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#162, W0
	CP.B	W1, W0
	BRA Z	L__spi_1235
	GOTO	L__spi_1157
L__spi_1235:
L__spi_1136:
;Concentrador.c,289 :: 		CambiarEstadoBandera(2,1);
	MOV.B	#1, W11
	MOV.B	#2, W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,290 :: 		i = 0;
	CLR	W0
	MOV	W0, _i
;Concentrador.c,288 :: 		if ((banSPI2==0)&&(bufferSPI==0xA2)){
L__spi_1158:
L__spi_1157:
;Concentrador.c,292 :: 		if ((banSPI2==1)&&(bufferSPI!=0xA2)&&(bufferSPI!=0xF2)){
	MOV	#lo_addr(_banSPI2), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1236
	GOTO	L__spi_1161
L__spi_1236:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#162, W0
	CP.B	W1, W0
	BRA NZ	L__spi_1237
	GOTO	L__spi_1160
L__spi_1237:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#242, W0
	CP.B	W1, W0
	BRA NZ	L__spi_1238
	GOTO	L__spi_1159
L__spi_1238:
L__spi_1135:
;Concentrador.c,293 :: 		tramaSolicitudSPI[i] = bufferSPI;
	MOV	#lo_addr(_tramaSolicitudSPI), W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], W1
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], [W1]
;Concentrador.c,292 :: 		if ((banSPI2==1)&&(bufferSPI!=0xA2)&&(bufferSPI!=0xF2)){
L__spi_1161:
L__spi_1160:
L__spi_1159:
;Concentrador.c,295 :: 		if ((banSPI2==1)&&(bufferSPI==0xF2)){
	MOV	#lo_addr(_banSPI2), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1239
	GOTO	L__spi_1163
L__spi_1239:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#242, W0
	CP.B	W1, W0
	BRA Z	L__spi_1240
	GOTO	L__spi_1162
L__spi_1240:
L__spi_1134:
;Concentrador.c,296 :: 		direccionRS485 = tramaSolicitudSPI[0];
	MOV	#lo_addr(_direccionRS485), W1
	MOV	#lo_addr(_tramaSolicitudSPI), W0
	MOV.B	[W0], [W1]
;Concentrador.c,297 :: 		outputPyloadRS485[0] = 0xD2;
	MOV	#lo_addr(_outputPyloadRS485), W1
	MOV.B	#210, W0
	MOV.B	W0, [W1]
;Concentrador.c,298 :: 		EnviarTramaRS485(RS485_PUERTO_UART2, direccionRS485, 0xF2, 1, outputPyloadRS485);
	MOV	#lo_addr(_tramaSolicitudSPI), W0
	MOV	#1, W13
	MOV.B	#242, W12
	MOV.B	[W0], W11
	MOV.B	#2, W10
	MOV	#lo_addr(_outputPyloadRS485), W0
	PUSH	W0
	CALL	_EnviarTramaRS485
	SUB	#2, W15
;Concentrador.c,299 :: 		CambiarEstadoBandera(2,0);
	CLR	W11
	MOV.B	#2, W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,295 :: 		if ((banSPI2==1)&&(bufferSPI==0xF2)){
L__spi_1163:
L__spi_1162:
;Concentrador.c,303 :: 		if ((banSPI7==0)&&(bufferSPI==0xA7)){
	MOV	#lo_addr(_banSPI7), W0
	MOV.B	[W0], W0
	CP.B	W0, #0
	BRA Z	L__spi_1241
	GOTO	L__spi_1165
L__spi_1241:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#167, W0
	CP.B	W1, W0
	BRA Z	L__spi_1242
	GOTO	L__spi_1164
L__spi_1242:
L__spi_1133:
;Concentrador.c,304 :: 		CambiarEstadoBandera(7,1);
	MOV.B	#1, W11
	MOV.B	#7, W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,305 :: 		i = 0;
	CLR	W0
	MOV	W0, _i
;Concentrador.c,303 :: 		if ((banSPI7==0)&&(bufferSPI==0xA7)){
L__spi_1165:
L__spi_1164:
;Concentrador.c,307 :: 		if ((banSPI7==1)&&(bufferSPI!=0xA7)&&(bufferSPI!=0xF7)){
	MOV	#lo_addr(_banSPI7), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1243
	GOTO	L__spi_1168
L__spi_1243:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#167, W0
	CP.B	W1, W0
	BRA NZ	L__spi_1244
	GOTO	L__spi_1167
L__spi_1244:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#247, W0
	CP.B	W1, W0
	BRA NZ	L__spi_1245
	GOTO	L__spi_1166
L__spi_1245:
L__spi_1132:
;Concentrador.c,308 :: 		tramaSolicitudSPI[i] = bufferSPI;
	MOV	#lo_addr(_tramaSolicitudSPI), W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], W1
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], [W1]
;Concentrador.c,307 :: 		if ((banSPI7==1)&&(bufferSPI!=0xA7)&&(bufferSPI!=0xF7)){
L__spi_1168:
L__spi_1167:
L__spi_1166:
;Concentrador.c,310 :: 		if ((banSPI7==1)&&(bufferSPI==0xF7)){
	MOV	#lo_addr(_banSPI7), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1246
	GOTO	L__spi_1170
L__spi_1246:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#247, W0
	CP.B	W1, W0
	BRA Z	L__spi_1247
	GOTO	L__spi_1169
L__spi_1247:
L__spi_1131:
;Concentrador.c,311 :: 		direccionRS485 = tramaSolicitudSPI[i];
	MOV	#lo_addr(_tramaSolicitudSPI), W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], W0
	MOV.B	[W0], W2
	MOV	#lo_addr(_direccionRS485), W0
	MOV.B	W2, [W0]
;Concentrador.c,312 :: 		outputPyloadRS485[0] = 0xD2;
	MOV	#lo_addr(_outputPyloadRS485), W1
	MOV.B	#210, W0
	MOV.B	W0, [W1]
;Concentrador.c,313 :: 		EnviarTramaRS485(RS485_PUERTO_UART2, direccionRS485, 0xF1, 1, outputPyloadRS485);
	MOV	#1, W13
	MOV.B	#241, W12
	MOV.B	W2, W11
	MOV.B	#2, W10
	MOV	#lo_addr(_outputPyloadRS485), W0
	PUSH	W0
	CALL	_EnviarTramaRS485
	SUB	#2, W15
;Concentrador.c,314 :: 		T2CON.TON = 1;                                                             //Inicia el Timeout 2 (espera de respuesta del nodo)
	BSET	T2CON, #15
;Concentrador.c,315 :: 		TMR2 = 0;
	CLR	TMR2
;Concentrador.c,316 :: 		banRespuestaPi = 1;
	MOV	#lo_addr(_banRespuestaPi), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
;Concentrador.c,317 :: 		CambiarEstadoBandera(7,0);
	CLR	W11
	MOV.B	#7, W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,310 :: 		if ((banSPI7==1)&&(bufferSPI==0xF7)){
L__spi_1170:
L__spi_1169:
;Concentrador.c,321 :: 		if ((banSPI8==0)&&(bufferSPI==0xA8)){
	MOV	#lo_addr(_banSPI8), W0
	MOV.B	[W0], W0
	CP.B	W0, #0
	BRA Z	L__spi_1248
	GOTO	L__spi_1172
L__spi_1248:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#168, W0
	CP.B	W1, W0
	BRA Z	L__spi_1249
	GOTO	L__spi_1171
L__spi_1249:
L__spi_1130:
;Concentrador.c,322 :: 		CambiarEstadoBandera(8,1);
	MOV.B	#1, W11
	MOV.B	#8, W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,323 :: 		i = 0;
	CLR	W0
	MOV	W0, _i
;Concentrador.c,321 :: 		if ((banSPI8==0)&&(bufferSPI==0xA8)){
L__spi_1172:
L__spi_1171:
;Concentrador.c,325 :: 		if ((banSPI8==1)&&(i<4)){
	MOV	#lo_addr(_banSPI8), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1250
	GOTO	L__spi_1174
L__spi_1250:
	MOV	_i, W0
	CP	W0, #4
	BRA LTU	L__spi_1251
	GOTO	L__spi_1173
L__spi_1251:
L__spi_1129:
;Concentrador.c,326 :: 		tramaSolicitudNodo[i] = bufferSPI;                                      //Cabecera: [0x3A, Direccion, Funcion, numDatos]
	MOV	#lo_addr(_tramaSolicitudNodo), W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], W1
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], [W1]
;Concentrador.c,327 :: 		i++;
	MOV	#1, W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], [W0]
;Concentrador.c,325 :: 		if ((banSPI8==1)&&(i<4)){
L__spi_1174:
L__spi_1173:
;Concentrador.c,329 :: 		if ((banSPI8==1)&&(i==4)){
	MOV	#lo_addr(_banSPI8), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1252
	GOTO	L__spi_1176
L__spi_1252:
	MOV	_i, W0
	CP	W0, #4
	BRA Z	L__spi_1253
	GOTO	L__spi_1175
L__spi_1253:
L__spi_1128:
;Concentrador.c,330 :: 		direccionRS485 = tramaSolicitudNodo[1];
	MOV	#lo_addr(_direccionRS485), W1
	MOV	#lo_addr(_tramaSolicitudNodo+1), W0
	MOV.B	[W0], [W1]
;Concentrador.c,331 :: 		funcionRS485   = tramaSolicitudNodo[2];
	MOV	#lo_addr(_funcionRS485), W1
	MOV	#lo_addr(_tramaSolicitudNodo+2), W0
	MOV.B	[W0], [W1]
;Concentrador.c,332 :: 		numDatosRS485  = tramaSolicitudNodo[3];
	MOV	#lo_addr(_tramaSolicitudNodo+3), W0
	ZE	[W0], W0
	MOV	W0, _numDatosRS485
;Concentrador.c,333 :: 		i = 0;
	CLR	W0
	MOV	W0, _i
;Concentrador.c,334 :: 		banSPI8 = 2;
	MOV	#lo_addr(_banSPI8), W1
	MOV.B	#2, W0
	MOV.B	W0, [W1]
;Concentrador.c,329 :: 		if ((banSPI8==1)&&(i==4)){
L__spi_1176:
L__spi_1175:
;Concentrador.c,336 :: 		if ((banSPI8==2)&&(i<=numDatosRS485)){
	MOV	#lo_addr(_banSPI8), W0
	MOV.B	[W0], W0
	CP.B	W0, #2
	BRA Z	L__spi_1254
	GOTO	L__spi_1178
L__spi_1254:
	MOV	_i, W1
	MOV	#lo_addr(_numDatosRS485), W0
	CP	W1, [W0]
	BRA LEU	L__spi_1255
	GOTO	L__spi_1177
L__spi_1255:
L__spi_1127:
;Concentrador.c,337 :: 		tramaSolicitudNodo[i] = bufferSPI;
	MOV	#lo_addr(_tramaSolicitudNodo), W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], W1
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], [W1]
;Concentrador.c,338 :: 		i++;
	MOV	#1, W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], [W0]
;Concentrador.c,336 :: 		if ((banSPI8==2)&&(i<=numDatosRS485)){
L__spi_1178:
L__spi_1177:
;Concentrador.c,340 :: 		if ((banSPI8==2)&&(bufferSPI==0xF8)&&(i>numDatosRS485)){
	MOV	#lo_addr(_banSPI8), W0
	MOV.B	[W0], W0
	CP.B	W0, #2
	BRA Z	L__spi_1256
	GOTO	L__spi_1181
L__spi_1256:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#248, W0
	CP.B	W1, W0
	BRA Z	L__spi_1257
	GOTO	L__spi_1180
L__spi_1257:
	MOV	_i, W1
	MOV	#lo_addr(_numDatosRS485), W0
	CP	W1, [W0]
	BRA GTU	L__spi_1258
	GOTO	L__spi_1179
L__spi_1258:
L__spi_1126:
;Concentrador.c,341 :: 		CambiarEstadoBandera(8,0);
	CLR	W11
	MOV.B	#8, W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,342 :: 		if (numDatosRS485>1){
	MOV	_numDatosRS485, W0
	CP	W0, #1
	BRA GTU	L__spi_1259
	GOTO	L_spi_185
L__spi_1259:
;Concentrador.c,343 :: 		for (x=0;x<numDatosRS485;x++){
	CLR	W0
	MOV	W0, _x
L_spi_186:
	MOV	_x, W1
	MOV	#lo_addr(_numDatosRS485), W0
	CP	W1, [W0]
	BRA LTU	L__spi_1260
	GOTO	L_spi_187
L__spi_1260:
;Concentrador.c,344 :: 		outputPyloadRS485[x] = tramaSolicitudNodo[x+1];
	MOV	#lo_addr(_outputPyloadRS485), W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], W2
	MOV	_x, W0
	ADD	W0, #1, W1
	MOV	#lo_addr(_tramaSolicitudNodo), W0
	ADD	W0, W1, W0
	MOV.B	[W0], [W2]
;Concentrador.c,343 :: 		for (x=0;x<numDatosRS485;x++){
	MOV	#1, W1
	MOV	#lo_addr(_x), W0
	ADD	W1, [W0], [W0]
;Concentrador.c,345 :: 		}
	GOTO	L_spi_186
L_spi_187:
;Concentrador.c,346 :: 		} else {
	GOTO	L_spi_189
L_spi_185:
;Concentrador.c,347 :: 		outputPyloadRS485[0] = tramaSolicitudNodo[1];
	MOV	#lo_addr(_outputPyloadRS485), W1
	MOV	#lo_addr(_tramaSolicitudNodo+1), W0
	MOV.B	[W0], [W1]
;Concentrador.c,348 :: 		}
L_spi_189:
;Concentrador.c,349 :: 		banRSI = 0;
	MOV	#lo_addr(_banRSI), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,350 :: 		banRSC = 0;
	MOV	#lo_addr(_banRSC), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,351 :: 		i_rs485 = 0;
	CLR	W0
	MOV	W0, _i_rs485
;Concentrador.c,352 :: 		banRespuestaPi = 1;
	MOV	#lo_addr(_banRespuestaPi), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
;Concentrador.c,353 :: 		EnviarTramaRS485(RS485_PUERTO_UART2, direccionRS485, funcionRS485, numDatosRS485, outputPyloadRS485);
	MOV	#lo_addr(_funcionRS485), W1
	MOV	#lo_addr(_direccionRS485), W0
	MOV	_numDatosRS485, W13
	MOV.B	[W1], W12
	MOV.B	[W0], W11
	MOV.B	#2, W10
	MOV	#lo_addr(_outputPyloadRS485), W0
	PUSH	W0
	CALL	_EnviarTramaRS485
	SUB	#2, W15
;Concentrador.c,354 :: 		if (direccionRS485 != RS485_DIR_BROADCAST) {
	MOV	#lo_addr(_direccionRS485), W0
	MOV.B	[W0], W1
	MOV.B	#255, W0
	CP.B	W1, W0
	BRA NZ	L__spi_1261
	GOTO	L_spi_190
L__spi_1261:
;Concentrador.c,355 :: 		T2CON.TON = 1;                                                         //Inicia el Timeout 2 solo para respuestas individuales
	BSET	T2CON, #15
;Concentrador.c,356 :: 		TMR2 = 0;
	CLR	TMR2
;Concentrador.c,357 :: 		}
L_spi_190:
;Concentrador.c,340 :: 		if ((banSPI8==2)&&(bufferSPI==0xF8)&&(i>numDatosRS485)){
L__spi_1181:
L__spi_1180:
L__spi_1179:
;Concentrador.c,361 :: 		if ((banSPIA==0)&&(bufferSPI==0xAA)){
	MOV	#lo_addr(_banSPIA), W0
	MOV.B	[W0], W0
	CP.B	W0, #0
	BRA Z	L__spi_1262
	GOTO	L__spi_1183
L__spi_1262:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#170, W0
	CP.B	W1, W0
	BRA Z	L__spi_1263
	GOTO	L__spi_1182
L__spi_1263:
L__spi_1125:
;Concentrador.c,362 :: 		CambiarEstadoBandera(0x0A,1);
	MOV.B	#1, W11
	MOV.B	#10, W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,363 :: 		SPI1BUF = inputPyloadRS485[0];
	MOV	#lo_addr(_inputPyloadRS485), W0
	ZE	[W0], W0
	MOV	WREG, SPI1BUF
;Concentrador.c,364 :: 		i = 1;
	MOV	#1, W0
	MOV	W0, _i
;Concentrador.c,361 :: 		if ((banSPIA==0)&&(bufferSPI==0xAA)){
L__spi_1183:
L__spi_1182:
;Concentrador.c,366 :: 		if ((banSPIA==1)&&(bufferSPI!=0xAA)&&(bufferSPI!=0xFA)){
	MOV	#lo_addr(_banSPIA), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1264
	GOTO	L__spi_1186
L__spi_1264:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#170, W0
	CP.B	W1, W0
	BRA NZ	L__spi_1265
	GOTO	L__spi_1185
L__spi_1265:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#250, W0
	CP.B	W1, W0
	BRA NZ	L__spi_1266
	GOTO	L__spi_1184
L__spi_1266:
L__spi_1124:
;Concentrador.c,367 :: 		SPI1BUF = inputPyloadRS485[i];
	MOV	#lo_addr(_inputPyloadRS485), W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], W0
	MOV.B	[W0], W0
	ZE	W0, W0
	MOV	WREG, SPI1BUF
;Concentrador.c,368 :: 		i++;
	MOV	#1, W1
	MOV	#lo_addr(_i), W0
	ADD	W1, [W0], [W0]
;Concentrador.c,366 :: 		if ((banSPIA==1)&&(bufferSPI!=0xAA)&&(bufferSPI!=0xFA)){
L__spi_1186:
L__spi_1185:
L__spi_1184:
;Concentrador.c,370 :: 		if ((banSPIA==1)&&(bufferSPI==0xFA)){
	MOV	#lo_addr(_banSPIA), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__spi_1267
	GOTO	L__spi_1188
L__spi_1267:
	MOV	#lo_addr(_bufferSPI), W0
	MOV.B	[W0], W1
	MOV.B	#250, W0
	CP.B	W1, W0
	BRA Z	L__spi_1268
	GOTO	L__spi_1187
L__spi_1268:
L__spi_1123:
;Concentrador.c,371 :: 		CambiarEstadoBandera(0x0A,0);
	CLR	W11
	MOV.B	#10, W10
	CALL	_CambiarEstadoBandera
;Concentrador.c,370 :: 		if ((banSPIA==1)&&(bufferSPI==0xFA)){
L__spi_1188:
L__spi_1187:
;Concentrador.c,374 :: 		}
L_end_spi_1:
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
; end of _spi_1

_Timer3Int:
	PUSH	DSWPAG
	PUSH	50
	PUSH	RCOUNT
	PUSH	W0
	MOV	#2, W0
	REPEAT	#12
	PUSH	[W0++]

;Concentrador.c,379 :: 		void Timer3Int() org IVT_ADDR_T3INTERRUPT{
;Concentrador.c,383 :: 		T3IF_bit = 0;
	BCLR	T3IF_bit, BitPos(T3IF_bit+0)
;Concentrador.c,384 :: 		contadorSync++;
	MOV	#1, W1
	MOV	#lo_addr(Timer3Int_contadorSync_L0), W0
	ADD	W1, [W0], [W0]
;Concentrador.c,386 :: 		if (contadorSync >= (PERIODO_SYNC_MS/100)){
	MOV	Timer3Int_contadorSync_L0, W0
	CP	W0, #10
	BRA GEU	L__Timer3Int270
	GOTO	L_Timer3Int100
L__Timer3Int270:
;Concentrador.c,387 :: 		contadorSync = 0;
	CLR	W0
	MOV	W0, Timer3Int_contadorSync_L0
;Concentrador.c,389 :: 		INT_SINC = ~INT_SINC;                                                  //D5: heartbeat de sincronismo
	BTG	LATA1_bit, BitPos(LATA1_bit+0)
;Concentrador.c,392 :: 		INT_SINC_1 = 1;
	BSET	LATA0_bit, BitPos(LATA0_bit+0)
;Concentrador.c,393 :: 		Delay_us(ANCHO_PULSO_SYNC_US);
	MOV	#8000, W7
L_Timer3Int101:
	DEC	W7
	BRA NZ	L_Timer3Int101
	NOP
	NOP
;Concentrador.c,394 :: 		INT_SINC_1 = 0;
	BCLR	LATA0_bit, BitPos(LATA0_bit+0)
;Concentrador.c,395 :: 		}
L_Timer3Int100:
;Concentrador.c,397 :: 		}
L_end_Timer3Int:
	MOV	#26, W0
	REPEAT	#12
	POP	[W0--]
	POP	W0
	POP	RCOUNT
	POP	50
	POP	DSWPAG
	RETFIE
; end of _Timer3Int

_Timer2Int:
	PUSH	DSWPAG
	PUSH	50
	PUSH	RCOUNT
	PUSH	W0
	MOV	#2, W0
	REPEAT	#12
	PUSH	[W0++]

;Concentrador.c,402 :: 		void Timer2Int() org IVT_ADDR_T2INTERRUPT{
;Concentrador.c,404 :: 		T2IF_bit = 0;
	PUSH	W10
	PUSH	W11
	PUSH	W12
	BCLR	T2IF_bit, BitPos(T2IF_bit+0)
;Concentrador.c,405 :: 		T2CON.TON = 0;
	BCLR	T2CON, #15
;Concentrador.c,406 :: 		TMR2 = 0;
	CLR	TMR2
;Concentrador.c,408 :: 		INT_SINC = ~INT_SINC;                                                     //D5: tambien se conmuta en caso de timeout
	BTG	LATA1_bit, BitPos(LATA1_bit+0)
;Concentrador.c,410 :: 		banRSI = 0;
	MOV	#lo_addr(_banRSI), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,411 :: 		banRSC = 0;
	MOV	#lo_addr(_banRSC), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,412 :: 		i_rs485 = 0;
	CLR	W0
	MOV	W0, _i_rs485
;Concentrador.c,415 :: 		numDatosRS485 = 3;
	MOV	#3, W0
	MOV	W0, _numDatosRS485
;Concentrador.c,416 :: 		inputPyloadRS485[0] = 0xD3;
	MOV	#lo_addr(_inputPyloadRS485), W1
	MOV.B	#211, W0
	MOV.B	W0, [W1]
;Concentrador.c,417 :: 		inputPyloadRS485[1] = 0xEE;
	MOV	#lo_addr(_inputPyloadRS485+1), W1
	MOV.B	#238, W0
	MOV.B	W0, [W1]
;Concentrador.c,418 :: 		inputPyloadRS485[2] = 0xE4;
	MOV	#lo_addr(_inputPyloadRS485+2), W1
	MOV.B	#228, W0
	MOV.B	W0, [W1]
;Concentrador.c,419 :: 		banRespuestaPi = 1;
	MOV	#lo_addr(_banRespuestaPi), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
;Concentrador.c,420 :: 		InterrupcionP1(0xB3,0xD3,3);
	MOV	#3, W12
	MOV.B	#211, W11
	MOV.B	#179, W10
	CALL	_InterrupcionP1
;Concentrador.c,422 :: 		}
L_end_Timer2Int:
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

_urx_2:
	PUSH	DSWPAG
	PUSH	50
	PUSH	RCOUNT
	PUSH	W0
	MOV	#2, W0
	REPEAT	#12
	PUSH	[W0++]

;Concentrador.c,427 :: 		void urx_2() org  IVT_ADDR_U2RXINTERRUPT {
;Concentrador.c,429 :: 		U2RXIF_bit = 0;
	PUSH	W10
	PUSH	W11
	PUSH	W12
	BCLR	U2RXIF_bit, BitPos(U2RXIF_bit+0)
;Concentrador.c,430 :: 		byteRS485 = U2RXREG;
	MOV	#lo_addr(_byteRS485), W1
	MOV.B	U2RXREG, WREG
	MOV.B	W0, [W1]
;Concentrador.c,431 :: 		U2STA.OERR = 0;
	BCLR.B	U2STA, #1
;Concentrador.c,434 :: 		if (banRSI==2){
	MOV	#lo_addr(_banRSI), W0
	MOV.B	[W0], W0
	CP.B	W0, #2
	BRA Z	L__urx_2273
	GOTO	L_urx_2103
L__urx_2273:
;Concentrador.c,435 :: 		if (i_rs485<(numDatosRS485)){
	MOV	_i_rs485, W1
	MOV	#lo_addr(_numDatosRS485), W0
	CP	W1, [W0]
	BRA LTU	L__urx_2274
	GOTO	L_urx_2104
L__urx_2274:
;Concentrador.c,436 :: 		inputPyloadRS485[i_rs485] = byteRS485;
	MOV	#lo_addr(_inputPyloadRS485), W1
	MOV	#lo_addr(_i_rs485), W0
	ADD	W1, [W0], W1
	MOV	#lo_addr(_byteRS485), W0
	MOV.B	[W0], [W1]
;Concentrador.c,437 :: 		i_rs485++;
	MOV	#1, W1
	MOV	#lo_addr(_i_rs485), W0
	ADD	W1, [W0], [W0]
;Concentrador.c,438 :: 		} else {
	GOTO	L_urx_2105
L_urx_2104:
;Concentrador.c,439 :: 		banRSI = 0;
	MOV	#lo_addr(_banRSI), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,440 :: 		banRSC = 1;
	MOV	#lo_addr(_banRSC), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
;Concentrador.c,441 :: 		}
L_urx_2105:
;Concentrador.c,442 :: 		}
L_urx_2103:
;Concentrador.c,445 :: 		if ((banRSI==0)&&(banRSC==0)){
	MOV	#lo_addr(_banRSI), W0
	MOV.B	[W0], W0
	CP.B	W0, #0
	BRA Z	L__urx_2275
	GOTO	L__urx_2193
L__urx_2275:
	MOV	#lo_addr(_banRSC), W0
	MOV.B	[W0], W0
	CP.B	W0, #0
	BRA Z	L__urx_2276
	GOTO	L__urx_2192
L__urx_2276:
L__urx_2191:
;Concentrador.c,446 :: 		if (byteRS485==RS485_BYTE_INICIO){
	MOV	#lo_addr(_byteRS485), W0
	MOV.B	[W0], W1
	MOV.B	#58, W0
	CP.B	W1, W0
	BRA Z	L__urx_2277
	GOTO	L_urx_2109
L__urx_2277:
;Concentrador.c,447 :: 		banRSI = 1;
	MOV	#lo_addr(_banRSI), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
;Concentrador.c,448 :: 		i_rs485 = 0;
	CLR	W0
	MOV	W0, _i_rs485
;Concentrador.c,449 :: 		}
L_urx_2109:
;Concentrador.c,445 :: 		if ((banRSI==0)&&(banRSC==0)){
L__urx_2193:
L__urx_2192:
;Concentrador.c,451 :: 		if ((banRSI==1)&&(i_rs485<5)){
	MOV	#lo_addr(_banRSI), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__urx_2278
	GOTO	L__urx_2195
L__urx_2278:
	MOV	_i_rs485, W0
	CP	W0, #5
	BRA LTU	L__urx_2279
	GOTO	L__urx_2194
L__urx_2279:
L__urx_2190:
;Concentrador.c,452 :: 		tramaCabeceraRS485[i_rs485] = byteRS485;
	MOV	#lo_addr(_tramaCabeceraRS485), W1
	MOV	#lo_addr(_i_rs485), W0
	ADD	W1, [W0], W1
	MOV	#lo_addr(_byteRS485), W0
	MOV.B	[W0], [W1]
;Concentrador.c,453 :: 		i_rs485++;
	MOV	#1, W1
	MOV	#lo_addr(_i_rs485), W0
	ADD	W1, [W0], [W0]
;Concentrador.c,451 :: 		if ((banRSI==1)&&(i_rs485<5)){
L__urx_2195:
L__urx_2194:
;Concentrador.c,455 :: 		if ((banRSI==1)&&(i_rs485==5)){
	MOV	#lo_addr(_banRSI), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__urx_2280
	GOTO	L__urx_2197
L__urx_2280:
	MOV	_i_rs485, W0
	CP	W0, #5
	BRA Z	L__urx_2281
	GOTO	L__urx_2196
L__urx_2281:
L__urx_2189:
;Concentrador.c,456 :: 		if (tramaCabeceraRS485[1]==direccionRS485){
	MOV	#lo_addr(_tramaCabeceraRS485+1), W0
	MOV.B	[W0], W1
	MOV	#lo_addr(_direccionRS485), W0
	CP.B	W1, [W0]
	BRA Z	L__urx_2282
	GOTO	L_urx_2116
L__urx_2282:
;Concentrador.c,457 :: 		T2CON.TON = 0;                                                          //Detiene el Timeout 2
	BCLR	T2CON, #15
;Concentrador.c,458 :: 		TMR2 = 0;
	CLR	TMR2
;Concentrador.c,459 :: 		funcionRS485 = tramaCabeceraRS485[2];
	MOV	#lo_addr(_funcionRS485), W1
	MOV	#lo_addr(_tramaCabeceraRS485+2), W0
	MOV.B	[W0], [W1]
;Concentrador.c,460 :: 		*(ptrnumDatosRS485)   = tramaCabeceraRS485[3];                       //LSB numDatosRS485
	MOV	#lo_addr(_tramaCabeceraRS485+3), W1
	MOV	_ptrnumDatosRS485, W0
	MOV.B	[W1], [W0]
;Concentrador.c,461 :: 		*(ptrnumDatosRS485+1) = tramaCabeceraRS485[4];                       //MSB numDatosRS485
	MOV	_ptrnumDatosRS485, W0
	ADD	W0, #1, W1
	MOV	#lo_addr(_tramaCabeceraRS485+4), W0
	MOV.B	[W0], [W1]
;Concentrador.c,462 :: 		banRSI = 2;
	MOV	#lo_addr(_banRSI), W1
	MOV.B	#2, W0
	MOV.B	W0, [W1]
;Concentrador.c,463 :: 		i_rs485 = 0;
	CLR	W0
	MOV	W0, _i_rs485
;Concentrador.c,464 :: 		} else {
	GOTO	L_urx_2117
L_urx_2116:
;Concentrador.c,465 :: 		banRSI = 0;
	MOV	#lo_addr(_banRSI), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,466 :: 		banRSC = 0;
	MOV	#lo_addr(_banRSC), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,467 :: 		i_rs485 = 0;
	CLR	W0
	MOV	W0, _i_rs485
;Concentrador.c,468 :: 		}
L_urx_2117:
;Concentrador.c,455 :: 		if ((banRSI==1)&&(i_rs485==5)){
L__urx_2197:
L__urx_2196:
;Concentrador.c,472 :: 		if (banRSC==1){
	MOV	#lo_addr(_banRSC), W0
	MOV.B	[W0], W0
	CP.B	W0, #1
	BRA Z	L__urx_2283
	GOTO	L_urx_2118
L__urx_2283:
;Concentrador.c,473 :: 		subFuncionRS485 = inputPyloadRS485[0];
	MOV	#lo_addr(_subFuncionRS485), W1
	MOV	#lo_addr(_inputPyloadRS485), W0
	MOV.B	[W0], [W1]
;Concentrador.c,474 :: 		banRespuestaPi = 1;
	MOV	#lo_addr(_banRespuestaPi), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
;Concentrador.c,476 :: 		INT_SINC_2 = ~INT_SINC_2;                                              //D2: trama valida recibida de un nodo (trafico RS485)
	BTG	LATA3_bit, BitPos(LATA3_bit+0)
;Concentrador.c,478 :: 		switch (funcionRS485){
	GOTO	L_urx_2119
;Concentrador.c,479 :: 		case 0xF1: InterrupcionP1(0xB1,subFuncionRS485,numDatosRS485); break;
L_urx_2121:
	MOV	#lo_addr(_subFuncionRS485), W0
	MOV	_numDatosRS485, W12
	MOV.B	[W0], W11
	MOV.B	#177, W10
	CALL	_InterrupcionP1
	GOTO	L_urx_2120
;Concentrador.c,480 :: 		case 0xF3: InterrupcionP1(0xB3,subFuncionRS485,numDatosRS485); break;
L_urx_2122:
	MOV	#lo_addr(_subFuncionRS485), W0
	MOV	_numDatosRS485, W12
	MOV.B	[W0], W11
	MOV.B	#179, W10
	CALL	_InterrupcionP1
	GOTO	L_urx_2120
;Concentrador.c,481 :: 		}
L_urx_2119:
	MOV	#lo_addr(_funcionRS485), W0
	MOV.B	[W0], W1
	MOV.B	#241, W0
	CP.B	W1, W0
	BRA NZ	L__urx_2284
	GOTO	L_urx_2121
L__urx_2284:
	MOV	#lo_addr(_funcionRS485), W0
	MOV.B	[W0], W1
	MOV.B	#243, W0
	CP.B	W1, W0
	BRA NZ	L__urx_2285
	GOTO	L_urx_2122
L__urx_2285:
L_urx_2120:
;Concentrador.c,482 :: 		banRSC = 0;
	MOV	#lo_addr(_banRSC), W1
	CLR	W0
	MOV.B	W0, [W1]
;Concentrador.c,483 :: 		}
L_urx_2118:
;Concentrador.c,484 :: 		}
L_end_urx_2:
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
; end of _urx_2
