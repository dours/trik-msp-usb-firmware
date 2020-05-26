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


#ifndef MEMORY_COMMAND_H
#define MEMORY_COMMAND_H

#include <stdint.h>

// This header describes a generic memory command.
// We expect the host to send us arrays of such commands 
// via OUT USB packets to manage the motors or do something else 
// The header is used by MSP and a host both. 

// some random magic to skip erroneously sent commands 
#define HEADER_MAGIC (0xca73) 

// assign a new value to a reg, set some bits acc. to a mask, 
// or clear some bits
enum tmemoryCommandKind { Assign, SetBits, ClearBits }; 
struct tmemoryCommand { 
  uint16_t kind:2;
  uint16_t address:14;
  uint16_t value;
};

void executeMemoryCommand(struct tmemoryCommand*); 
void executeMemoryCommandBuffer(void* buf, uint8_t size); 


#endif

