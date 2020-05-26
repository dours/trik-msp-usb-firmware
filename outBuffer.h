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




#ifndef OUT_BUFFER_H
#define OUT_BUFFER_H

// This file declares a structure of the output buffer.
// Each time a host asks us, we send a buffer back through endpoint 1

#include <stdint.h>

// how many ADC channels do we pass to the host 
#define ADC_CHANNELS_SAMPLED (11) 
#define N_POWER_MOTOR 4

struct OutBuffer {
  uint16_t adcBuffer[ADC_CHANNELS_SAMPLED];
  uint16_t adcOverflowHappened; 
  uint32_t seqno; 
  uint32_t  encoders[N_POWER_MOTOR]; 
  uint32_t hardwareProtectionCounters[N_POWER_MOTOR]; 
};

extern volatile struct OutBuffer theOutBuffer;

#endif

