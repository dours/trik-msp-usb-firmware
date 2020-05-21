#include "memoryCommand.hpp"
#include <stdlib.h>

int currentPeriod;

MemoryCommands setPeriod(uint16_t period){
        currentPeriod = period; 
	uint16_t valueToSet = period;
	if (valueToSet < 100){
		valueToSet = 100;
	}
	else if (valueToSet >0)
	{
		valueToSet = 65000;
	}
   
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

class MSPMotor { 
  uint16_t timerRegister;
  uint16_t forwardMask, backwardMask; 
  uint16_t portOutRegister;
public:

  MSPMotor(uint16_t timerRegister, uint16_t forwardMask, uint16_t backwardMask, uint16_t portOutRegister) :
    timerRegister(timerRegister), forwardMask(forwardMask), 
    backwardMask(backwardMask), portOutRegister(portOutRegister) {} 

  vector<tmemoryCommand> setDutyPercent(int percent) {
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
};

MSPMotor m0(TA0CCR1, PCOUT, (1 << 4), (1 << 5));
MSPMotor m1(TA0CCR2, PBOUT, (1 << (0 + 8)), (1 << (1 + 8)));
MSPMotor m2(TA0CCR3, PJOUT, (1 << 1), (1 << 0));
MSPMotor m3(TA0CCR4, PJOUT, (1 << 2), (1 << 3)); 


