;**************************
; Universidad del Valle de Guatemala
; Programación de Microcrontroladores
; Proyecto: Laboratorio Contador
; Archivo: Contador_Laboratoio_2_Progra_de_Micro.asm
; Hardware: ATMEGA328p
; Created: 30/01/2024 18:11:46
; Author : RUDY GREGORIO
;**************************
; Encabezado: DISPLAY 7 SEGMENTOS
;**************************

.include "M328PDEF.inc" ; para reconozer los nombres de los registros
.cseg ; indica que lo que viene después es el segmento de código
.org 0x00 ; establecemos la dirección en posición 0
;**************************
; stack pointer
;**************************
LDI R16, LOW(RAMEND)
OUT SPL, R16 
LDI R17, HIGH(RAMEND)
OUT SPH, R17

;**********************************************************

;**************************
; Configuración
;**************************
Setup:
	  ; Configurar temporizador (Timer 0)
	LDI R16, (1 << CS02) | (1 << CS00) ; Configurar prescaler a 1024
	OUT TCCR0B, R16
	LDI R16, 0xFF
	OUT DDRB, R16
	CALL TIMER0
	LDI R19, 0



;CONFIGURACIÓN DE PUERTOS Y PINES
	LDI R16, 0b0001_1111 ; CONFIGURAMOS LOS PULLUPS 
	OUT PORTC, R16	; HABILITAMOS EL PULLUPS
	LDI R18, 0b0000_0000 ;METEMOS ESE VALOR EN BINARIO EN EL REGISTRO R18
	OUT DDRC, R18 ; DEFINIMOS COMO ENTRADAS LOS PUERTOS C

	LDI R16, 0b1111_1111 ;METEMOS ESE VALOR EN BINARIO EN EL REGISTRO R16
	OUT DDRD, R16 ;DEFINIMOS CÓMO SALIDA LOS PUERTOS D
	LDI R16, 0b1111_1111 ;METEMOS ESE VALOR EN BINARIO EN EL	 REGISTRO R16
	OUT DDRB, R16 ;DEFINIMOS CÓMO SALIDAS LOS PUERTOS B 



MAIN:
	LDI R24, 0
	LDI ZH, HIGH(TABLA<<1)
	LDI ZL, LOW(TABLA<<1)	
	//ADD ZL, R24
	LPM R24, Z
	OUT	PORTD, R24
	CLR R16
	STS UCSR0B, R16

LOOP:

;LEER SI EL PUSH ESTÁ PRESIONADO 
	SBIS PINC, PC0
	CALL INC1

	SBIS PINC, PC1 ;PORTC
	CALL DEC1

;TIMER 0
	IN R16, TIFR0
	CPI R16, (1<<TOV0)
	BRNE LOOP		;Si no esta seteada, continuar esperando 

	LDI R16, 195
	OUT TCNT0, R16

	SBI TIFR0, TOV0
	INC R19
	CPI R19, 10
	BRNE LOOP
	CALL INCREMENTO_LEDS
	
	
INCREMENTO_LEDS:
	CPI R21, 0x0F
    BREQ RESET_LEDS
	INC R21

LEDS_ON:
	OUT PORTB, R21
	RJMP LOOP
	RET

TIMER0:
	LDI R23, (1<<CS02) |  (1<<CS00)
	OUT TCCR0B, R23
	
	LDI R23, 195			;CARGAR EL VALOR DE DESBORDAMIENTO
	OUT TCNT0, R16			;Cargar el valor incial 
	RET
RESET_LEDS:
	CLR R21
	RJMP LEDS_ON
	
	;RJMP LOOP



;**************************
; Subrutinas
;**************************
;ESPERAMOS UN BREVE TIEMPO 
DELAY:
	LDI R16, 200; METEMOS EL VALOR DECIMAL DE "200" EN EL REGISTRO 100
LOOP_DELAY:
	DEC R16
	CPI R16,0
	BRNE LOOP_DELAY ; SI NO ES IGUAL A CERO, RECGRESA EL DELAY
	RET

INC1:
	CALL DELAY
	LDI R23, (0<<CS02) |  (0<<CS00)
	OUT TCCR0B, R23
	CALL TIMER0
	SBIS PINC, PC0 ; SI EL BIT "1" DEL PINC ESTÁ ENCENDIDO SALTA DE LÍNEA.
	RJMP INC1 ; REGRESO A LA FUNCIÓN 
	;CBR R20, 4
	;CBR R20, 0b0010_0000	
	INC R20 ; INCREMENTO EL VALOR DE R20
	SBRC R20, 4 ; SI EL BIT "4" DEL REGISTRO 20 ESTÁ APAGADO SALTA A LA LÍNEA DE JUMP
	CLR R20 ;SE LIMPIA EL VALOR DE R20
	CALL OPERACION_TABLA
	//OUT PIND, R20	
	RET

DEC1:
	CALL DELAY
	LDI R23, (0<<CS02) |  (0<<CS00)
	OUT TCCR0B, R23
	CALL TIMER0
	SBIS PINC, PC1 ; SI EL BIT "1" DEL PINC ESTÁ ENCENDIDO SALTA DE LÍNEA.
	RJMP DEC1 ; REGRESO A LA FUNCIÓN DB
	DEC R20 ; DECREMENTO EL VALOR DE R17
	SBRC R20, 4 ; SI EL BIT "4" DEL REGISTRO 20 ESTÁ APAGADO SALTA A LA LÍNEA DE JUMP
	LDI R20, 0B0000_1111
	CALL OPERACION_TABLA
	RET


TABLA: .DB 0x7F, 0x0E, 0xB7, 0x9F, 0xCE, 0xDB, 0xFB, 0x0F, 0xFF, 0xCF, 0xEF, 0xFA, 0x73, 0xBE, 0xF3, 0xE3

OPERACION_TABLA:
	LDI ZH, HIGH(TABLA<<1)
	LDI ZL, LOW(TABLA<<1)
	ADD	ZL, R20
	LPM	R24, Z
	OUT	PORTD, R24 
	RET



