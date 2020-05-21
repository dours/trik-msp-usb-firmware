#ifndef MEMORY_COMMAND_HPP
#define MEMORY_COMMAND_HPP

#include <cassert>
#include <stdint.h>
#include <vector>
#include <cstring>

extern "C" {
#include "memoryCommand.h"
#include "msp430f5510_symbols.h"
}

class MemoryCommands {
  int size; 
  uint16_t* data;

public: 
  MemoryCommands(vector<tmemoryCommand>& v)  {
    static_assert(sizeof(tmemoryCommand) % sizeof(uint16_t) == 0); 
    size = sizeof(uint16_t) + v.size() * sizeof(tmemoryCommand); 
    assert(size <= 64); 
    data = new uint16_t[size / sizeof(uint16_t)]; 
    data[0] = HEADER_MAGIC;
    memcpy(data + 1, v.data(), v.size() * sizeof(tmemoryCommand)); 
  }
  ~MemoryCommands() { delete[] data; } 
  unsigned char* getBuf() const { return reinterpret_cast<unsigned char*>(data); } 
  int getSize() const { return size; } 
};


tmemoryCommand mkSetBits(uint16_t dst, uint16_t value);
tmemoryCommand mkClearBits(uint16_t dst, uint16_t value);
tmemoryCommand mkAssign(uint16_t dst, uint16_t value);


#endif

