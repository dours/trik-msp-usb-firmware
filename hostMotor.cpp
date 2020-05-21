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
#ifndef OLIMEXINO_5510
	P5OUT &= ~(BIT4|BIT5);
#endif
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
#ifndef OLIMEXINO_5510
					P5OUT &= ~(BIT4|BIT5);
					P5OUT |= (BIT4);
#endif
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

