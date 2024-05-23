//*****************************************************************************
// Universidad del Valle de Guatemala
// Programación de Microcontroladores
// Archivo: Proyecto
// Hardware: ATMEGA328P
// Autor: Rudy Gregorio
// Carnet: 22127

//*****************************************************************************

// Define la frecuencia del CPU a 16 MHz
#define F_CPU 16000000UL

// Bibliotecas necesarias
#include <avr/io.h>         // Biblioteca para manejo de entradas/salidas
#include <util/delay.h>     // Biblioteca para funciones de retardo
#include <avr/interrupt.h>  // Biblioteca para manejo de interrupciones

// Inclusión de archivos de encabezado específicos para PWM, ADC y UART
#include "PWM/PWM0.h"
#include "PWM/PWM1.h"
#include "PWM/PWM2.h"
#include "ADC/ADC.h"
#include "UART/UART.h"

// Definición de canales de PWM
#define channel2A 0 // Ajustar según el valor correcto
uint8_t DutyC1 = 0;  // Duty cycle para el primer canal de PWM
uint8_t DutyC2 = 0;  // Duty cycle para el segundo canal de PWM
uint8_t DutyC3 = 0;  // Duty cycle para el tercer canal de PWM
uint8_t DutyC4 = 0;  // Duty cycle para el cuarto canal de PWM

// Variables globales para almacenamiento de valores
uint8_t pyvalue;
uint8_t Caracter;
uint8_t servos = 1;  // Indica cuál servo está siendo controlado
float angulo = 0;    // Ángulo para el servo


volatile int state = 0;  // Variable de estado para la máquina de estados

// Prototipos de funciones
void setup(void);
void canal(char pyvalue);
unsigned char read_EEPROM(unsigned int uiAddress);
void write_EEPROM(unsigned int uiAddress, unsigned char ucData);

int main(void) {
    cli();  // Deshabilitar interrupciones globales durante la configuración
    setup();  // Configuración de pines, PWM y ADC
    sei();  // Habilitar interrupciones globales
    
    while (1) {
        // Control de LEDs basado en el estado actual
        switch (state) {
            // Modo ADC y escritura en EEPROM
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

            // Lectura de EEPROM
            case 1:
                PORTC = (1 << PC1); // Enciende el LED en PC1
                // Leer los valores de la EEPROM y actualizar los duty cycles
                break;

            // Comunicación UART
            case 2:
                PORTC = (1 << PC0) | (1 << PC1); // Enciende todos los LEDs
                if (servos == 1) {
                    DutyC1 = angulo;
                    updateDutyCA1(angulo);
                    _delay_ms(10);
                } else if (servos == 2) {
                    DutyC2 = angulo;
                    updateDutyCA2(DutyC2);
                    _delay_ms(10);
                } else if (servos == 3) {
                    DutyC3 = angulo;
                    updateDutyCB1(DutyC3);
                    _delay_ms(10);
                } else if (servos == 4) {
                    DutyC4 = angulo;
                    updateDutyCB2(DutyC4);
                    _delay_ms(10);
                }
                break;

            // Manejo de estado por defecto
            default:
                state = 0; // Reinicia el estado si es un valor inesperado
                break;
        }
    }
}

void setup(void) {
    // Configuración de botones como entradas con pull-ups
    DDRD &= ~((1 << DDD6) | (1 << DDD7) | (1 << DDD4)| (1 << DDD2)); // PD6, PD7, PD4 y PD2 como entradas
    PORTD |= (1 << PORTD6) | (1 << PORTD7) | (1 << PORTD4)| (1 << PORTD2); // Activar pull-ups en PD6, PD7, PD4 y PD2

    DDRB &= ~((1 << DDB0) | (1 << DDB4)| (1 << DDB5)); // PB0, PB4 y PB5 como entradas
    PORTB |= (1 << PORTB0) | (1 << PORTB4)| (1 << PORTB5); // Activar pull-ups en PB0, PB4 y PB5

    // Configuración de pines de LEDs como salidas
    DDRC |= (1 << DDC0) | (1 << DDC1) | (1 << DDC2); // PC0, PC1 y PC2 como salidas
    PORTC &= ~((1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2)); // Iniciar LEDs apagados

    // Configuración de PWM y ADC
    initADC(); // Inicializa el ADC
    initPWM1A(no_invertido, 8, 39999); // Inicializa PWM en el canal 1A
    initPWM1B(no_invertido, 8, 39999); // Inicializa PWM en el canal 1B
    initPWM2A(no_invertido, 1024); // Inicializa PWM en el canal 2A
    initPWM2B(no_invertido, 1024); // Inicializa PWM en el canal 2B
    initUART9600(); // Inicializa UART a 9600 baudios

    // Configuración de interrupciones de pin cambio para PD7 (PCINT23)
    PCICR |= (1 << PCIE2); // Habilitar interrupciones de PCINT[23:16]
    PCMSK2 |= (1 << PCINT23)| (1 << PCINT20)| (1 << PCINT18); // Habilitar interrupción en PD7, PD4 y PD2

    // Configuración de interrupciones de pin cambio para PB0, PB4 y PB5 (PCINT0, PCINT4 y PCINT5)
    PCICR |= (1 << PCIE0); // Habilitar interrupciones de PCINT[7:0]
    PCMSK0 |= (1 << PCINT0) | (1 << PCINT4)| (1 << PCINT5); // Habilitar interrupción en PB0, PB4 y PB5

    // Deshabilitar TX y RX (comentado porque no es necesario en esta configuración)
    // UCSR0B = 0; // Deshabilitar TX y RX
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

void canal(char pyvalue) {
    // Selección del servo basado en el valor recibido
    switch(pyvalue) {
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

void aaangulo(char pyvalue) {
    // Asignación del ángulo basado en el valor recibido
    switch (pyvalue) {
        case '0': angulo = 0; break;
        case '1': angulo = 10; break;
        case '2': angulo = 20; break;
        case '3': angulo = 30; break;
        case '4': angulo = 40; break;
        case '5': angulo = 50; break;
        case '6': angulo = 60; break;
        case '7': angulo = 70; break;
        case '8': angulo = 80; break;
        case '9': angulo = 90; break;
        case 'a': angulo = 100; break;
        case 'b': angulo = 110; break;
        case 'c': angulo = 120; break;
        case 'd': angulo = 130; break;
        case 'e': angulo = 140; break;
        case 'f': angulo = 150; break;
        case 'g': angulo = 160; break;
        case 'h': angulo = 170; break;
        case 'i': angulo = 180; break;
        case 'j': angulo = 190; break;
        case 'k': angulo = 200; break;
        case 'l': angulo = 210; break;
        case 'm': angulo = 220; break;
        case 'n': angulo = 230; break;
        case 'o': angulo = 240; break;
        case 'p': angulo = 250; break;
        case 'q': angulo = 255; break;
        default: break;
        _delay_ms(5);
    }
}

// Interrupción de pin cambio para PD7, PD4 y PD2
ISR(PCINT2_vect) {
    if ((PIND & (1 << PIND7)) == 0) {
        _delay_ms(50); // Debounce delay
        if ((PIND & (1 << PIND7)) == 0) { // Verificar si el botón sigue presionado
            state++;
            if (state > 2) {
                state = 0; // Resetear estado si sobrepasa el límite
            }
        }
    } else if ((PIND & (1 << PIND4)) == 0) {
        PORTC = (1 << PC2); // Enciende LED en PC2
        if (state == 0) {
            // Escribe valores de los duty cycles actuales en la EEPROM
            write_EEPROM(0, DutyC1);
            write_EEPROM(1, DutyC2);
            write_EEPROM(2, DutyC3);
            write_EEPROM(3, DutyC4);
        } else if (state == 1) {
            // Lee valores de la EEPROM y actualiza los duty cycles
            uint8_t readDutyC1 = read_EEPROM(0);
            updateDutyCA1(readDutyC1);
            uint8_t readDutyC2 = read_EEPROM(1);
            updateDutyCA2(readDutyC2);
            uint8_t readDutyC3 = read_EEPROM(2);
            updateDutyCB1(readDutyC3);
            uint8_t readDutyC4 = read_EEPROM(3);
            updateDutyCB2(readDutyC4);
        }
    } else if ((PIND & (1 << PIND2)) == 0) {
        if (state == 0) {
            PORTC = (1 << PC2); // Enciende LED en PC2
            // Escribe valores de los duty cycles actuales en la EEPROM
            write_EEPROM(12, DutyC1);
            write_EEPROM(13, DutyC2);
            write_EEPROM(14, DutyC3);
            write_EEPROM(15, DutyC4);
        } else if (state == 1) {
            PORTC = (1 << PC2); // Enciende LED en PC2
            // Lee valores de la EEPROM y actualiza los duty cycles
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

// Interrupción de pin cambio para PB0, PB4 y PB5
ISR(PCINT0_vect) {
    if ((PINB & (1 << PINB0)) == 0) {
        PORTC = (1 << PC2); // Enciende LED en PC2
        if (state == 0) {
            // Escribe valores de los duty cycles actuales en la EEPROM
            write_EEPROM(4, DutyC1);
            write_EEPROM(5, DutyC2);
            write_EEPROM(6, DutyC3);
            write_EEPROM(7, DutyC4);
        } else if (state == 1) {
            // Lee valores de la EEPROM y actualiza los duty cycles
            uint8_t readDutyC1 = read_EEPROM(4);
            updateDutyCA1(readDutyC1);
            uint8_t readDutyC2 = read_EEPROM(5);
            updateDutyCA2(readDutyC2);
            uint8_t readDutyC3 = read_EEPROM(6);
            updateDutyCB1(readDutyC3);
            uint8_t readDutyC4 = read_EEPROM(7);
            updateDutyCB2(readDutyC4);
        }
    } else if ((PINB & (1 << PINB4)) == 0) {
        PORTC = (1 << PC2); // Enciende LED en PC2
        if (state == 0) {
            // Escribe valores de los duty cycles actuales en la EEPROM
            write_EEPROM(8, DutyC1);
            write_EEPROM(9, DutyC2);
            write_EEPROM(10, DutyC3);
            write_EEPROM(11, DutyC4);
        } else if (state == 1) {
            // Lee valores de la EEPROM y actualiza los duty cycles
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
ISR(USART_RX_vect) {
    pyvalue = UDR0; // Almacenar los datos recibidos en el buffer.
    Caracter = UDR0;
    canal(pyvalue);
    aaangulo(pyvalue);
}
