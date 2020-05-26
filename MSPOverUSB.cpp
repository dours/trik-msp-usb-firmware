
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



#include "MSPOverUSB.h"

#include <libusb-1.0/libusb.h>
#include <cstdio>
#include <unistd.h>
#include <string.h>
#include "memoryCommand.hpp"
#include "hostMotor.h"
#include "outBuffer.h"
#include "math.h"
using namespace std; 


MSPOverUSB::MSPOverUSB() :
  log(false), adc10(false), adc12(false), 
  motors { 
    MSPMotor(TA0CCR1, (1 << 4), (1 << 5), PCOUT), 
    MSPMotor(TA0CCR2, (1 << (0 + 8)), (1 << (1 + 8)), PBOUT), 
    MSPMotor(TA0CCR3, (1 << 1), (1 << 0), PJOUT),
    MSPMotor(TA0CCR4, (1 << 2), (1 << 3), PJOUT) 
  }
{ 
  int error = libusb_init(&usbContext); 
  if (error) throw libusb_exception { error, false }; 
  mspHandle = libusb_open_device_with_vid_pid(usbContext, 0x2047, 0x0310);
  if (mspHandle) { adc10 = true; } 
  else {
    mspHandle = libusb_open_device_with_vid_pid(usbContext, 0x2047, 0x0328);
    adc12 = true; 
  }
  if (!mspHandle) {
    libusb_exit(usbContext);
    throw libusb_exception { 0, true };
  }
  error = libusb_claim_interface(mspHandle, 0);
  if (error) { 
    libusb_close(mspHandle);
    libusb_exit(usbContext);
    throw libusb_exception { error, false };
  }
  for (auto& x : encoderZeroes) x = 0;
  for (auto& x : hardwareProtectionCountersZeroes) x = 0; 
  setPeriod(1000).libusbSend(mspHandle, log); 
}

MSPOverUSB::~MSPOverUSB() { 
    libusb_close(mspHandle);
    libusb_exit(usbContext);
}

RawSensorValues MSPOverUSB::askMSP() { 

    int transferred = -1; 
    // the protocol: first send an empty packet, then try to read 
    unsigned char buf[1]; 
    int error = libusb_bulk_transfer(mspHandle, 0x01, buf, 0, &transferred, 100);
    if (error) throw libusb_exception { error, false }; 
    error = libusb_bulk_transfer(mspHandle, 0x81, (unsigned char*)&recv, sizeof(OutBuffer), &transferred, 100); 
    if (error) throw libusb_exception { error, false }; 
    if (!(sizeof(OutBuffer) == transferred)) {
      throw runtime_error("a packet with a wrong size received"); 
    }
    auto es = encoderZeroes;
    for (unsigned i = 0; i < es.size(); ++i) es[i] = recv.encoders[i] - es[i]; 
    auto hwc = hardwareProtectionCountersZeroes;
    for (unsigned i = 0; i < hwc.size(); ++i) hwc[i] = recv.hardwareProtectionCounters[i] - hwc[i]; 
    array<uint16_t, ADC_CHANNELS_SAMPLED> adc;
    if (adc10) {
        for (unsigned  i = 6; i < adc.size(); ++i) 
          adc[i] = recv.adcBuffer[adc.size() - i - 1] << 2; // this is to mimic 12-bit format 
	for (unsigned i = 0; i < 6; ++i) 
	  adc[i] = recv.adcBuffer[i + adc.size() - 6]; 
      } else { 
        memcpy(&adc, recv.adcBuffer, sizeof(adc)); 
      }
    if (recv.adcOverflowHappened) throw runtime_error("ADC overflow happened"); 
    return RawSensorValues{ adc, recv.seqno, es, hwc }; 
}

RawSensorValues MSPOverUSB::askMSPAndResetCounters(vector<int> const& encoders, vector<int> const& hwps) {
  auto newValues = askMSP();
  for (int n : encoders) encoderZeroes.at(n) = newValues.encoders.at(n);
  for (int n : hwps) hardwareProtectionCountersZeroes.at(n) = newValues.hardwareProtectionCounters.at(n);
  return newValues;
}

void MSPOverUSB::setMotorPowers(std::array<std::pair<bool, int8_t>, N_POWER_MOTOR> const& powerValues) {
  vector<tmemoryCommand> commands;
  for (unsigned n = 0; n < powerValues.size(); ++n) if (powerValues[n].first) {
    vector<tmemoryCommand> v { motors[n].mkSetDutyPercent(powerValues[n].second) };
    commands.insert(commands.end(), v.begin(), v.end()); 
  }
  MemoryCommands(commands).libusbSend(mspHandle, log); 
}



