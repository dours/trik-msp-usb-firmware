#ifndef MEMORY_COMMAND_H
#define MEMORY_COMMAND_H

#include <stdint.h>

// This header describes a generic memory command.
// We expect the host to send us arrays of such commands 
// via OUT USB packets to manage the motors or do something else 

// some random magic to skip erroneously sent commands 
#define HEADER_MAGIC (0xca73) 

enum tmemoryCommandKind { Assign, SetBits, ClearBits }; 
struct tmemoryCommand { 
  enum tmemoryCommandKind kind;
  uint16_t address;
  uint16_t value;
};

void executeMemoryCommand(struct tmemoryCommand*); 
void executeMemoryCommandBuffer(void* buf, uint8_t size); 


#endif

