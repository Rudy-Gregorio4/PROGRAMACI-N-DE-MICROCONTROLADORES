


#ifndef PWM0_H_
#define PWM0_H_

#include <avr/io.h>
#include <stdint.h>

#define invertido 1
#define no_invertido 0

void initPWM0A(uint8_t inverted, uint16_t prescaler);

void initPWM0B(uint8_t inverted, uint16_t prescaler);

void updateDutyCA(uint8_t duty);

void updateDutyCB(uint8_t duty);

#endif /* PWM0_H_ */