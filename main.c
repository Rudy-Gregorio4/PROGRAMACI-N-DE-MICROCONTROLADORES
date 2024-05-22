//*****************************************************************************
// Universidad del Valle de Guatemala
// Programación de Microcontroladores
// Archivo: Proyecto
// Hardware: ATMEGA328P
// Autor: Rudy Gregorio
// Carnet: 22127

// PRUEBA

//*****************************************************************************

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "PWM/PWM0.h"
#include "PWM/PWM1.h"
#include "PWM/PWM2.h"
#include "ADC/ADC.h"

#define channel2A 0 // Ajustar según el valor correcto


uint8_t DutyC1 = 0;
uint8_t DutyC2 = 0;
uint8_t DutyC3 = 0;
uint8_t DutyC4 = 0;


// Variables necesarias del primer código
uint8_t garra = 0, garra1 = 0, brazo = 0, brazo1 = 0, codo = 0, codo1 = 0, rota = 0, rota1 = 0;
uint8_t receivedChar = 0;
uint8_t activa = 0, activa2 = 0, state3 = 0, state5 = 0;

// Declaración de funciones del primer código
void convertServo(uint8_t value, uint8_t channel);
void convertServo2(uint8_t value, uint8_t channel);

volatile int state = 0;

void setup(void);
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

                PORTC = (1 << PC0) | (1 << PC1); // Enciende todos los LEDs

                if (state == 0) {
	                state++;
	                } else if (state == 1) {
	                if (receivedChar != 0 || (PINC & (1 << PINC4)) || (PINC & (1 << PINC5)) || state5 == 1) {
		                state5 = 1;

		                if (garra >= 255) {
			                garra = 254;
			                } else if (garra <= 1) {
			                garra = 0;
		                }

		                if ((PINC & (1 << PINC4))) {
			                _delay_ms(15);
			                receivedChar = 'A';
			                } else if ((PINC & (1 << PINC5))) {
			                _delay_ms(15);
			                receivedChar = 'B';
			                } else if ((PINB & (1 << PINB0)) == 0) {
			                receivedChar = 'R';
		                }

		                while ((PINB & (1 << PINB0)) == 0) {
			                _delay_ms(30);
		                }

		                switch (receivedChar) {
			                case 'A':
			                state2 = 1;
			                garra += 10;
			                convertServo2(garra, channel2A);
			                receivedChar = 0;
			                break;
			                case 'B':
			                state2 = 1;
			                garra -= 10;
			                convertServo2(garra, channel2A);
			                receivedChar = 0;
			                break;
			                case 'R':
			                garra1 = garra;
			                state++;
			                receivedChar = 0;
			                break;
		                }
	                }
	                } else if (state == 2) {
	                activa5 = 0;
	                if (receivedChar != 0 || (PINC & (1 << PINC4)) || (PINC & (1 << PINC5)) || (PINB & (1 << PINB0)) == 0) {
		                if (garra1 >= 255) {
			                garra1 = 254;
			                } else if (garra1 <= 1) {
			                garra1 = 0;
		                }

		                if ((PINC & (1 << PINC4))) {
			                receivedChar = 'A';
			                _delay_ms(15);
			                } else if ((PINC & (1 << PINC5))) {
			                receivedChar = 'B';
			                _delay_ms(15);
			                } else if ((PINB & (1 << PINB0)) == 0) {
			                receivedChar = 'R';
		                }

		                while ((PINB & (1 << PINB0)) == 0) {
			                _delay_ms(30);
		                }

		                switch (receivedChar) {
			                case 'A':
			                garra1 += 10;
			                convertServo2(garra1, channel2A);
			                receivedChar = 0;
			                break;
			                case 'B':
			                garra1 -= 10;
			                convertServo2(garra1, channel2A);
			                receivedChar = 0;
			                break;
			                case 'R':
			                activa++;
			                receivedChar = 0;
			                break;
		               }
	               }
               }
               // Agregar los demás casos de 'activa' aquí siguiendo la misma estructura
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

    // Configuración de interrupciones de pin cambio para PD7 (PCINT23)
    PCICR |= (1 << PCIE2); // Habilitar interrupciones de PCINT[23:16]
    PCMSK2 |= (1 << PCINT23)| (1 << PCINT20)| (1 << PCINT18); // Habilitar interrupción en PD7 y PD4

    // Configuración de interrupciones de pin cambio para PB0 y PB4 (PCINT0 y PCINT4)
    PCICR |= (1 << PCIE0); // Habilitar interrupciones de PCINT[7:0]
    PCMSK0 |= (1 << PCINT0) | (1 << PCINT4)| (1 << PCINT5); // Habilitar interrupción en PB0, PB4 y PB5

    // Deshabilitar TX y RX
    UCSR0B = 0; // Deshabilitar TX y RX
}

unsigned char read_EEPROM(unsigned int uiAddress) {
    // Esperar la finalización de la escritura anterior
    while (EECR & (1 << EEPE));
    
    // Configurar el registro de dirección
    EEAR = uiAddress;
    
    // Iniciar la lectura de la EEPROM escribiendo EERE
    EECR |= (1 << EERE);
    
    // Devolver los datos del registro de datos
    return EEDR;
}

void write_EEPROM(unsigned int uiAddress, unsigned char ucData) {
    // Esperar la finalización de la escritura anterior
    while (EECR & (1 << EEPE));
    
    // Configurar los registros de dirección y datos
    EEAR = uiAddress;
    EEDR = ucData;
    
    // Escribir un uno lógico en EEMPE
    EECR |= (1 << EEMPE);
    
    // Iniciar la escritura de la EEPROM estableciendo EEPE
    EECR |= (1 << EEPE);
}

// Interrupción de pin cambio para PD7
ISR(PCINT2_vect) {
    if ((PIND & (1 << PIND7)) == 0) {
        _delay_ms(50); // Debounce delay
        if ((PIND & (1 << PIND7)) == 0) { // Verificar si el botón sigue presionado
            state++;
            if (state > 2) {
                state = 0; // Resetear estado si sobrepasa el límite
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
 