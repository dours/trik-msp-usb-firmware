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

