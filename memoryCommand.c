#include "memoryCommand.h"

void executeMemoryCommand(struct tmemoryCommand* c) {
  uint16_t* dst = (uint16_t*)c->address; 
  switch (c->kind) {

    case Assign:
      *dst = c->value;
      break;

    case SetBits:
      *dst |= c->value;
      break;

    case ClearBits:
      *dst &= ~(c->value);
      break;
  }
}

void executeMemoryCommandBuffer(void* buf, uint8_t size) {
  if (*(uint16_t*)buf != HEADER_MAGIC) return;
  size -= 2;

  struct tmemoryCommand* cmd = (struct tmemoryCommand*)(buf + sizeof(uint16_t)); 

  while (size >= sizeof(struct tmemoryCommand)) {
    executeMemoryCommand(cmd);
    ++cmd;
    size -= sizeof(struct tmemoryCommand);
  }
}

