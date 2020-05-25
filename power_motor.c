/*
 * power_motor2.c
 *
 *  Created on: Mar 29, 2014
 *      Author: roma
 */

#include <msp430.h>
#include "power_motor.h"
#include "outBuffer.h"


volatile uint32_t* encoders; 

void encoderInit(){
        encoders = theOutBuffer.encoders;
	P1SEL &= ~(BIT0|BIT6);
	P1DIR &= ~(BIT0|BIT6);
#ifndef OLIMEXINO_5510
	P2SEL &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
	P2DIR &= ~(BIT0|BIT1|BIT2|BIT3|BIT4|BIT5);
	P5SEL &= ~BIT3; 
	P5DIR |= (BIT3);
	P5OUT |= (BIT3);
#endif

	P1IES	&= ~(BIT0|BIT6);
	P1IE	|= 	(BIT0|BIT6);
	P1IFG	&=	~(BIT0|BIT6);

#ifndef OLIMEXINO_5510
	P2IES	&= ~(BIT0|BIT1|BIT4|BIT5);
	P2IE	|= 	(BIT0|BIT1|BIT4|BIT5);
	P2IFG	&=	~(BIT0|BIT1|BIT4|BIT5);
#endif
}

int initPowerMotor(){
	//A1P A1M
#ifndef OLIMEXINO_5510
	P5SEL &= ~(BIT4|BIT5);
	P5OUT &= ~(BIT4|BIT5);
	P5DIR |= (BIT4|BIT5);
#endif
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


#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = PORT1_VECTOR
__interrupt void P1_ISR (void)
#else 
void __attribute__ ((interrupt(PORT1_VECTOR))) P1_ISR (void)
#endif
{
	switch(P1IV){
		case P1IV_P1IFG1:
			theOutBuffer.hardwareProtectionCounters[0]++;
			break;
		case P1IV_P1IFG7:
			theOutBuffer.hardwareProtectionCounters[3]++;
			break;
		default:
			break;
	}

}

#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = PORT2_VECTOR
__interrupt void P2_ISR (void)
#else 
void __attribute__ ((interrupt(PORT2_VECTOR))) P2_ISR (void)
#endif
{
	switch(P2IV){
		case P2IV_NONE:
			break;
		case P2IV_P2IFG0:
			if ((P2IN & BIT3)==BIT3)
				++encoders[0];
			else
				--encoders[0];
//			P2IES ^= (BIT0);
			P2IFG &= ~(BIT0);
			break;
		case P2IV_P2IFG1:
			if ((P1IN & BIT6)==BIT6)
				++encoders[3];
			else
				--encoders[3];
//			P2IES ^= (BIT1);
			P2IFG &= ~(BIT1);
			break;
		case P2IV_P2IFG4:
			if ((P1IN & BIT0)==BIT0)
				++encoders[1];
			else
				--encoders[1];
//			P2IES ^= (BIT4);
			P2IFG &= ~(BIT4);
			break;
		case P2IV_P2IFG5:
			if ((P2IN & BIT2)==BIT2)
				++encoders[2];
			else
				--encoders[2];
//			P2IES ^= (BIT5);
			P2IFG &= ~(BIT5);
			break;
		case P2IV_P2IFG6:
			theOutBuffer.hardwareProtectionCounters[1]++;
			break;
		case P2IV_P2IFG7:
			theOutBuffer.hardwareProtectionCounters[3]++;
			break;
		default:
			break;
		}
}
