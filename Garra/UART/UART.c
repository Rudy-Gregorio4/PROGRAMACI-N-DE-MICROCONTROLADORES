#include <avr/io.h>
#include <stdint.h>
#include "UART.h"

void initUART9600(void){

	//Paso 1: RX como entrada y TX como salida
	DDRD &= ~(1<<DDD0); //RX Como Entrada
	DDRD |= (1<<DDD1);    //TX Como Salida

	//Paso 2: Configurar UCSR0A

	UCSR0A = 0;                        //Resetear Registro

	//Paso 3: Configurar UCSR0B Habilitando ISR de recepción, rx y tx

	UCSR0B = 0;                                            //Resetear Registro
	UCSR0B |= (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);    //Habilitar Interrupciones de RX, Recepción, Transmisión

	//Paso 4: Configurar UCSR0C, Asincrono, Pariedad: None, 1 bit de Stop, Data bits: 8

	UCSR0C = 0;                                            //Resetear Registro
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);                //Colocar 8 bits de datos

	//Paso 5: Configurar velocidad de Baudrate: 9600

	UBRR0 = 103;
}

void writeUART(char Caracter){
	while(!(UCSR0A & (1<<UDRE0)));

	UDR0 = Caracter;
}

void writeTextUART(char* Texto){
	uint8_t i;
	for(i=0; Texto[i]!='\0'; i++){
		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = Texto[i];
	}

}