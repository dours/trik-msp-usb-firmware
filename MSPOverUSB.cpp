#include "MSPOverUSB.h"

#include <libusb-1.0/libusb.h>
#include <cstdio>
#include <unistd.h>
#include "memoryCommand.hpp"
#include "hostMotor.h"
#include "outBuffer.h"
#include "math.h"
using namespace std; 


MSPOverUSB::MSPOverUSB() 
  m0(TA0CCR1, (1 << 4), (1 << 5), PCOUT), 
  m1(TA0CCR2, (1 << (0 + 8)), (1 << (1 + 8)), PBOUT), 
  m2(TA0CCR3, (1 << 1), (1 << 0), PJOUT),
  m3(TA0CCR4, (1 << 2), (1 << 3), PJOUT) 
{ 
  int error = libusb_init(&usbContext); 
  if (error) throw libusb_exception { error, false }; 
  mspHandle = libusb_open_device_with_vid_pid(context, 0x2047, 0x0301);
  if (!mspHandle) {
    libusb_exit(usbContext);
    throw libusb_exception { 0, true };
  }
  error = libusb_claim_interface(handle, 0);
  if (error) { 
    libusb_close_device(mspHandle);
    libusb_exit(usbContext);
    throw libusb_exception { error, false };
  }
  for (auto& x : encoderZeroes) x = 0;
  for (auto& x : hardwareProtectionCountersZeroes) x = 0; 
  setPeriod(1000).libusbSend(handle, false); 
}

MSPOverUSB::~MSPOverUSB() { 
    libusb_close_device(mspHandle);
    libusb_exit(usbContext);
}

RawSensorValues MSPOverUSB::askMSP() { 

    int transferred = -1; 
    // the protocol: first send an empty packet, then try to read 
    int error = libusb_bulk_transfer(handle, 0x01, (unsigned char*)&buf, 0, &transferred, 100);
    if (error) throw libusb_exception { error, false }; 
    error = libusb_bulk_transfer(handle, 0x81, (unsigned char*)&recv, sizeof(OutBuffer), &transferred, 100); 
    if (error) throw libusb_exception { error, false }; 
    if (!(sizeof(OutBuffer) == transferred)) {
      throw exception("a packet with a wrong size received"); 
    }
    auto es = encoderZeroes;
    for (int i = 0; i < es.size; ++i) es[i] = recv.encoders[i] - es[i]; 
    auto hwc = hardwareProtectionCountersZeroes;
    for (int i = 0; i < hwc.size; ++i) hwc[i] = recv.hardwareProtectionCounters[i] - hwc[i]; 

    return RawSensorValues(recv, es, hwc); 
}

RawSensorValues MSPOverUSB::askMSPAndResetCounters(vector<int> const& encoders, vector<int> const& hwps) {
  auto newValues = askMSP();
  for (int n : encoders) encoderZeroes.at(n) = newValues.encoders.at(n);
  for (int n : hwps) hardwareProtectionCountersZeroes.at(n) = newValues.hardwareProtectionCounters.at(n);
  return newValues;
}

void MSPOverUSB::setMotorPowers(std::array<std::pair<bool, int8_t>, N_POWER_MOTORS> const& powerValues) {
  vector<tmemoryCommand> commands;
  for (int n = 0; n < powerValues.size(); ++n) if (powerValues[n].first) {
    commands.append(motors[n].setDutyPercent(powerValues[n].second)); 
  }
  MemoryCommands(commands).libusbSend(handle, false); 
}



