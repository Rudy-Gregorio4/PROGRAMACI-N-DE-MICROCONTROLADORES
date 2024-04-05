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
void delay(uint8_t ciclos);
int count = 0;


int main(void)
{
    setup();
	
    while (1) 
    {
			//count++;
			//_delay_ms(1000);
			//PORTB = (count >> 2) & 0b00110000; // Salida de los bits 4 y 5 del PORTB para la variable COUNT
			//PORTC = count & 0b00111111;       // Salida de los primeros 6 bits del PORTC para la variable COUNT
			/*if (count > 255){
				count = 0;	
			}
			if (count == -1){
				count = 254;
			}
				
			PORTB = (count >> 2) & 0b00110000; // Salida de los bits 4 y 5 del PORTB para la variable COUNT
			PORTC = count & 0b00111111;       // Salida de los primeros 6 bits del PORTC para la variable COUNT*/
				
	}

}


void setup(void){
	DDRB = 0b11111100;   // PB0 y PB1 como entradas, el resto como salidas
	PORTB = 0b00000011;		//Pull Up en PB0 y PB1
	
	
	DDRD = 0b11111111;  //Salida de DISPLAY
	PORTD =0b00000000;	//DISPLAY apagados
	
	DDRC = 0b00111111;  //Salida de led
	PORTC =0b00000000;	//Iniciamos los leds apagados
	
	
	PCMSK0 |= (1<<PCINT0)|(1<<PCINT1); //PCINT0, PCINT1
	PCICR |= (1<<PCIE0); //Mascara de interrupción
	sei(); //Activar interrupciones
	
    
}

void delay(uint8_t ciclos){
	for(uint8_t j = 0; j < ciclos; j++){
		for(uint8_t i = 0; i < 255; i++){
			
		}
	}
}

ISR(PCINT0_vect){
	
	
	if(((PINB) & (1<<0)) == 0){
		count ++;
		if(count >= 255){
			count = 255;
		}
			PORTB = (count >> 2) & 0b00110000; // Salida de los bits 4 y 5 del PORTB para la variable COUNT
			PORTC = count & 0b00111111;       // Salida de los primeros 6 bits del PORTC para la variable COUNT
	}
	
	if(((PINB) & (1<<1)) == 0){
		count --;
		if(count <= 0){
			count = 0;
		}
			PORTB = (count >> 2) & 0b00110000; // Salida de los bits 4 y 5 del PORTB para la variable COUNT
			PORTC = count & 0b00111111;       // Salida de los primeros 6 bits del PORTC para la variable COUNT
	}
}




/*
ISR(PCINT0_vect){
	
	//if(((PINB) & (1<<0)) == 0)
	
	if((PINB&(1<<PINB0))==0)	{
		count++;		
	}
	
	if((PINB&(1<<PINB1))==0)	{
		count--;
	}
		
	//PORTB = (count >> 2) & 0b00110000; // Salida de los bits 4 y 5 del PORTB para la variable COUNT 
	//PORTC = count & 0b00111111;       // Salida de los primeros 6 bits del PORTC para la variable COUNT 
	
	
	
	//PCINT0_vect();
}*/