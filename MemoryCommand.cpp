
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



#include "memoryCommand.hpp"
#include "MSPOverUSB.h"
#include <cstdio>

  MemoryCommands::MemoryCommands(vector<tmemoryCommand> v)  {
    static_assert(sizeof(tmemoryCommand) % sizeof(uint16_t) == 0, "sizeof(tmemoryCommand) % sizeof(uint16_t) == 0  "); 
    size = sizeof(uint16_t) + v.size() * sizeof(tmemoryCommand); 
    if (!(size <= 64)) throw runtime_error("this packet of commands does not fit to one 64-byte USB packet"); 
    data = new uint16_t[size / sizeof(uint16_t)]; 
    data[0] = HEADER_MAGIC;
    memcpy(data + 1, v.data(), v.size() * sizeof(tmemoryCommand)); 
  }

  void MemoryCommands::libusbSend(libusb_device_handle* handle, bool printError) {
    int transferred = -1; 
    if (printError) {
      printf("sending "); 
      for (int i = 0; i < getSize() / sizeof(uint16_t); ++i) printf("%04x  ", data[i]);
      printf("\n");
    }
    int error = libusb_bulk_transfer(handle, 0x01, getBuf(), getSize(), &transferred, 100); 
    if (printError && 0 != error) fprintf(stderr, "error = %i\n", error); 
    if (error) throw libusb_exception{error, false}; 
    if (getSize() != transferred) throw runtime_error("failed to send a whole USB packet. Why?"); 
  }


tmemoryCommand mkSetBits(uint16_t dst, uint16_t value) { 
  tmemoryCommand c { SetBits, dst, value }; 
  return c; 
}

tmemoryCommand mkClearBits(uint16_t dst, uint16_t value) { 
  tmemoryCommand c { ClearBits, dst, value }; 
  return c; 
}

tmemoryCommand mkAssign(uint16_t dst, uint16_t value) { 
  tmemoryCommand c { Assign, dst, value }; 
  return c; 
}

