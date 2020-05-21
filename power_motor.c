/*
 * power_motor2.c
 *
 *  Created on: Mar 29, 2014
 *      Author: roma
 */

//#include <assert.h>
#include <msp430.h>
#include <msp430f5528.h>
#include "power_motor.hpp"

#define MAX_POWER_MOTOR 4

volatile uint32_t hardwareDefense[4];

struct Encoder{
	int32_t value;
	int8_t type;
};
volatile struct Encoder encoders[4];


void encoderInit(){
	P1DIR &= ~(BIT0|BIT6);
	P2DIR &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
	P5DIR |= (BIT3);
	P5OUT |= (BIT3);

	P1IES	&= ~(BIT0|BIT6);
	P1IE	|= 	(BIT0|BIT6);
	P1IFG	&=	~(BIT0|BIT6);

	P2IES	&= ~(BIT0|BIT1|BIT4|BIT5);
	P2IE	|= 	(BIT0|BIT1|BIT4|BIT5);
	P2IFG	&=	~(BIT0|BIT1|BIT4|BIT5);

}
void resetEncoder(const uint8_t number){
	encoders[number].value = 0;
}
uint32_t getEncoderValue(const uint8_t number){
	uint32_t value = encoders[number].value;
	return value;
}
uint16_t 	getHardwareDefence(const uint8_t number){
	uint16_t value = 0;
	//assert(number<MAX_POWER_MOTOR && number>=0);
	value = hardwareDefense[number];
	hardwareDefense[number] = 0;
	return  value;
}
int initPowerMotor(){
	//A1P A1M
	P5SEL &= ~(BIT4|BIT5);
	P5OUT &= ~(BIT4|BIT5);
	P5DIR |= (BIT4|BIT5);
	//B1P B1M
	P4SEL &= ~(BIT0|BIT1);
	P4OUT &= ~(BIT0|BIT1);
	P4DIR |= (BIT0|BIT1);
	//A2P A2M B2P B2M
	PJOUT &= ~(BIT0|BIT1|BIT2|BIT3);
	PJDIR |= (BIT0|BIT1|BIT2|BIT3);
	//PJSEL &= ~(BIT0|BIT1|BIT2|BIT3);

	// M_A1 M_A2
	P1DIR &= ~(BIT1|BIT7);
	P1SEL &= ~(BIT1|BIT7);
	//a low-to-high transition interrupt
	P1IES &= ~(BIT1|BIT7);
	//The interrupt is enabled
	P1IE |= (BIT1|BIT7);

	// M_B1 M_B2
	P2DIR &= ~(BIT6|BIT7);
	P2SEL &= ~(BIT6|BIT7);

	P2IES &= ~(BIT6|BIT7);
	//The interrupt is enabled
	P2IE |= (BIT6|BIT7);
	//Timer Init

	P1DIR |= (BIT2|BIT3|BIT4|BIT5);
	P1SEL |= (BIT2|BIT3|BIT4|BIT5);

	TA0CCR0 = 0;
	TA0CCR1 = 0 ;
	TA0CCTL1 = OUTMOD_6;
	TA0CCR2 =  0;
	TA0CCTL2 = OUTMOD_6;
	TA0CCR3 =  0;
	TA0CCTL3 = OUTMOD_6;
	TA0CCR4 =  0;
	TA0CCTL4 = OUTMOD_6;

//	setPeriod(0,1000);
//	setDutyPercent(0,50);
	TA0CTL = TASSEL_2 |ID_3| MC_3 ;
	TA0EX0 = TAIDEX_1;
	TA0CTL |= TACLR;
	return 0;
  }

int setPeriod(uint16_t period){
	uint16_t valueToSet = period;
	if (valueToSet < 100){
		valueToSet = 100;
	}
	else if (valueToSet >65000)
	{
		valueToSet = 65000;
	}
	TA0CCR0 = valueToSet;
	TA0CCR1 = valueToSet -1;
	TA0CCR2 = valueToSet -1;
	TA0CCR3 = valueToSet -1;
	TA0CCR4 = valueToSet -1;
	P5OUT &= ~(BIT4|BIT5);
	P4OUT &= ~(BIT0|BIT1);
	PJOUT &= ~(BIT0|BIT1|BIT2|BIT3);
	return 0;
}

int setDutyPercent(uint8_t number,int8_t percent){

	if(TA0CCR0>0){
		switch (number) {
			case 0:{
				if((percent>0)&&(percent <100)){
					uint16_t valueToSet = TA0CCR0/100 *(100-percent);
					TA0CCR1 = valueToSet;
					P5OUT &= ~(BIT4|BIT5);
					P5OUT |= (BIT4);
				}
				else if (( percent<0 )&&(percent>-100)){
					percent = -percent;
					uint16_t valueToSet = TA0CCR0/100 *(100 - percent);
					TA0CCR1 = valueToSet;
					P5OUT &= ~(BIT4|BIT5);
					P5OUT |= (BIT5);
				}
				else if (percent == 0)
				{
					P5OUT |= (BIT4|BIT5);
					//P5OUT |= (BIT5);
					TA0CCR1 = TA0CCR0-1;
				}
				else if (percent == 100)
				{
					TA0CCR1 = 1;
					P5OUT &= ~(BIT4|BIT5);
					P5OUT |= (BIT4);
				}
				else if (percent == -100)
				{
					TA0CCR1 = 1;
					P5OUT &= ~(BIT4|BIT5);
					P5OUT |= (BIT5);
				}
				else  if ((percent < -100) | (percent >100)){
					TA0CCR1 = 1;
					P5OUT |= (BIT4|BIT5);
				}
				break;
			}
			case 1:{
				if((percent>0)&&(percent <100)){
					uint16_t valueToSet = TA0CCR0/100 *(100-percent);
					TA0CCR2 = valueToSet;
					P4OUT &= ~(BIT0|BIT1);
					P4OUT |= (BIT0);
				}
				else if (( percent<0 )&&(percent>-100)){
					percent = -percent;
					uint16_t valueToSet = TA0CCR0/100 *(100 - percent);
					TA0CCR2 = valueToSet;
					P4OUT &= ~(BIT0|BIT1);
					P4OUT |= (BIT1);
				}
				else if (percent == 0)
				{
					P4OUT &= ~(BIT0|BIT1);
					//P5OUT |= (BIT5);
					TA0CCR2 = TA0CCR0-1;
				}
				else if (percent == 100)
				{
					TA0CCR2 = 1;
					P4OUT &= ~(BIT0|BIT1);
					P4OUT |= (BIT0);
				}
				else if (percent == -100)
				{
					TA0CCR2 = 1;
					P4OUT &= ~(BIT0|BIT1);
					P4OUT |= (BIT1);
				}
				else  if ((percent < -100) | (percent >100)){
					TA0CCR2 = 1;
					P4OUT |= (BIT0|BIT1);
				}
				break;
			}
			case 2:{
				if((percent>0)&&(percent <100)){
					uint16_t valueToSet = TA0CCR0/100 *(100-percent);
					TA0CCR3 = valueToSet;
					PJOUT &= ~(BIT0|BIT1);
					PJOUT |= (BIT1);
				}
				else if (( percent<0 )&&(percent>-100)){
					percent = -percent;
					uint16_t valueToSet = TA0CCR0/100 *(100 - percent);
					TA0CCR3 = valueToSet;
					PJOUT &= ~(BIT0|BIT1);
					PJOUT |= (BIT0);
				}
				else if (percent == 0)
				{
					PJOUT &= ~(BIT0|BIT1);
					//P5OUT |= (BIT5);
					TA0CCR3 = TA0CCR0-1;
				}
				else if (percent == 100)
				{
					TA0CCR3 = 1;
					PJOUT &= ~(BIT0|BIT1);
					PJOUT |= (BIT1);
				}
				else if (percent == -100)
				{
					TA0CCR3 = 1;
					PJOUT &= ~(BIT0|BIT1);
					PJOUT |= (BIT0);
				}
				else  if ((percent < -100) | (percent >100)){
					TA0CCR3 = 1;
					PJOUT |= (BIT0|BIT1);
				}
				break;
			}
			case 3:{
				if((percent>0)&&(percent <100)){
					uint16_t valueToSet = TA0CCR0/100 *(100-percent);
					TA0CCR4 = valueToSet;
					PJOUT &= ~(BIT2|BIT3);
					PJOUT |= (BIT2);
				}
				else if (( percent<0 )&&(percent>-100)){
					percent = -percent;
					uint16_t valueToSet = TA0CCR0/100 *(100 - percent);
					TA0CCR4 = valueToSet;
					PJOUT &= ~(BIT2|BIT3);
					PJOUT |= (BIT3);
				}
				else if (percent == 0)
				{
					PJOUT &= ~(BIT2|BIT3);
					//P5OUT |= (BIT5);
					TA0CCR4 = TA0CCR0-1;
				}
				else if (percent == 100)
				{
					TA0CCR4 = 1;
					PJOUT &= ~(BIT2|BIT3);
					PJOUT |= (BIT2);
				}
				else if (percent == -100)
				{
					TA0CCR4 = 1;
					PJOUT &= ~(BIT2|BIT3);
					PJOUT |= (BIT3);
				}
				else  if ((percent < -100) | (percent >100)){
					TA0CCR4 = 1;
					PJOUT |= (BIT2|BIT3);
				}
				break;
			}
		}
	}
	return 0;
}

int setAllMotors(uint8_t *data)
{
    uint8_t i = 0;
	for(; i < MAX_POWER_MOTOR; ++i)
	{
		setDutyPercent(i, data[i]);
	}
	return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR (void){
	switch(P1IV){
//		case P1IV_NONE:
//			break;
//		case P1IV_P1IFG0:
//			break;
		case P1IV_P1IFG1:
			hardwareDefense[0]++;
			break;
//		case P1IV_P1IFG2:
//			break;
//		case P1IV_P1IFG3:
//			break;
//		case P1IV_P1IFG4:
//			break;
//		case P1IV_P1IFG5:
//			break;
//		case P1IV_P1IFG6:
//			break;
		case P1IV_P1IFG7:
			hardwareDefense[3]++;
			break;
		default:
			break;
	}

}
#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void){
	switch(P2IV){
		case P2IV_NONE:
			break;
		case P2IV_P2IFG0:
			if ((P2IN & BIT3)==BIT3)
				++encoders[0].value;
			else
				--encoders[0].value;
//			P2IES ^= (BIT0);
			P2IFG &= ~(BIT0);
			break;
		case P2IV_P2IFG1:
			if ((P1IN & BIT6)==BIT6)
				++encoders[3].value;
			else
				--encoders[3].value;
//			P2IES ^= (BIT1);
			P2IFG &= ~(BIT1);
			break;
//		case P2IV_P1IFG2:
//			break;
//		case P2IV_P1IFG3:
//			break;
		case P2IV_P2IFG4:
			if ((P1IN & BIT0)==BIT0)
				++encoders[1].value;
			else
				--encoders[1].value;
//			P2IES ^= (BIT4);
			P2IFG &= ~(BIT4);
			break;
		case P2IV_P2IFG5:
			if ((P2IN & BIT2)==BIT2)
				++encoders[2].value;
			else
				--encoders[2].value;
//			P2IES ^= (BIT5);
			P2IFG &= ~(BIT5);
			break;
		case P2IV_P2IFG6:
			hardwareDefense[1]++;
			break;
		case P2IV_P2IFG7:
			hardwareDefense[3]++;
			break;
		default:
			break;
		}
}
