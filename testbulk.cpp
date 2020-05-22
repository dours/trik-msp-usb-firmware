
#include <libusb-1.0/libusb.h>
#include <cstdio>
#include <unistd.h>
#include "memoryCommand.hpp"
#include "hostMotor.h"
#include "outBuffer.h"
using namespace std; 

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


int main() { 
  vector<tmemoryCommand> v { mkSetBits(PAOUT, 1 << 4) };
  MemoryCommands set1(v);
  vector<tmemoryCommand> u { mkClearBits(PAOUT, 1 << 4) } ;
  MemoryCommands clr1(u); 


  libusb_context* context;
  libusb_init(&context); 
  libusb_device_handle* handle = libusb_open_device_with_vid_pid(context, 0x2047, 0x0301);
  assert(handle); 
  assert(0 == libusb_claim_interface(handle, 0));
  uint16_t prevseqno; 
  setPeriod(1000).libusbSend(handle, false); 
  for (int iteration = 0; iteration < 100000; ++iteration) { 
    int transferred = -1; 
#if 1
    OutBuffer buf; 
    int error = libusb_bulk_transfer(handle, 0x81, (unsigned char*)&buf, sizeof(OutBuffer), &transferred, 100); 
    if (0 != error) fprintf(stderr, "error = %i\n", error); 
    assert(0 == error); 
    assert(sizeof(OutBuffer) == transferred);
#if 0
    printf("N%i seq %04x motor protection  ", iteration, buf.seqno);
    for (int j = 0; j < 4; ++j)  printf("%04x  ", buf.hardwareProtectionCounters[j]); 
    printf("  ADC  "); 
    for (int j = 0; j < 11; ++j) printf("%04x  ", buf.adcBuffer[j]); 
    printf("\n"); 
    usleep(100000); 
#else
    if (0 != iteration) if (!((prevseqno + 1 == buf.seqno) || prevseqno == buf.seqno)) {
      printf("iter %i, prevseqno %04x   buf.seqno %04x\n", iteration, prevseqno , buf.seqno);
//      assert(false);
    }
    prevseqno = buf.seqno; 
#endif
#endif
    #if 1
    for (int j = 0; j < 1; ++j) { 
      //printf("%i\n", j %100); 
      MemoryCommands(m1.setDutyPercent(j%100)).libusbSend(handle, false); 
      //usleep(300000); 
    } 
    #endif
  }
    return 0; 
}

