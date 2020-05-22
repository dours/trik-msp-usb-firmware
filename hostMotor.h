#ifndef HOST_MOTOR_H
#define HOST_MOTOR_H

#include "memoryCommand.hpp"

MemoryCommands setPeriod(uint16_t period);

class MSPMotor { 
  uint16_t timerRegister;
  uint16_t forwardMask, backwardMask; 
  uint16_t portOutRegister;
public:

  MSPMotor(uint16_t timerRegister, uint16_t forwardMask, uint16_t backwardMask, uint16_t portOutRegister) :
    timerRegister(timerRegister), forwardMask(forwardMask), 
    backwardMask(backwardMask), portOutRegister(portOutRegister) {} 

  vector<tmemoryCommand> setDutyPercent(int percent);

};

#endif

