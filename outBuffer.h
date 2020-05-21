#ifndef OUT_BUFFER_H
#define OUT_BUFFER_H

// This file declares a structure of the output buffer 
// The structure is mapped directly onto the output buffer X
// of the USB IN Endpoint 1.
// This means that 
// 1. if a host reads this USB device then it always gets a packet
//    with current values of the structure
// 2. all writes to the structure must be atomic, because 
//    the host access is not synchronized 
//    (TODO: It could be though -- by setting the NAK bit before
// each access and clearing it afterwards. Maybe this is reasonable)

#include <stdint.h>
#include "power_motor.h"

// how many ADC channels do we pass to the host 
#define ADC_CHANNELS_SAMPLED (11)

struct OutBuffer {
  uint16_t adcBuffer[ADC_CHANNELS_SAMPLED];
  uint16_t seqno; 
  Encoder  encoders[4]; 
};

extern volatile struct OutBuffer* const theOutBuffer; // = IEP2_X_BUFFER_ADDRESS   


#endif

