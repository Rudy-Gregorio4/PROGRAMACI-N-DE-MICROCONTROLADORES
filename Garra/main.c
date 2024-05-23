//*****************************************************************************
// Universidad del Valle de Guatemala
// Programaci?n de Microcontroladores
// Archivo: Proyecto
// Hardware: ATMEGA328P
// Autor: Rudy Gregorio
// Carnet: 22127

//*****************************************************************************

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "PWM/PWM0.h"
#include "PWM/PWM1.h"
#include "PWM/PWM2.h"
#include "ADC/ADC.h"
#include "UART/UART.h"

#define channel2A 0 // Ajustar según el valor correcto
uint8_t DutyC1 = 0;
uint8_t DutyC2 = 0;
uint8_t DutyC3 = 0;
uint8_t DutyC4 = 0;


uint8_t pyvalue;
uint8_t Caracter;

uint8_t servos = 1;

float angulo = 0;

// Variables necesarias del primer código
uint8_t garra = 0, brazo = 0, codo = 0, rota = 0;
uint8_t receivedChar = 0;
uint8_t activa = 0, activa2 = 0, state3 = 0, state5 = 0, state2 = 0;
volatile int state = 0;
 


void setup(void);
void canal(char pyvalue);
unsigned char read_EEPROM(unsigned int uiAddress); // Declaración de la función read_EEPROM
void write_EEPROM(unsigned int uiAddress, unsigned char ucData); // Declaración de la función write_EEPROM

int main(void) {
	cli(); // Deshabilitar interrupciones globales durante la configuración
	setup(); // Configuración de pines, PWM y ADC
	sei(); // Habilitar interrupciones globales
	
	
	while (1) {
		// Control de LEDs basado en el estado
		switch (state) {
			// MODO ADC y ESCRITURA DE EEPROM
			case 0:
			PORTC = (1 << PC0); // Enciende el LED en PC0
			// Actualización de Duty Cycles para los servos
			_delay_ms(10);
			DutyC1 = ADC_CONVERT(6);
			updateDutyCA1(DutyC1);
			_delay_ms(10);
			DutyC2 = ADC_CONVERT(4);
			updateDutyCA2(DutyC2);
			_delay_ms(10);
			DutyC3 = ADC_CONVERT(5);
			updateDutyCB1(DutyC3);
			_delay_ms(10);
			DutyC4 = ADC_CONVERT(7);
			updateDutyCB2(DutyC4);
			break;

			// LECTURA DE EEPROM
			case 1:
			PORTC = (1 << PC1); // Enciende el LED en PC1
			// Leer los valores de la EEPROM y actualizar los duty cycles
			break;

			// UART
			case 2:
			PORTC = (1 << PC0) | (1 << PC1); // Enciende todos los LEDs
			
			if (servos == 1)
			{
				DutyC1 = angulo;
				updateDutyCA1(angulo);
				_delay_ms(10);
			}
			
			else if (servos == 2)
			{
				DutyC2 = angulo;
				updateDutyCA2(DutyC2);
				_delay_ms(10);
				
			}
			
			else if (servos == 3)
			{
				DutyC3 = angulo;
				updateDutyCB1(DutyC3);
				_delay_ms(10);
				
			}
			
			else if (servos == 4)
			{
				DutyC4 = angulo;
				updateDutyCB2(DutyC4);
				_delay_ms(10);
				
			}
					
	
			
			break;

			default:
			state = 0; // Reinicia el estado si es un valor inesperado
			break;
		}
	}
}

void setup(void) {
	// Configuración de botones como entradas con pull-ups
	DDRD &= ~((1 << DDD6) | (1 << DDD7) | (1 << DDD4)| (1 << DDD2)); // PD6, PD7 y PD4 como entradas
	PORTD |= (1 << PORTD6) | (1 << PORTD7) | (1 << PORTD4)| (1 << PORTD2); // Activar pull-ups en PD6, PD7 y PD4

	DDRB &= ~((1 << DDB0) | (1 << DDB4)| (1 << DDB5)); // PB0 y PB4 como entradas
	PORTB |= (1 << PORTB0) | (1 << PORTB4)| (1 << PORTB5); // Activar pull-ups en PB0 y PB4

	// Configuración de pines de LEDs como salidas
	DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2); // PC0, PC1 y PC2 como salidas
	PORTC &= ~((1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2)); // Iniciar LEDs apagados

	// Configuración de PWM y ADC
	initADC();
	initPWM1A(no_invertido, 8, 39999);
	initPWM1B(no_invertido, 8, 39999);
	initPWM2A(no_invertido, 1024);
	initPWM2B(no_invertido, 1024);
	initUART9600();

    // Configuraci?n de interrupciones de pin cambio para PD7 (PCINT23)
    PCICR |= (1 << PCIE2); // Habilitar interrupciones de PCINT[23:16]
    PCMSK2 |= (1 << PCINT23)| (1 << PCINT20)| (1 << PCINT18); // Habilitar interrupci?n en PD7 y PD4

    // Configuraci?n de interrupciones de pin cambio para PB0 y PB4 (PCINT0 y PCINT4)
    PCICR |= (1 << PCIE0); // Habilitar interrupciones de PCINT[7:0]
    PCMSK0 |= (1 << PCINT0) | (1 << PCINT4)| (1 << PCINT5); // Habilitar interrupci?n en PB0, PB4 y PB5

    // Deshabilitar TX y RX
    //UCSR0B = 0; // Deshabilitar TX y RX
}

unsigned char read_EEPROM(unsigned int uiAddress) {
    // Esperar la finalizaci?n de la escritura anterior
    while (EECR & (1 << EEPE));
    
    // Configurar el registro de direcci?n
    EEAR = uiAddress;
    
    // Iniciar la lectura de la EEPROM escribiendo EERE
    EECR |= (1 << EERE);
    
    // Devolver los datos del registro de datos
    return EEDR;
}

void write_EEPROM(unsigned int uiAddress, unsigned char ucData) {
    // Esperar la finalizaci?n de la escritura anterior
    while (EECR & (1 << EEPE));
    
    // Configurar los registros de direcci?n y datos
    EEAR = uiAddress;
    EEDR = ucData;
    
    // Escribir un uno l?gico en EEMPE
    EECR |= (1 << EEMPE);
    
    // Iniciar la escritura de la EEPROM estableciendo EEPE
    EECR |= (1 << EEPE);
}


void canal(char pyvalue){
	
	switch(pyvalue){
		
		case 'servo1':
		servos = 1;
		_delay_ms(5);
		break;
		
		case 'servo2':
		servos = 2;
		_delay_ms(5);
		break;
		
		case 'servo3':
		servos = 3;
		_delay_ms(5);
		break;
		
		case 'servo4':
		servos = 4;
		_delay_ms(5);
		break;
		
		default:
		break;
		
	}

}


void aaangulo(char pyvalue){
	
	
		switch (pyvalue)
		{
			case '0':
			angulo = 0;
			_delay_ms(5);
			break;
			
			case '1':
			angulo = 10;
			_delay_ms(5);
			break;
			
			case '2':
			angulo = 20;
			_delay_ms(5);
			break;
			
			case '3':
			angulo = 30;
			_delay_ms(5);
			break;
			
			case '4':
			angulo = 40;
			_delay_ms(5);
			break;
			
			case '5':
			angulo = 50;
			_delay_ms(5);
			break;
			
			case '6':
			angulo = 60;
			_delay_ms(5);
			break;
			
			case '7':
			angulo = 70;
			_delay_ms(5);
			break;
			
			case '8':
			angulo = 80;
			_delay_ms(5);
			break;
			
			case '9':
			angulo = 90;
			_delay_ms(5);
			break;
			
			case 'a':
			angulo = 100;
			_delay_ms(5);
			break;
			
			case 'b':
			angulo = 110;
			_delay_ms(5);
			break;
			
			case 'c':
			angulo = 120;
			_delay_ms(5);
			break;
			
			case 'd':
			angulo = 130;
			_delay_ms(5);
			break;
			
			case 'e':
			angulo = 140;
			_delay_ms(5);
			break;
			
			case 'f':
			angulo = 150;
			_delay_ms(5);
			break;
			
			case 'g':
			angulo = 160;
			_delay_ms(5);
			break;
			
			case 'h':
			angulo = 170;
			_delay_ms(5);
			break;
			
			case 'i':
			angulo = 180;
			_delay_ms(5);
			break;
			
			case 'j':
			angulo = 190;
			_delay_ms(5);
			break;
			
			case 'k':
			angulo = 200;
			_delay_ms(5);
			break;
			
			case 'l':
			angulo = 210;
			_delay_ms(5);
			break;
			
			case 'm':
			angulo = 220;
			_delay_ms(5);
			break;
			
			case 'n':
			angulo = 230;
			_delay_ms(5);
			break;
			
			case 'o':
			angulo = 240;
			_delay_ms(5);
			break;
			
			case 'p':
			angulo = 250;
			_delay_ms(5);
			break;
			
			case 'q':
			angulo = 255;
			_delay_ms(5);
			break;
			
		}
	
}






// Interrupci?n de pin cambio para PD7
ISR(PCINT2_vect) {
    if ((PIND & (1 << PIND7)) == 0) {
        _delay_ms(50); // Debounce delay
        if ((PIND & (1 << PIND7)) == 0) { // Verificar si el bot?n sigue presionado
            state++;
            if (state > 2) {
                state = 0; // Resetear estado si sobrepasa el l?mite
            }
        }
    }
    
    else if ((PIND & (1 << PIND4)) == 0) {
        
		PORTC = (1 << PC2);
            if (state == 0) {
                write_EEPROM(0, DutyC1);
                write_EEPROM(1, DutyC2);
				write_EEPROM(2, DutyC3);
				write_EEPROM(3, DutyC4);
            } else if (state == 1) {
                uint8_t readDutyC1 = read_EEPROM(0);
				updateDutyCA1(readDutyC1);
                uint8_t readDutyC2 = read_EEPROM(1);
				updateDutyCA2(readDutyC2);
				uint8_t readDutyC3 = read_EEPROM(2);
				updateDutyCB1(readDutyC3);
				uint8_t readDutyC4 = read_EEPROM(3);
				updateDutyCB2(readDutyC4);
                
                
            }
        
    }
	
	  else if ((PIND & (1 << PIND2)) == 0) {
		  
		  
		  if (state == 0) {
			  PORTC = (1 << PC2);
			  write_EEPROM(12, DutyC1);
			  write_EEPROM(13, DutyC2);
			  write_EEPROM(14, DutyC3);
			  write_EEPROM(15, DutyC4);
			  } else if (state == 1) {
			  PORTC = (1 << PC2);
			  uint8_t readDutyC1 = read_EEPROM(12);
			  updateDutyCA1(readDutyC1);
			  uint8_t readDutyC2 = read_EEPROM(13);
			  updateDutyCA2(readDutyC2);
			  uint8_t readDutyC3 = read_EEPROM(14);
			  updateDutyCB1(readDutyC3);
			  uint8_t readDutyC4 = read_EEPROM(15);
			  updateDutyCB2(readDutyC4);
		  }
	  }
}

ISR(PCINT0_vect) {
    if ((PINB & (1 << PINB0)) == 0) {
		PORTC = (1 << PC2);
       
            if (state == 0) {
                write_EEPROM(4, DutyC1);
                write_EEPROM(5, DutyC2);
                write_EEPROM(6, DutyC3);
                write_EEPROM(7, DutyC4);
            } else if (state == 1) {
                uint8_t readDutyC1 = read_EEPROM(4);
                updateDutyCA1(readDutyC1);
                uint8_t readDutyC2 = read_EEPROM(5);
                updateDutyCA2(readDutyC2);
                uint8_t readDutyC3 = read_EEPROM(6);
                updateDutyCB1(readDutyC3);
                uint8_t readDutyC4 = read_EEPROM(7);
                updateDutyCB2(readDutyC4);
            }
        
    }
    else if ((PINB & (1 << PINB4)) == 0) {
		PORTC = (1 << PC2);
       
            if (state == 0) {
                write_EEPROM(8, DutyC1);
                write_EEPROM(9, DutyC2);
                write_EEPROM(10, DutyC3);
                write_EEPROM(11, DutyC4);
            } else if (state == 1) {
                uint8_t readDutyC1 = read_EEPROM(8);
                updateDutyCA1(readDutyC1);
                uint8_t readDutyC2 = read_EEPROM(9);
                updateDutyCA2(readDutyC2);
                uint8_t readDutyC3 = read_EEPROM(10);
                updateDutyCB1(readDutyC3);
                uint8_t readDutyC4 = read_EEPROM(11);
                updateDutyCB2(readDutyC4);
            }
        
    }
 
}

// ISR para la recepción completa de USART.
ISR(USART_RX_vect)
{
	pyvalue = UDR0; // Almacenar los datos recibidos en el buffer.
	Caracter = UDR0;
	
	
	canal(pyvalue);
	aaangulo(pyvalue);
		
	
}
 
