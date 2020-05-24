#include "hostMotor.h"
#include <stdlib.h>
#include <stdio.h>

int currentPeriod;

MemoryCommands setPeriod(uint16_t period){
	uint16_t valueToSet = period;
	if (valueToSet < 100){
		valueToSet = 100;
	}
	else if (valueToSet >0)
	{
	// TODO: 
	// I don't know why we must always set period to such a high value. 
	// Our "standard" motors tend to vibrate on this setting of PWM 
	// maybe the driver goes too hot under the max 2.5A load at this high frequency?
		valueToSet = 65000;
//		valueToSet = 10000; 
	}
        currentPeriod = valueToSet; 
   
        vector<tmemoryCommand> v { 
  	  mkAssign(TA0CCR0, valueToSet),
	  mkAssign(TA0CCR1, valueToSet -1),
	  mkAssign(TA0CCR2, valueToSet -1),
	  mkAssign(TA0CCR3, valueToSet -1),
	  mkAssign(TA0CCR4, valueToSet -1),
#ifndef OLIMEXINO_5510
          mkClearBits(PCOUT, 0x0030),
#endif
          mkClearBits(PBOUT, 0x0003), 
	  mkClearBits(PJOUT, 0x000F)
	}; 
	return MemoryCommands(v);
}

  vector<tmemoryCommand> MSPMotor::mkSetDutyPercent(int percent) const {
    uint16_t valueToSet;
    uint16_t clearMask;
    uint16_t setMask;
    if ((percent>0)&&(percent<100)) {
      valueToSet = (100 - percent) * currentPeriod / 100;
      clearMask = forwardMask | backwardMask; 
      setMask = forwardMask; 
    } else if (( percent<0 )&&(percent>-100)){
      percent = -percent;
      valueToSet = currentPeriod *(100 - percent) / 100;
      clearMask = forwardMask | backwardMask;
      setMask = backwardMask;
    } else if (percent == 0) {
      setMask = forwardMask | backwardMask;
      clearMask = 0; 
      valueToSet = currentPeriod - 1; 
    } else if (percent == 100) { 
      valueToSet = 1;
      clearMask = forwardMask | backwardMask;
      setMask = forwardMask;
    } else if (percent == -100) { 
      valueToSet = 1;
      clearMask = forwardMask | backwardMask;
      setMask = forwardMask;
    } else if ((percent < -100) | (percent >100)){
      valueToSet = 1;
      setMask = forwardMask | backwardMask;
      clearMask = 0;
    } else abort(); 

    vector<tmemoryCommand> v {
        mkAssign(timerRegister, valueToSet),
        mkClearBits(portOutRegister, clearMask),
	mkSetBits(portOutRegister, setMask)
    };
    return v;
  }



