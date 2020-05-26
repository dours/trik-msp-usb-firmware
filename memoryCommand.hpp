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

// Some helpers to create a script of memory commands on a host 
// and send the script to an MSP for execution:
// create commands with mk*, push them into a vector v, 
// call MemoryCommands(v).libusbSend

#ifndef MEMORY_COMMAND_HPP
#define MEMORY_COMMAND_HPP

#include <cassert>
#include <stdint.h>
#include <vector>
#include <cstring>
#include <libusb-1.0/libusb.h>
using namespace std;

extern "C" {
#include "memoryCommand.h"
#include "msp430f5510_symbols.h"
}

class MemoryCommands {
  int size; 
  uint16_t* data;

  MemoryCommands(MemoryCommands const&); 

public: 
  MemoryCommands(MemoryCommands&& x) : size(x.size), data(x.data) {} 
  MemoryCommands(vector<tmemoryCommand> v); 
  void libusbSend(libusb_device_handle* handle, bool printError); 
  ~MemoryCommands() { delete[] data; } 
  unsigned char* getBuf() const { return reinterpret_cast<unsigned char*>(data); } 
  int getSize() const { return size; } 
};


tmemoryCommand mkSetBits(uint16_t dst, uint16_t value);
tmemoryCommand mkClearBits(uint16_t dst, uint16_t value);
tmemoryCommand mkAssign(uint16_t dst, uint16_t value);


#endif

