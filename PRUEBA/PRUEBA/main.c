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
#include <avr/interrupt.h>

void setup(void);
void initADC(void);




//int valorADC;
int valorADC2;




int main(void)
{
    setup();
	ADCSRA |= (1<<ADSC);
	
    while (1) 
    {	
				
			
			OCR2A = valorADC2;
			
			
			
				
	}

}


void setup(void){
	cli();
	
	DDRB = 0b11111111;   // Todo como salidas
	PORTB = 0b00000000;		
	
	
	DDRD = 0b11111111;  //Salida de DISPLAY
	PORTD =0b00000000;	//DISPLAY apagados	
	
	DDRC = 0b00111111;  //Salida de led
	PORTC =0b00000000;	//Iniciamos los leds apagados
	
	
	
	//TIMER 2
	TCCR2A = 0;
	TCCR2B = 0;
		
	TCCR2A |= (1<<COM2A1);	//NO INVERTIDO
	TCCR2A |= (1<<WGM21)|(1<<WGM20);	//MODO FAST
		
	TCCR2B &= ~(1<<WGM22);	//
	TCCR2B |= (1<<CS22)|(1<<CS21)|(1<<CS20);	//PRESCALER DE 1024
	
	
	
		
	initADC();
	sei(); //Activar interrupciones
	
    
}




void initADC(void){
	
	ADMUX = 6;
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


ISR(ADC_vect){
	valorADC2 = ADCH;
	
	
	ADCSRA |= (1<<ADIF);
	ADCSRA |= (1<<ADSC);
}

