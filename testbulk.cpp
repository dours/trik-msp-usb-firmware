
#include <libusb-1.0/libusb.h>
#include <cstdio>
#include <unistd.h>
#include "MSPOverUSB.h"
using namespace std; 

void checkAtomicity(int iteration, uint32_t& prevseqno, uint32_t seqno, char const* name) {
    if (labs(seqno - prevseqno) > 5 ) {
      printf("iter %i, prev%s %04x   %s %04x\n", iteration, name, prevseqno, name, seqno);
//      assert(false);
    }
    prevseqno = seqno; 
}

int main() { 

 MSPOverUSB& msp { MSPOverUSB::get() };

 uint32_t prevseqno; 
  for (int iteration = 0; iteration < 1000000; ++iteration) { 
#if 1
    auto buf = msp.askMSP(); 
#if 0
    printf("N%i seq %04x motor protection  ", iteration, buf.seqno);
    for (int j = 0; j < 4; ++j)  printf("%04x  ", buf.hardwareProtectionCounters[j]); 
    printf("  ADC  "); 
    for (int j = 0; j < 11; ++j) printf("%04x  ", buf.adcBuffer[j]); 
    printf("\n"); 
    usleep(100000); 
#else
    checkAtomicity(iteration, prevseqno, buf.seqno, "seqno");
#endif
#endif
    #if 1
    for (int j = 0; j < 1; ++j) { 
      msp.setMotorPowers(MotorHelper().setPower(1, j % 100).finish()); 
      //usleep(300000); 
    } 
    #endif
  }
    return 0; 
}

