/*
 * CFile1.c
 *
 * Created: 4/12/2024 2:27:04 PM
 *  Author: Rudy Gregorio
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#include "T1PWM.h"
void initADC(void){
		ADMUX = 7;
		//REFERENCIA A 5V
		ADMUX |= (1<<REFS0);
		ADMUX &= ~(1<<REFS1);
		
		//JUSTIFICACION IZQUIERDA
		ADMUX |= (1<<ADLAR);
		
		ADCSRA = 0;

		//INTERRUPCION DEL ADC
		ADCSRA |= (1<<ADIE);
		
		//PREESCALER DE 125KHZ
		ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
		
		//HABILITACION DEL ADC
		ADCSRA |= (1<<ADEN);
}