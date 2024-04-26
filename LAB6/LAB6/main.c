//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Programaci�n de Microcontroladores
// Autor: Rudy Gregorio
// Carn�: 22127
// Proyecto: Laboratorio 6
// Hardware: Atmega238p
// Creado: 18/04/2024
//******************************************************************************


#define F_CPU 16000000 // Definir la frecuencia del reloj (16 MHz en este caso).

#include <avr/io.h>    // Incluir biblioteca est�ndar para control de I/O.
#include <util/delay.h> // Incluir biblioteca para funciones de retardo.
#include <avr/interrupt.h> // Incluir biblioteca para manejo de interrupciones.
#include <stdio.h> // Incluir biblioteca est�ndar de C para funciones de entrada/salida.

// Variables globales vol�tiles para uso en interrupciones y funciones principales.
volatile uint8_t bufferTX; // Buffer para almacenar el �ltimo byte recibido por UART.
volatile uint16_t valorADC; // Variable para almacenar el resultado de 10 bits del ADC.

// Declaraci�n de funciones
void initUART9600(void);
void writeUART(char Caracter);
void writeTextUART(const char* Texto);
void initADC(void);
void setupPorts(void);

int main(void)
{
	initUART9600(); // Inicializar UART a 9600 bps.
	initADC(); // Inicializar el ADC.
	setupPorts(); // Configurar puertos B y C como salidas.
	sei();  // Habilitar interrupciones globales.

	while (1) // Bucle principal infinito.
	{
		writeTextUART("1. Leer Potenciometro\r\n"); // Opci�n para leer el valor del potenci�metro.
		writeTextUART("2. Enviar ASCII\r\n"); // Opci�n para enviar caracteres ASCII.
		writeTextUART("Ingrese su eleccion: "); // Pedir al usuario que ingrese su elecci�n.

		uint8_t choice = 0; // Variable para almacenar la elecci�n del usuario.
		while (!choice) // Esperar hasta que se reciba una elecci�n v�lida.
		{
			if (bufferTX != 0) // Verificar si se ha recibido un nuevo dato por UART.
			{
				char receivedChar = bufferTX; // Guardar el caracter recibido.
				bufferTX = 0; // Limpiar el buffer.
				if (receivedChar == '1' || receivedChar == '2') // Verificar si la elecci�n es v�lida.
				{
					choice = receivedChar - '0'; // Convertir el caracter a n�mero.
				}
			}
		}

		switch (choice) // Procesar la elecci�n del usuario.
		{
			case 1:
			ADCSRA |= (1 << ADSC);  // Iniciar la conversi�n del ADC estableciendo ADSC.
			while (ADCSRA & (1 << ADSC));  // Esperar a que la conversi�n se complete.
			valorADC = ADC;  // Leer el valor del ADC.

			char buffer[30];
			snprintf(buffer, sizeof(buffer), "Valor ADC: %u\r\n", valorADC); // Formatear el valor del ADC como string.
			writeTextUART(buffer); // Enviar el valor formateado por UART.
			break;

			case 2:
			writeTextUART("Ingrese el caracter ASCII: "); // Pedir al usuario que ingrese un caracter ASCII.
			while (bufferTX == 0);  // Esperar a que se ingrese un caracter.
			char asciiChar = bufferTX; // Almacenar el caracter recibido.
			bufferTX = 0; // Limpiar el buffer.
			PORTB = asciiChar & 0x3F; // Asignar los 6 bits menos significativos al puerto B.
			PORTC = (asciiChar >> 6) & 0x03; // Asignar los 2 bits m�s significativos al puerto C.
			writeUART(asciiChar); // Enviar el caracter de vuelta por UART.
			writeTextUART("\r\n"); // Enviar un salto de l�nea por UART.
			break;

			default:
			writeTextUART("Elecci�n invalida. Intente nuevamente.\r\n"); // Mensaje para elecci�n inv�lida.
			break;
		}
	}
}

void initUART9600(void)
{
	UBRR0 = 103;  // Configurar el registro UBRR0 para 9600 bps con F_CPU de 16MHz.
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);  // Habilitar receptor, transmisor y la interrupci�n de recepci�n completa.
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // Configurar formato de 8 bits de datos, sin paridad y 1 bit de parada.
}

void writeTextUART(const char* Texto)
{
	while (*Texto) // Mientras haya caracteres por enviar.
	{
		while (!(UCSR0A & (1 << UDRE0))); // Esperar a que el buffer de transmisi�n est� vac�o.
		UDR0 = *Texto++; // Enviar el siguiente caracter.
	}
}

void writeUART(char Caracter)
{
	while (!(UCSR0A & (1 << UDRE0))); // Esperar a que el buffer de transmisi�n est� vac�o.
	UDR0 = Caracter; // Enviar el caracter.
}

void initADC(void)
{
	// Configurar el ADC para usar AVCC como referencia y ajustar el resultado a la derecha.
	ADMUX = (1 << REFS0) | 7; // Seleccionar ADC7 como entrada.
	// Habilitar el ADC, su interrupci�n y establecer el preescalado a 128 para un reloj de ADC adecuado.
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void setupPorts(void)
{
	DDRB = 0xFF;  // Configurar todos los pines del puerto B como salidas.
	DDRC = 0xFF;  // Configurar todos los pines del puerto C como salidas.
}

// ISR para la conversi�n completa del ADC.
ISR(ADC_vect)
{
	valorADC = ADC; // Almacenar el resultado del ADC en la variable global.
}

// ISR para la recepci�n completa de USART.
ISR(USART_RX_vect)
{
	bufferTX = UDR0; // Almacenar los datos recibidos en el buffer.
}
