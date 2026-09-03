
_my_u32_to_str:
	LNK	#22

	MOV	#0, W0
	MOV	W0, [W14+16]
	MOV	#0, W0
	MOV	W0, [W14+18]
	MOV	[W14-10], W0
	MOV	[W14-8], W1
	CP	W0, #0
	CPB	W1, #0
	BRA Z	L__my_u32_to_str82
	GOTO	L_my_u32_to_str0
L__my_u32_to_str82:
	MOV	[W14-12], W1
	MOV.B	#48, W0
	MOV.B	W0, [W1]
	MOV	[W14-12], W0
	ADD	W0, #1, W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#1, W0
	GOTO	L_end_my_u32_to_str
L_my_u32_to_str0:
L_my_u32_to_str1:
	MOV	[W14-10], W0
	MOV	[W14-8], W1
	CP	W0, #0
	CPB	W1, #0
	BRA GTU	L__my_u32_to_str83
	GOTO	L_my_u32_to_str2
L__my_u32_to_str83:
	ADD	W14, #0, W1
	ADD	W14, #16, W0
	ADD	W1, [W0], W0
	MOV	W0, [W14+20]
	MOV	#10, W2
	MOV	#0, W3
	MOV	[W14-10], W0
	MOV	[W14-8], W1
	CLR	W4
	CALL	__Modulus_32x32
	MOV	#48, W2
	MOV	#0, W3
	ADD	W2, W0, W2
	MOV	[W14+20], W0
	MOV.B	W2, [W0]
	MOV	#1, W1
	ADD	W14, #16, W0
	ADD	W1, [W0], [W0]
	MOV	#10, W2
	MOV	#0, W3
	MOV	[W14-10], W0
	MOV	[W14-8], W1
	CLR	W4
	CALL	__Divide_32x32
	MOV	W0, [W14-10]
	MOV	W1, [W14-8]
	GOTO	L_my_u32_to_str1
L_my_u32_to_str2:
	CLR	W0
	MOV	W0, [W14+18]
L_my_u32_to_str3:
	MOV	[W14+18], W1
	ADD	W14, #16, W0
	CP	W1, [W0]
	BRA LTU	L__my_u32_to_str84
	GOTO	L_my_u32_to_str4
L__my_u32_to_str84:
	MOV	[W14-12], W1
	ADD	W14, #18, W0
	ADD	W1, [W0], W2
	MOV	[W14+16], W1
	ADD	W14, #18, W0
	SUB	W1, [W0], W0
	SUB	W0, #1, W1
	ADD	W14, #0, W0
	ADD	W0, W1, W0
	MOV.B	[W0], [W2]
	MOV	#1, W1
	ADD	W14, #18, W0
	ADD	W1, [W0], [W0]
	GOTO	L_my_u32_to_str3
L_my_u32_to_str4:
	MOV	[W14-12], W1
	ADD	W14, #16, W0
	ADD	W1, [W0], W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	[W14+16], W0
L_end_my_u32_to_str:
	ULNK
	RETURN
; end of _my_u32_to_str

_spi_xchg:
	LNK	#0

	SUB	W14, #8, W0
	ZE	[W0], W10
	CALL	_SPI1_Read
L_end_spi_xchg:
	ULNK
	RETURN
; end of _spi_xchg

_spi_cs_low:

	BCLR	LATB5_bit, BitPos(LATB5_bit+0)
L_end_spi_cs_low:
	RETURN
; end of _spi_cs_low

_spi_cs_high:

	BSET	LATB5_bit, BitPos(LATB5_bit+0)
L_end_spi_cs_high:
	RETURN
; end of _spi_cs_high

_my_strcpy:
	LNK	#0

L_my_strcpy6:
	MOV	[W14-10], W1
	MOV	#___Lib_System_DefaultPage, W0
	MOV	WREG, 50
	MOV.B	[W1], W0
	CP0.B	W0
	BRA NZ	L__my_strcpy89
	GOTO	L_my_strcpy7
L__my_strcpy89:
	MOV	[W14-10], W1
	MOV	#___Lib_System_DefaultPage, W0
	MOV	WREG, 50
	MOV.B	[W1], W1
	MOV	[W14-8], W0
	MOV.B	W1, [W0]
	MOV	#1, W1
	SUB	W14, #8, W0
	ADD	W1, [W0], [W0]
	MOV	#1, W1
	SUB	W14, #10, W0
	ADD	W1, [W0], [W0]
	GOTO	L_my_strcpy6
L_my_strcpy7:
	MOV	[W14-8], W1
	CLR	W0
	MOV.B	W0, [W1]
L_end_my_strcpy:
	ULNK
	RETURN
; end of _my_strcpy

_my_strcat:
	LNK	#0

L_my_strcat8:
	MOV	[W14-8], W0
	CP0.B	[W0]
	BRA NZ	L__my_strcat91
	GOTO	L_my_strcat9
L__my_strcat91:
	MOV	#1, W1
	SUB	W14, #8, W0
	ADD	W1, [W0], [W0]
	GOTO	L_my_strcat8
L_my_strcat9:
	SUB	W14, #10, W0
	PUSH	[W0]
	SUB	W14, #8, W0
	PUSH	[W0]
	CALL	_my_strcpy
	SUB	#4, W15
L_end_my_strcat:
	ULNK
	RETURN
; end of _my_strcat

_sd_cmd:
	LNK	#2

	CALL	_spi_cs_high
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	CALL	_spi_cs_low
	MOV	#64, W1
	SUB	W14, #8, W0
	ZE	[W0], W0
	IOR	W1, W0, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	SUB	W14, #12, W2
	MOV.D	[W2], W0
	LSR	W1, #8, W0
	CLR	W1
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	SUB	W14, #12, W2
	MOV	[++W2], W0
	CLR	W1
	DEC2	W2
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV	[W14-12], W2
	MOV	[W14-10], W3
	LSR	W2, #8, W0
	SL	W3, #8, W1
	IOR	W0, W1, W0
	LSR	W3, #8, W1
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	SUB	W14, #12, W0
	PUSH	[W0]
	CALL	_spi_xchg
	SUB	#2, W15
	SUB	W14, #14, W0
	ZE	[W0], W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	CLR	W0
	MOV.B	W0, [W14+0]
L_sd_cmd10:
	MOV.B	[W14+0], W0
	CP.B	W0, #10
	BRA LTU	L__sd_cmd93
	GOTO	L_sd_cmd11
L__sd_cmd93:
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV.B	W0, [W14+1]
	MOV.B	#255, W1
	CP.B	W0, W1
	BRA NZ	L__sd_cmd94
	GOTO	L_sd_cmd13
L__sd_cmd94:
	GOTO	L_sd_cmd11
L_sd_cmd13:
	MOV.B	#1, W1
	ADD	W14, #0, W0
	ADD.B	W1, [W0], [W0]
	GOTO	L_sd_cmd10
L_sd_cmd11:
	MOV.B	[W14+1], W0
L_end_sd_cmd:
	ULNK
	RETURN
; end of _sd_cmd

_sd_wait_idle:
	LNK	#4

	MOV	#10000, W0
	MOV	W0, [W14+2]
L_sd_wait_idle14:
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV.B	W0, [W14+0]
	MOV	#1, W2
	ADD	W14, #2, W1
	SUBR	W2, [W1], [W1]
	CP.B	W0, #0
	BRA Z	L__sd_wait_idle96
	GOTO	L__sd_wait_idle78
L__sd_wait_idle96:
	MOV	[W14+2], W0
	CP	W0, #0
	BRA GTU	L__sd_wait_idle97
	GOTO	L__sd_wait_idle78
L__sd_wait_idle97:
	GOTO	L_sd_wait_idle14
L__sd_wait_idle78:
	MOV.B	[W14+0], W0
L_end_sd_wait_idle:
	ULNK
	RETURN
; end of _sd_wait_idle

_sd_init:
	LNK	#4

	CALL	_spi_cs_high
	CLR	W0
	MOV	W0, [W14+0]
L_sd_init19:
	MOV	[W14+0], W0
	CP	W0, #10
	BRA LTU	L__sd_init99
	GOTO	L_sd_init20
L__sd_init99:
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV	#1, W1
	ADD	W14, #0, W0
	ADD	W1, [W0], [W0]
	GOTO	L_sd_init19
L_sd_init20:
	CALL	_spi_cs_low
	MOV	#149, W0
	PUSH	W0
	CLR	W0
	ASR	W0, #15, W1
	PUSH.D	W0
	CLR	W0
	PUSH	W0
	CALL	_sd_cmd
	SUB	#8, W15
	MOV.B	W0, [W14+2]
	CP.B	W0, #1
	BRA NZ	L__sd_init100
	GOTO	L_sd_init22
L__sd_init100:
	CALL	_spi_cs_high
	CLR	W0
	GOTO	L_end_sd_init
L_sd_init22:
	CALL	_spi_cs_low
	MOV	#135, W0
	PUSH	W0
	MOV	#426, W0
	MOV	#0, W1
	PUSH.D	W0
	MOV	#8, W0
	PUSH	W0
	CALL	_sd_cmd
	SUB	#8, W15
	MOV.B	W0, [W14+2]
	CP.B	W0, #1
	BRA Z	L__sd_init101
	GOTO	L_sd_init23
L__sd_init101:
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
L_sd_init23:
	CLR	W0
	MOV	W0, [W14+0]
L_sd_init24:
	MOV	[W14+0], W1
	MOV	#2000, W0
	CP	W1, W0
	BRA LTU	L__sd_init102
	GOTO	L_sd_init25
L__sd_init102:
	CALL	_spi_cs_low
	MOV	#255, W0
	PUSH	W0
	CLR	W0
	ASR	W0, #15, W1
	PUSH.D	W0
	MOV	#55, W0
	PUSH	W0
	CALL	_sd_cmd
	SUB	#8, W15
	MOV.B	W0, [W14+2]
	CP.B	W0, #1
	BRA NZ	L__sd_init103
	GOTO	L_sd_init27
L__sd_init103:
	CALL	_spi_cs_high
	CLR	W0
	GOTO	L_end_sd_init
L_sd_init27:
	MOV	#255, W0
	PUSH	W0
	MOV	#0, W0
	MOV	#16384, W1
	PUSH.D	W0
	MOV	#41, W0
	PUSH	W0
	CALL	_sd_cmd
	SUB	#8, W15
	MOV.B	W0, [W14+2]
	CP.B	W0, #0
	BRA Z	L__sd_init104
	GOTO	L_sd_init28
L__sd_init104:
	GOTO	L_sd_init25
L_sd_init28:
	MOV	#8000, W7
L_sd_init29:
	DEC	W7
	BRA NZ	L_sd_init29
	NOP
	NOP
	MOV	#1, W1
	ADD	W14, #0, W0
	ADD	W1, [W0], [W0]
	GOTO	L_sd_init24
L_sd_init25:
	MOV.B	[W14+2], W0
	CP.B	W0, #0
	BRA NZ	L__sd_init105
	GOTO	L_sd_init31
L__sd_init105:
	CALL	_spi_cs_high
	CLR	W0
	GOTO	L_end_sd_init
L_sd_init31:
	CALL	_spi_cs_high
	MOV.B	#1, W0
L_end_sd_init:
	ULNK
	RETURN
; end of _sd_init

_sd_read_sector:
	LNK	#4

	CALL	_spi_cs_low
	MOV	#255, W0
	PUSH	W0
	SUB	W14, #10, W0
	PUSH	[W0++]
	PUSH	[W0--]
	MOV	#17, W0
	PUSH	W0
	CALL	_sd_cmd
	SUB	#8, W15
	CP.B	W0, #0
	BRA NZ	L__sd_read_sector107
	GOTO	L_sd_read_sector32
L__sd_read_sector107:
	CALL	_spi_cs_high
	CLR	W0
	GOTO	L_end_sd_read_sector
L_sd_read_sector32:
L_sd_read_sector33:
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV.B	#254, W1
	CP.B	W0, W1
	BRA NZ	L__sd_read_sector108
	GOTO	L_sd_read_sector34
L__sd_read_sector108:
	GOTO	L_sd_read_sector33
L_sd_read_sector34:
	CLR	W0
	MOV	W0, [W14+0]
L_sd_read_sector35:
	MOV	[W14+0], W1
	MOV	#512, W0
	CP	W1, W0
	BRA LTU	L__sd_read_sector109
	GOTO	L_sd_read_sector36
L__sd_read_sector109:
	MOV	[W14-12], W1
	ADD	W14, #0, W0
	ADD	W1, [W0], W0
	MOV	W0, [W14+2]
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV	[W14+2], W1
	MOV.B	W0, [W1]
	MOV	#1, W1
	ADD	W14, #0, W0
	ADD	W1, [W0], [W0]
	GOTO	L_sd_read_sector35
L_sd_read_sector36:
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	CALL	_spi_cs_high
	MOV.B	#1, W0
L_end_sd_read_sector:
	ULNK
	RETURN
; end of _sd_read_sector

_sd_write_sector:
	LNK	#2

	CALL	_spi_cs_low
	MOV	#255, W0
	PUSH	W0
	SUB	W14, #10, W0
	PUSH	[W0++]
	PUSH	[W0--]
	MOV	#24, W0
	PUSH	W0
	CALL	_sd_cmd
	SUB	#8, W15
	CP.B	W0, #0
	BRA NZ	L__sd_write_sector111
	GOTO	L_sd_write_sector38
L__sd_write_sector111:
	CALL	_spi_cs_high
	CLR	W0
	GOTO	L_end_sd_write_sector
L_sd_write_sector38:
	MOV	#254, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	CLR	W0
	MOV	W0, [W14+0]
L_sd_write_sector39:
	MOV	[W14+0], W1
	MOV	#512, W0
	CP	W1, W0
	BRA LTU	L__sd_write_sector112
	GOTO	L_sd_write_sector40
L__sd_write_sector112:
	MOV	[W14-12], W1
	ADD	W14, #0, W0
	ADD	W1, [W0], W1
	MOV	#___Lib_System_DefaultPage, W0
	MOV	WREG, 50
	MOV.B	[W1], W0
	ZE	W0, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV	#1, W1
	ADD	W14, #0, W0
	ADD	W1, [W0], [W0]
	GOTO	L_sd_write_sector39
L_sd_write_sector40:
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	ZE	W0, W0
	AND	W0, #31, W0
	CP	W0, #5
	BRA NZ	L__sd_write_sector113
	GOTO	L_sd_write_sector42
L__sd_write_sector113:
	CALL	_spi_cs_high
	CLR	W0
	GOTO	L_end_sd_write_sector
L_sd_write_sector42:
L_sd_write_sector43:
	MOV	#255, W0
	PUSH	W0
	CALL	_spi_xchg
	SUB	#2, W15
	CP.B	W0, #0
	BRA Z	L__sd_write_sector114
	GOTO	L_sd_write_sector44
L__sd_write_sector114:
	GOTO	L_sd_write_sector43
L_sd_write_sector44:
	CALL	_spi_cs_high
	MOV.B	#1, W0
L_end_sd_write_sector:
	ULNK
	RETURN
; end of _sd_write_sector

_Led3PulsoRapido:
	LNK	#2

	CLR	W0
	MOV	W0, [W14+0]
L_Led3PulsoRapido45:
	MOV	[W14+0], W0
	CP	W0, #3
	BRA LTU	L__Led3PulsoRapido116
	GOTO	L_Led3PulsoRapido46
L__Led3PulsoRapido116:
	BSET	LATA2_bit, BitPos(LATA2_bit+0)
	MOV	#49, W8
	MOV	#54300, W7
L_Led3PulsoRapido48:
	DEC	W7
	BRA NZ	L_Led3PulsoRapido48
	DEC	W8
	BRA NZ	L_Led3PulsoRapido48
	NOP
	NOP
	NOP
	BCLR	LATA2_bit, BitPos(LATA2_bit+0)
	MOV	#49, W8
	MOV	#54300, W7
L_Led3PulsoRapido50:
	DEC	W7
	BRA NZ	L_Led3PulsoRapido50
	DEC	W8
	BRA NZ	L_Led3PulsoRapido50
	NOP
	NOP
	NOP
	MOV	#1, W1
	ADD	W14, #0, W0
	ADD	W1, [W0], [W0]
	GOTO	L_Led3PulsoRapido45
L_Led3PulsoRapido46:
	MOV	#123, W8
	MOV	#4681, W7
L_Led3PulsoRapido52:
	DEC	W7
	BRA NZ	L_Led3PulsoRapido52
	DEC	W8
	BRA NZ	L_Led3PulsoRapido52
L_end_Led3PulsoRapido:
	ULNK
	RETURN
; end of _Led3PulsoRapido

_my_strlen:
	LNK	#2

	MOV	#0, W0
	MOV	W0, [W14+0]
L_my_strlen54:
	SUB	W14, #8, W0
	CP0	[W0]
	BRA NZ	L__my_strlen118
	GOTO	L_my_strlen55
L__my_strlen118:
	MOV	[W14-8], W2
	MOV	#1, W1
	SUB	W14, #8, W0
	ADD	W1, [W0], [W0]
	MOV	#___Lib_System_DefaultPage, W0
	MOV	WREG, 50
	MOV.B	[W2], W0
	CP0.B	W0
	BRA NZ	L__my_strlen119
	GOTO	L_my_strlen55
L__my_strlen119:
L__my_strlen79:
	MOV	#1, W1
	ADD	W14, #0, W0
	ADD	W1, [W0], [W0]
	GOTO	L_my_strlen54
L_my_strlen55:
	MOV	[W14+0], W0
L_end_my_strlen:
	ULNK
	RETURN
; end of _my_strlen

_sd_log_append:
	LNK	#8

	MOV	#lo_addr(_sd_buf), W0
	PUSH	W0
	MOV	#1, W0
	MOV	#0, W1
	PUSH.D	W0
	CALL	_sd_read_sector
	SUB	#6, W15
	CP0.B	W0
	BRA Z	L__sd_log_append121
	GOTO	L_sd_log_append58
L__sd_log_append121:
	GOTO	L_end_sd_log_append
L_sd_log_append58:
	MOV	#lo_addr(_sd_buf), W0
	ZE	[W0], W4
	CLR	W5
	MOV	#lo_addr(_sd_buf+1), W0
	ZE	[W0], W2
	CLR	W3
	SL	W3, #8, W1
	LSR	W2, #8, W0
	IOR	W0, W1, W1
	SL	W2, #8, W0
	IOR	W4, W0, W2
	IOR	W5, W1, W3
	MOV	#lo_addr(_sd_buf+2), W0
	ZE	[W0], W0
	CLR	W1
	MOV	W0, W1
	CLR	W0
	IOR	W2, W0, W4
	IOR	W3, W1, W5
	MOV	#lo_addr(_sd_buf+3), W0
	ZE	[W0], W0
	CLR	W1
	SL	W0, #8, W3
	CLR	W2
	ADD	W14, #0, W0
	IOR	W4, W2, [W0++]
	IOR	W5, W3, [W0--]
	SUB	W14, #8, W0
	PUSH	[W0]
	CALL	_my_strlen
	SUB	#2, W15
	MOV	W0, [W14+4]
	MOV	W0, W3
	CLR	W4
	ADD	W14, #0, W2
	ADD	W3, [W2++], W0
	ADDC	W4, [W2--], W1
	ADD	W0, #2, W2
	ADDC	W1, #0, W3
	MOV	#480, W0
	MOV	#0, W1
	CP	W2, W0
	CPB	W3, W1
	BRA GTU	L__sd_log_append122
	GOTO	L_sd_log_append59
L__sd_log_append122:
	CLR	W0
	CLR	W1
	MOV	W0, [W14+0]
	MOV	W1, [W14+2]
L_sd_log_append59:
	CLR	W0
	MOV	W0, [W14+6]
L_sd_log_append60:
	MOV	[W14+6], W1
	ADD	W14, #4, W0
	CP	W1, [W0]
	BRA LTU	L__sd_log_append123
	GOTO	L_sd_log_append61
L__sd_log_append123:
	MOV	[W14+0], W0
	MOV	[W14+2], W1
	ADD	W0, #8, W2
	ADDC	W1, #0, W3
	MOV	[W14+6], W0
	CLR	W1
	ADD	W2, W0, W2
	ADDC	W3, W1, W3
	MOV	#lo_addr(_sd_buf), W0
	ADD	W0, W2, W2
	MOV	[W14-8], W1
	ADD	W14, #6, W0
	ADD	W1, [W0], W1
	MOV	#___Lib_System_DefaultPage, W0
	MOV	WREG, 50
	MOV.B	[W1], W0
	MOV.B	W0, [W2]
	MOV	#1, W1
	ADD	W14, #6, W0
	ADD	W1, [W0], [W0]
	GOTO	L_sd_log_append60
L_sd_log_append61:
	MOV	[W14+0], W0
	MOV	[W14+2], W1
	ADD	W0, #8, W2
	ADDC	W1, #0, W3
	MOV	[W14+4], W0
	CLR	W1
	ADD	W2, W0, W2
	ADDC	W3, W1, W3
	MOV	#lo_addr(_sd_buf), W0
	ADD	W0, W2, W1
	MOV.B	#13, W0
	MOV.B	W0, [W1]
	MOV	[W14+0], W0
	MOV	[W14+2], W1
	ADD	W0, #9, W2
	ADDC	W1, #0, W3
	MOV	[W14+4], W0
	CLR	W1
	ADD	W2, W0, W2
	ADDC	W3, W1, W3
	MOV	#lo_addr(_sd_buf), W0
	ADD	W0, W2, W1
	MOV.B	#10, W0
	MOV.B	W0, [W1]
	MOV	[W14+4], W0
	INC2	W0
	MOV	W0, W1
	CLR	W2
	ADD	W14, #0, W0
	ADD	W1, [W0++], W4
	ADDC	W2, [W0--], W5
	MOV	W4, [W14+0]
	MOV	W5, [W14+2]
	MOV.B	#255, W1
	MOV	#lo_addr(_sd_buf), W0
	AND.B	W4, W1, [W0]
	MOV	#lo_addr(_sd_buf), W1
	MOV	#lo_addr(_sd_buf), W0
	MOV.B	[W0], [W1]
	LSR	W4, #8, W2
	SL	W5, #8, W3
	IOR	W2, W3, W2
	LSR	W5, #8, W3
	MOV.B	#255, W1
	MOV	#lo_addr(_sd_buf+1), W0
	AND.B	W2, W1, [W0]
	MOV	#lo_addr(_sd_buf+1), W1
	MOV	#lo_addr(_sd_buf+1), W0
	MOV.B	[W0], [W1]
	MOV	W5, W2
	CLR	W3
	MOV.B	#255, W1
	MOV	#lo_addr(_sd_buf+2), W0
	AND.B	W2, W1, [W0]
	MOV	#lo_addr(_sd_buf+2), W1
	MOV	#lo_addr(_sd_buf+2), W0
	MOV.B	[W0], [W1]
	LSR	W5, #8, W2
	CLR	W3
	MOV.B	#255, W1
	MOV	#lo_addr(_sd_buf+3), W0
	AND.B	W2, W1, [W0]
	MOV	#lo_addr(_sd_buf+3), W1
	MOV	#lo_addr(_sd_buf+3), W0
	MOV.B	[W0], [W1]
	MOV	#lo_addr(_sd_buf), W0
	PUSH	W0
	MOV	#1, W0
	MOV	#0, W1
	PUSH.D	W0
	CALL	_sd_write_sector
	SUB	#6, W15
L_end_sd_log_append:
	ULNK
	RETURN
; end of _sd_log_append

_int_1:
	PUSH	DSWPAG
	PUSH	50
	PUSH	RCOUNT
	PUSH	W0
	MOV	#2, W0
	REPEAT	#12
	PUSH	[W0++]

	BCLR	INT1IF_bit, BitPos(INT1IF_bit+0)
	BTG	LATA2_bit, BitPos(LATA2_bit+0)
	MOV	#lo_addr(_banPulsoSinc), W1
	MOV.B	#1, W0
	MOV.B	W0, [W1]
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

_main:
	MOV	#2048, W15
	MOV	#6142, W0
	MOV	WREG, 32
	MOV	#1, W0
	MOV	WREG, 50
	MOV	#4, W0
	IOR	68
	LNK	#48

	MOV	CLKDIVbits, W1
	MOV	#63743, W0
	AND	W1, W0, W0
	MOV	WREG, CLKDIVbits
	MOV	#lo_addr(CLKDIVbits), W0
	MOV.B	[W0], W1
	MOV.B	#63, W0
	AND.B	W1, W0, W1
	MOV	#lo_addr(CLKDIVbits), W0
	MOV.B	W1, [W0]
	MOV.B	#5, W0
	MOV.B	W0, W1
	MOV	#lo_addr(CLKDIVbits), W0
	XOR.B	W1, [W0], W1
	AND.B	W1, #31, W1
	MOV	#lo_addr(CLKDIVbits), W0
	XOR.B	W1, [W0], W1
	MOV	#lo_addr(CLKDIVbits), W0
	MOV.B	W1, [W0]
	MOV	#150, W0
	MOV	W0, W1
	MOV	#lo_addr(PLLFBDbits), W0
	XOR	W1, [W0], W1
	MOV	#511, W0
	AND	W1, W0, W1
	MOV	#lo_addr(PLLFBDbits), W0
	XOR	W1, [W0], W1
	MOV	W1, PLLFBDbits
	CLR	ANSELA
	CLR	ANSELB
	BCLR	TRISA2_bit, BitPos(TRISA2_bit+0)
	BCLR	LATA2_bit, BitPos(LATA2_bit+0)
	BSET	TRISB14_bit, BitPos(TRISB14_bit+0)
	BCLR	TRISB5_bit, BitPos(TRISB5_bit+0)
	BSET	LATB5_bit, BitPos(LATB5_bit+0)
	BSET	INTCON2, #15
	BCLR	INT1IE_bit, BitPos(INT1IE_bit+0)
	BCLR	INT1IF_bit, BitPos(INT1IF_bit+0)
	MOV.B	#1, W0
	MOV.B	W0, W1
	MOV	#lo_addr(IPC5bits), W0
	XOR.B	W1, [W0], W1
	AND.B	W1, #7, W1
	MOV	#lo_addr(IPC5bits), W0
	XOR.B	W1, [W0], W1
	MOV	#lo_addr(IPC5bits), W0
	MOV.B	W1, [W0]
	CLR	W0
	PUSH	W0
	CLR	W0
	PUSH	W0
	CLR	W0
	PUSH	W0
	CLR	W0
	PUSH	W0
	CLR	W13
	CLR	W12
	CLR	W11
	MOV	#32, W10
	CALL	_SPI1_Init_Advanced
	SUB	#8, W15
	CALL	_sd_init
	CP0.B	W0
	BRA Z	L__main126
	GOTO	L_main63
L__main126:
L_main64:
	BSET	LATA2_bit, BitPos(LATA2_bit+0)
	MOV	#25, W8
	MOV	#27150, W7
L_main66:
	DEC	W7
	BRA NZ	L_main66
	DEC	W8
	BRA NZ	L_main66
	NOP
	BCLR	LATA2_bit, BitPos(LATA2_bit+0)
	MOV	#98, W8
	MOV	#43066, W7
L_main68:
	DEC	W7
	BRA NZ	L_main68
	DEC	W8
	BRA NZ	L_main68
	GOTO	L_main64
L_main63:
	CALL	_Led3PulsoRapido
	MOV	#lo_addr(?lstr_1_nodo1), W0
	PUSH	W0
	CALL	_sd_log_append
	SUB	#2, W15
	MOV	#lo_addr(_sd_buf), W0
	PUSH	W0
	MOV	#1, W0
	MOV	#0, W1
	PUSH.D	W0
	CALL	_sd_read_sector
	SUB	#6, W15
	MOV	#lo_addr(_sd_buf), W0
	MOV.B	[W0], W1
	MOV.B	#255, W0
	CP.B	W1, W0
	BRA Z	L__main127
	GOTO	L_main72
L__main127:
	MOV	#lo_addr(_sd_buf+1), W0
	MOV.B	[W0], W1
	MOV.B	#255, W0
	CP.B	W1, W0
	BRA Z	L__main128
	GOTO	L_main72
L__main128:
	MOV	#lo_addr(_sd_buf+2), W0
	MOV.B	[W0], W1
	MOV.B	#255, W0
	CP.B	W1, W0
	BRA Z	L__main129
	GOTO	L_main72
L__main129:
	MOV	#lo_addr(_sd_buf+3), W0
	MOV.B	[W0], W1
	MOV.B	#255, W0
	CP.B	W1, W0
	BRA Z	L__main130
	GOTO	L_main72
L__main130:
L__main80:
	MOV	#lo_addr(_sd_buf), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_sd_buf+1), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_sd_buf+2), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_sd_buf+3), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(_sd_buf), W0
	PUSH	W0
	MOV	#1, W0
	MOV	#0, W1
	PUSH.D	W0
	CALL	_sd_write_sector
	SUB	#6, W15
L_main72:
	MOV	#lo_addr(_banPulsoSinc), W1
	CLR	W0
	MOV.B	W0, [W1]
	CLR	W0
	CLR	W1
	MOV	W0, _contPulsos
	MOV	W1, _contPulsos+2
	BCLR	INT1IF_bit, BitPos(INT1IF_bit+0)
	BSET	INT1IE_bit, BitPos(INT1IE_bit+0)
L_main73:
	CLRWDT
	MOV	#lo_addr(_banPulsoSinc), W0
	CP0.B	[W0]
	BRA NZ	L__main131
	GOTO	L_main75
L__main131:
	MOV	#lo_addr(_banPulsoSinc), W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#1, W1
	MOV	#0, W2
	MOV	#lo_addr(_contPulsos), W0
	ADD	W1, [W0], [W0++]
	ADDC	W2, [W0], [W0--]
	MOV	#32, W0
	ADD	W14, W0, W0
	PUSH	W0
	PUSH	_contPulsos
	PUSH	_contPulsos+2
	CALL	_my_u32_to_str
	SUB	#6, W15
	ADD	W14, #0, W1
	CLR	W0
	MOV.B	W0, [W1]
	MOV	#lo_addr(?lstr_2_nodo1), W0
	PUSH	W0
	PUSH	W1
	CALL	_my_strcpy
	SUB	#4, W15
	MOV	#32, W0
	ADD	W14, W0, W0
	PUSH	W0
	ADD	W14, #0, W0
	PUSH	W0
	CALL	_my_strcat
	SUB	#4, W15
	MOV	#lo_addr(?lstr_3_nodo1), W0
	PUSH	W0
	ADD	W14, #0, W0
	PUSH	W0
	CALL	_my_strcat
	SUB	#4, W15
	ADD	W14, #0, W0
	PUSH	W0
	CALL	_sd_log_append
	SUB	#2, W15
L_main75:
	MOV	#2, W8
	MOV	#14464, W7
L_main76:
	DEC	W7
	BRA NZ	L_main76
	DEC	W8
	BRA NZ	L_main76
	NOP
	NOP
	GOTO	L_main73
L_end_main:
	ULNK
L__main_end_loop:
	BRA	L__main_end_loop
; end of _main
