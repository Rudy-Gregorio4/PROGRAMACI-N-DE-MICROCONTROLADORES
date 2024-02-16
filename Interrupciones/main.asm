;**************************
; Universidad del Valle de Guatemala
; Programación de Microcrontroladores
; Proyecto: Laboratorio CON INTERRUPCIONES
; Archivo: INTERRUPCIONES.asm
; Hardware: ATMEGA328p
; Created: 08/02/2024 18:11:46
; Author : RUDY GREGORIO
;**************************
; Encabezado: INTERRUPCIONES
;**************************

.include "M328PDEF.inc"

.cseg
.org 0x00
	JMP MAIN

.org 0x0006
	JMP ISR_PCINT0	

.org 0x0020
	JMP ISR_TIMER0_OVF	;VECTOR TIMER 0 CON OVERFLOW
//****************************************************
//STACK POINTER
//****************************************************
MAIN: 
LDI R16, LOW(RAMEND)
OUT SPL, R16
LDI R17, HIGH(RAMEND)
OUT SPH, R17

//****************************************************

//****************************************************
//CONFIGURACIÓN DEL MCU
//****************************************************
SETUP:
	//CONFIGURACIÓN DEL PRESCALER
	LDI R16, 0b1000_0000
	LDI R16, (1 << CLKPCE)
	STS CLKPR, R16

	LDI R16, 0b0000_0001
	STS CLKPR, R16		;FRECUENCIA 8MHZ

	LDI R16, 0b0111_1111
	OUT DDRD, R16		;PUERTO D COMO SALIDA

	LDI R16, 0b0000_0111
	OUT DDRB, R16		;PUERTO B, COMO ENTRADA Y SALIDA
	
	LDI R16, 0b0000_0111
	OUT PORTB, R16		;PULLUP EN LOS PINES

	LDI R16, 0b0011_1111
	OUT DDRC, R16
	LDI R18, 0

	LDI R16, (1 << PCIE0)
	STS PCICR, R16		; PCINT 0-7 HABILITADO

	LDI R16, (1 << PCINT1) | (1 << PCINT2)
	STS PCMSK0, R16
	SBI PINB, PB4
	SEI		;INTERRUPCIONES GLOBALES

	LDI R19, 0
	LDI R17, 0
	LDI R25, 0
	LDI R28, 0

	TABLA: .DB 0x7F, 0x0E, 0xB7, 0x9F, 0xCE, 0xDB, 0xFB, 0x0F, 0xFF, 0xCF

	LDI R21, 0	;DECENAS
	LDI R22, 0	;UNIDADES

	CALL INITTIMER0


	
//****************************************************
//Configuración LOOP
//****************************************************

LOOP:
	CPI R22, 10	;COMPARAMOS SI LAS UNIDADES LLEGARON A 10
	BREQ RESETT
	CPI R23, 50	;COMPARAMOS LAS PASADAS QUE HAYA DADO EL TIMER0
	BREQ UNIDADES

	CALL DELAY0
	SBI PINB, PB3
	SBI PINB, PB4


	CALL TABLA_DECENAS
	CALL TABLA_UNIDADES

//*********** DELAYS *********************
DELAY0:
	LDI R19, 255
DELAY:
	DEC R19
	BRNE DELAY
	LDI R19, 255
DELAY1:
	DEC R19
	BRNE DELAY1
	LDI R19, 255
DELAY2:
	DEC R19
	BRNE DELAY2
	LDI R19, 255
DELAY3:
	DEC R19
	BRNE DELAY
RET

RESETT:
	LDI R22, 0
	INC R21
	JMP LOOP

UNIDADES:
	INC R22
	LDI R23, 0
	JMP LOOP

RESDE:			;SE RESETEA TODO A 0
	CALL DELAY
	LDI R21, 0
	LDI R22, 0
	JMP LOOP










//**************** TIMER0 ********************
INITTIMER0:
	LDI R26, 0
	OUT TCCR0A, R26

	LDI R26, (1 << CS02) | (1 << CS00)
	OUT TCCR0B, R26		;PREESCALER A 1024

	LDI R26, 100
	OUT TCNT0, R26

	LDI R26, (1 << TOIE0)
	STS TIMSK0, R26

	RET





//****************************************************
//Subrutina 
//****************************************************

	//************	OPERACIÓN DE TABLA DEL DISPLAY DE DECENAS ****************
TABLA_DECENAS:
	LDI ZH, HIGH(TABLA<<1)
	LDI ZL, LOW(TABLA<<1)
	ADD	ZL, R21
	LPM	R25, Z
	OUT	PORTD, R25

	CALL DELAY0
	SBI PINB, PB3
	SBI PINB, PB4
	RET

	//************	OPERACIÓN DE TABLA DEL DISPLAY DE UNIDADES****************
TABLA_UNIDADES:
	LDI ZH, HIGH(TABLA<<1)
	LDI ZL, LOW(TABLA<<1)
	ADD	ZL, R22
	LPM	R25, Z
	OUT	PORTD, R25
	CALL DELAY

	CPI R21, 6
	BREQ RESDE
	JMP LOOP
	RET

ISR_PCINT0:
	PUSH R16
	IN R16, SREG
	PUSH R16

	IN R20,  PINB	;LEER EL PUERTO B
	SBRC R20, PB1

	JMP CPB2

	DEC R18
	JMP SALIDA

CPB2:
	SBRC R20, PB2
	JMP SALIDA

	INC R18
	JMP SALIDA

SALIDA:
	CPI R18, -1
	BREQ RES1
	CPI R18, 16
	BREQ RES2

	OUT PORTC, R18
	SBI PCIFR, PCIF0	;APARGAR BANDERA DE ISR PCINT0

	POP R16
	OUT SREG, R16
	POP R16
	RETI


RES1:
	LDI R18, 0
	OUT PORTC, R18
	JMP SALIDA


RES2:
	LDI R18, 15
	OUT PORTC, R18
	JMP SALIDA

//********** SUBRUTINA DEL TIMER0
ISR_TIMER0_OVF:
	PUSH R16		;GUARDAR EL VALOR DE R16 EN LA PILA
	IN R16, SREG	;GUARDAR EL VALOR DEL SREG EN R6
	PUSH R16

	LDI R16, 100	;DESBORDAMIENTO 
	OUT TCNT0, R16	;CONTADOR INICIAL EN 100
	SBI TIFR0, TOV0	;SETEAR LA BANDERA TOV0
	INC R23			;INCREMENTAR EL VALOR 

	POP R16			;DEVOLVER EL VALOR DEL SREG
	OUT SREG, R16
	POP R16			;DEVOLVER EL VALOR ANTIGUO DE R16
	RETI			;VOLVER DONDE FUE LLAMADA




/*TABLA: .DB 0x7F, 0x0E, 0xB7, 0x9F, 0xCE, 0xDB, 0xFB, 0x0F, 0xFF, 0xCF, 0xEF, 0xFA, 0x73, 0xBE, 0xF3, 0xE3

OPERACION_TABLA:
	LDI ZH, HIGH(TABLA<<1)
	LDI ZL, LOW(TABLA<<1)
	ADD	ZL, R20
	LPM	R24, Z
	OUT	PORTD, R24 
	RET*/


//****************************************************