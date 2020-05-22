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
  int libusbSend(libusb_device_handle* handle, bool printError); 
  ~MemoryCommands() { delete[] data; } 
  unsigned char* getBuf() const { return reinterpret_cast<unsigned char*>(data); } 
  int getSize() const { return size; } 
};


tmemoryCommand mkSetBits(uint16_t dst, uint16_t value);
tmemoryCommand mkClearBits(uint16_t dst, uint16_t value);
tmemoryCommand mkAssign(uint16_t dst, uint16_t value);


#endif

