//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Programación de Microcontroladores
// Autor: Rudy Gregorio
// Carné: 22127
// Proyecto: Laboratorio 4
// Hardware: Atmega238p
// Creado: 04/04/2024
//******************************************************************************


#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

void setup(void);
void multiplex(void);
void initADC(void);



int count = 0;
int valor;
int unidad;
int decena;

const uint8_t display[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0X6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};



int main(void)
{
    setup();
	
    while (1) 
    {	
		
			ADCSRA |= (1<<ADSC);	//
			_delay_ms(10);
			
			unidad = valor & 0x0F;	//SE UTILIZA UN AND PARA OBTENER LOS EL VALOR QUE TIENEN LOS BITS MENOS SIGNIFICATIVOS Y GUARDARLO EN LAS UNIDADES
			decena = (valor>>4) & 0x0F;	//SE UTILIZA UN AND PARA OBTENER LOS EL VALOR QUE TIENEN LOS BITS MÁS SIGNIFICATIVOS Y GUARDARLO EN LAS DECENAS
			
			if (count < valor)		//SI EL VALOR DEL ADC ES MAYOR QUE EL VALOR DE LOS BITS, ENCENDER LA ALARMA
			{
				PORTB |= (1<<PINB5);
			}
			else if (count >= valor)	//SI ES MENO O IGUAL QUE, APAGAR ALARMA
			{
				PORTB &= ~(1<<PINB5);
			}
			
			
			multiplex();
					
		
	}

}


void setup(void){
	cli();
	
	DDRB = 0b11111100;   // PB0 y PB1 como entradas, el resto como salidas
	PORTB = 0b00000011;		//Pull Up en PB0 y PB1
	
	
	DDRD = 0b11111111;  //Salida de DISPLAY
	PORTD =0b00000000;	//DISPLAY apagados
	
	/*DDRC = 0b00111111;  //Salida de led
	PORTC =0b00000000;	//Iniciamos los leds apagados*/
	
	
	PCMSK0 |= (1<<PCINT0)|(1<<PCINT1); //PCINT0, PCINT1
	PCICR |= (1<<PCIE0); //Mascara de interrupción
	UCSR0B = 0;
	
	initADC();
	sei(); //Activar interrupciones
	
    
}

void multiplex (void){
		
	
	
	PORTB |= (1<<PINB3);	//ENCENDER EL PRIMER TRANASISTOR PARA EL PRIMER DISPLAY
	PORTD = display[unidad];	//MOSTRAR EL VALOR ADC DE UNIDAD EN EL DISPLAY
	_delay_ms(5);		//DELAY DE MULTIPLEXACION
	PORTB &= ~(1<<PINB3);	//APAGAR EL TRANSISTOR PARA CONTINUAR
	
	
	
	PORTB |= (1<<PINB2);	//ENCENDER EL SEGUNDO TRANASISTOR PARA EL SEGUNDO DISPLAY
	PORTD = display[decena];	//MOSTRAR EL VALOR ADC DE DECENA EN EL DISPLAY
	_delay_ms(5);		//DELAY DE MULTIPLEXACION
	PORTB &= ~(1<<PINB2);	//APAGAR EL TRANSISTOR PARA CONTINUAR
	
	
	PORTB |= (1<<PINB4);	//ENCENDER EL PRIMER TRANASISTOR PARA EL PRIMER DISPLAY
	PORTD = count;
	_delay_ms(1);		//DELAY DE MULTIPLEXACION
	PORTB &= ~(1<<PINB4);	//APAGAR EL TRANSISTOR PARA CONTINUAR
	
	

}


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


ISR(ADC_vect){
	valor = ADCH;
	//PORTD = ADCH;
	ADCSRA |= (1<<ADIF);
	
}


ISR(PCINT0_vect){
	
	if((PINB&(1<<PINB1))==0)	{
		count++;
		if (count > 255){
			count = 0;	//OVERFLOW DEL CONTADOR DE 8 BITS
		}		
	}
	
	if((PINB&(1<<PINB0))==0)	{
		count--;
		if (count<0){
			count = 255;	//UNDERFLOW DEL CONTADOR DE 8 BITS
		}
	}
		
}