

#ifndef PWM1_H_
#define PWM1_H_

#include <avr/io.h>
#include <stdint.h>

#define invertido 1
#define no_invertido 0

float map(float, float, float, float, float);

void initPWM1A(uint8_t inverted, uint16_t prescaler, uint16_t top);

void initPWM1B(uint8_t inverted, uint16_t prescaler, uint16_t top);

void updateDutyCA1(uint8_t duty);

void updateDutyCB1(uint8_t duty);

#endif /* PWM1_H_ */