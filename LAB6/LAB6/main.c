//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Programación de Microcontroladores
// Autor: Rudy Gregorio
// Carné: 22127
// Proyecto: Laboratorio 6
// Hardware: Atmega238p
// Creado: 4/18/2024
//******************************************************************************


#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>



void initUART9600(void);
void writeUART (char Caracter);
void writeTextUART (char* Texto); //puntero
void readUART(uint8_t receivedChar);
volatile uint8_t bufferTX;


int main(void)
{
	initUART9600();
	sei();
	
	
	
	writeTextUART("Hola mundo AHHHHHHHHH =) ");
	writeUART(10);	//New line
	writeUART(13);	//Retorno
	
    while (1) 
    {	
		if (bufferTX != 0) {
			readUART(bufferTX);
			bufferTX = 0;
		}			
				
	}

}




void initUART9600(void){
	
	cli ();
	//PORTB COMO SALIDAS
	DDRB = 0b11111111;   // Todo como salidas
	PORTB = 0b00000000;
	
	//RX como entrada y TX como salida
	DDRD &= ~(1<<DDD0);
	DDRD |= (1<<DDD1);
	
	//Configurar UCSR0A 
	UCSR0A = 0;
	
	//Configuracion del UCSR0B: DONDE ISR de recepcion, habilitamos RX y TX
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0)| (1<<RXEN0)| (1<<TXEN0);
	
	//Configuracion UCSR0C: Asincrono, Paridad: none, 1 bit de Stop, Data bits = 8 bits
	UCSR0C = 0;
	
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00); //8 BITS
	
	//Configuracion de velocidad de Baudrate de 9600
	UBRR0 = 103;
		
}

void writeTextUART(char* Texto){
	
	uint8_t i;
	for (i=0; Texto[i] !='\0'; i++){			//Empieza en 0 y va hasta el valor que tenga "Texto" en la posicion "i" cuando ya sea nulo para poder aumentar dicho valor
		while (!(UCSR0A & (1<<UDRE0)));		//Se espera a que se vacie el buffer del UDR, si esta vacio se carga el texto
		UDR0 = Texto[i];						//Avanzar al siguiente caracter
	}
}


void writeUART(char Caracter){
	
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = Caracter;
}

void readUART(uint8_t receivedChar) {
	switch (receivedChar) {
		case '0':
		PORTB = 0;
		PORTB |= (1 << PORTB0);
		break;
		case '1':
		PORTB = 0;
		PORTB |= (1 << PORTB1);
		break;
		case '2':
		PORTB = 0;
		PORTB |= (1 << PORTB2);
		break;
		case '3':
		PORTB = 0;
		PORTB |= (1 << PORTB3);
		break;
		case '4':
		PORTB = 0;
		PORTB |= (1 << PORTB4);
		break;
		case '5':
		PORTB = 0;
		PORTB |= (1 << PORTB5);
		break;
		default:
		PORTB = 0xFF;
		_delay_ms(100);
		PORTB = 0x00;
		_delay_ms(100);
		break;
	}
}

// INTERRUPCION DE USART
ISR (USART_RX_vect){
	
	bufferTX = UDR0;
	UDR0 = bufferTX;
	readUART(UDR0);
}

