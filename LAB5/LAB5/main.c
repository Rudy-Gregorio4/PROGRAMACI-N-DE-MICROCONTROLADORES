//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Programación de Microcontroladores
// Autor: Rudy Gregorio
// Carné: 22127
// Proyecto: Laboratorio 5
// Hardware: Atmega238p
// Creado: 04/04/2024
//******************************************************************************


#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "T1PWM/T1PWM.h"

void setup(void);
void initADC(void);




int valorADC;




int main(void)
{
    setup();
	
    while (1) 
    {	
			ADCSRA |= (1<<ADSC);	//
			OCR1A  = valorADC/6;		
			
			if (OCR1A <0)
			{
				OCR1A = 0;
			}
			else if (OCR1A>100)
			{
				OCR1A = 100;
			}
				
	}

}


void setup(void){
	cli();
	
	DDRB = 0b11111111;   // Todo como salidas
	PORTB = 0b00000000;		
	
	
	DDRD = 0b11111111;  //Salida de DISPLAY
	PORTD =0b00000000;	//DISPLAY apagados
	
	/*DDRC = 0b00111111;  //Salida de led
	PORTC =0b00000000;	//Iniciamos los leds apagados*/
	
	

	UCSR0B = 0;
	TCCR1A = 0;
	TCCR1B = 0;
	
	TCCR1A |= (1<<COM1A1);	//NO INVERTIDO
	TCCR1A |= (1<<WGM10);	//MODO FAST	
	
	TCCR1B |= (1<<WGM12);	//
	TCCR1B |= (1<<CS12)|(1<<CS10);	//PRESCALER DE 1024
	
	
	
		
	initADC();
	sei(); //Activar interrupciones
	
    
}



/*
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
	
}*/


ISR(ADC_vect){
	valorADC = ADCH;
	//PORTD = ADCH;
	ADCSRA |= (1<<ADIF);	
}


