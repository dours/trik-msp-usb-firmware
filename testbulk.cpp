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

// a console test for the MSPOverUSB to be run on the TRIK or on a PC

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

int main() try { 

 MSPOverUSB& msp { MSPOverUSB::get() };

 uint32_t prevseqno; 
  for (int iteration = 0; iteration < 1000000; ++iteration) { 
#if 0
    auto buf = msp.askMSP(); 
    checkAtomicity(iteration, prevseqno, buf.seqno, "seqno");
#else
    for (int j = -100; j < 100; j += 10) { 
      msp.setMotorPowers(MotorHelper().setPower(1, j ).setPower(0, j ).setPower(2, j ).setPower(3, j ).finish()); 
//      msp.setMotorPowers(MotorHelper().setPower(2, j ).setPower(3, j).finish()); 
      usleep(1000000); 
      auto buf = msp.askMSP(); 
      printf("N%i seq %04x motor protection  ", iteration, buf.seqno);
      for (int j = 0; j < 4; ++j)  printf("%04x  ", buf.hardwareProtectionCounters[j]); 
      printf("ENC  ");
      for (int j = 0; j < 4; ++j)  printf("%04x  ", buf.encoders[j]); 
      printf("  ADC  "); 
      for (int j = 0; j < 11; ++j) printf("%04x  ", buf.rawAnalogValues[j]); 
      printf("\n"); 
    } 
#endif
  }
    return 0; 
} 
catch (runtime_error e) { fprintf(stderr, "runtime_error(%s)\n", e.what()); }
catch (libusb_exception e) { fprintf(stderr, "libusb_exception %i %s\n", e.error, libusb_strerror(static_cast<libusb_error>(e.error))); } 


