/*
 * power_motor.hpp
 *
 *  Created on: Aug 22, 2013
 *      Author: roma
 */

#ifndef POWER_MOTOR_HPP_
#define POWER_MOTOR_HPP_
#include <stdint.h>

// Note: true encoder values are 32 bits, but they must be deduced by the host
// it is not that hard, because encoders for our standard motors tick on the order
// of 1khz speed, so, the host just need to read the values approx once in a second
// to check if any value becomes smaller (and increment higher bits accordingly)
typedef uint16_t Encoder; 


struct PwmMotor{
        uint16_t period;
        int8_t percent;
        uint16_t hardwareDefence;
};

int initPowerMotor();

int setPeriod(uint16_t period);
int setDutyPercent(uint8_t number,int8_t percent);
int setAllMotors(uint8_t *data);

void encoderInit();
void resetEncoder(const uint8_t number);
uint32_t getEncoderValue(const uint8_t number);
uint16_t 	getHardwareDefence(const uint8_t number);
#endif /* POWER_MOTOR_HPP_ */