
/* Copyright 2020 Oleg Medvedev and CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */


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
	// Well, no, I haven't noticed this with a 4ohm load 
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


// This is moved with minimal changes from the original power_motor.c (of the I2C version)
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



