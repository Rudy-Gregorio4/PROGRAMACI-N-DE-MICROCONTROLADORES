
#include "PWM0.h"

void initPWM0A(uint8_t inverted, uint16_t prescaler){
	DDRD |= (1 << DDD6); //PD6 COMO SALIDA
	TCCR0A |= (1 << WGM01)|(1 << WGM00); //PWM MODO FAST
	TIMSK0 = (1<<TOIE0);
	
	if (inverted) {
		TCCR0A |= (1 << COM0A1)|(1 << COM0A0);//PWM INVERTIDO
		} else	 {
		TCCR0A |= (1 << COM0A1);//PWM NO INVERTIDO
	}

	if (prescaler == 1024) {
		TCCR0B |= (1 << CS02)|(1 << CS00);
		} else {
		TCCR0B |= (1 << CS00);
	}

}

void initPWM0B(uint8_t inverted, uint16_t prescaler){
	DDRD |= (1 << DDD5); //PD5 COMO SALIDA
	TCCR0A |= (1 << WGM01)|(1 << WGM00); //PWM MODO FAST
	TIMSK0 = (1<<TOIE0);
	
	if (inverted) {
		TCCR0A |= (1 << COM0B1)|(1 << COM0B0);//PWM INVERTIDO
		} else	 {
		TCCR0A |= (1 << COM0B1);//PWM NO INVERTIDO
	}
	
	if (prescaler == 1024) {
		TCCR0B |= (1 << CS02)|(1 << CS00);
		} else {
		TCCR0B |= (1 << CS00);
	}
}

void updateDutyCA(uint8_t duty){
	OCR0A = ((duty/255.0)*30)+6;
}

void updateDutyCB(uint8_t duty){
	OCR0B = ((duty/255.0)*30)+6;
}

