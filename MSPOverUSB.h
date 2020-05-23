#pragma once 
#include "outBuffer.h"
#include <array>
#include <vector>
#include <libusb-1.0/libusb.h>
#include "hostMotor.h"

struct libusb_exception { 
  int const error; 
  bool const open_with_vid_pid_failed; 
};

// This is a packet of all sensor values from the MSP
// You can ask the MSP for the whole packet only, not for particular specific sensor values 
struct RawSensorValues {
  // raw values for analog ports A1-A6 are at indexes 0-5 of the returned array
  const std::array<uint16_t, ADC_CHANNELS_SAMPLED> rawAnalogValues;
  // all the analog ports are sampled (once per port) then this counter is incremented
  // and the sampling restarts
  const uint32_t seqno; 
  const std::array<uint32_t, N_POWER_MOTOR> encoders;
  const std::array<uint32_t, N_POWER_MOTOR> hardwareProtectionCounters;

  // getters for the named analog ports 
  uint16_t getRawTemperature() const { return rawAnalogValues[10]; }
  uint16_t getRawBatteryVoltage() const { return rawAnalogValues[6]; }
  uint16_t getRawMotor2Current()  const { return rawAnalogValues[7]; } 
  uint16_t getRawLCDxP() const { return rawAnalogValues[8]; } 
  uint16_t getRawLCDyP() const { return rawAnalogValues[9]; } 

};
 
class MotorHelper { 
  std::array<std::pair<bool, int8_t>, N_POWER_MOTOR> theArray;
public:
  MotorHelper() { for (auto& x : theArray) x.first = false; } 
  MotorHelper& setPower(int n, int8_t pow) {
    theArray.at(n) = std::pair<bool, uint8_t>(true, pow);
    return *this;
  }
  std::array<std::pair<bool, int8_t>, N_POWER_MOTOR> finish() const { return theArray; }
};

class MSPOverUSB { 

  MSPOverUSB();
  ~MSPOverUSB(); 
  MSPOverUSB(MSPOverUSB const &) = delete;
  MSPOverUSB& operator = (MSPOverUSB const&) = delete; 

  libusb_context* usbContext;
  libusb_device_handle* mspHandle; 
  struct OutBuffer recv; 
  std::array<uint32_t, N_POWER_MOTOR> encoderZeroes;
  std::array<uint32_t, N_POWER_MOTOR> hardwareProtectionCountersZeroes;

  std::array<MSPMotor, N_POWER_MOTOR> const motors;

public:

  static MSPOverUSB& get() {
    static MSPOverUSB instance;
    return instance;
  }

  RawSensorValues askMSP();

  // request new values and then clear some of the counters
  // then indexes of counters to clear are given in [encoders], [hwps]
  RawSensorValues askMSPAndResetCounters(vector<int> const& encoders, vector<int> const& hwps);


  void setMotorPowers(std::array<std::pair<bool, int8_t>, N_POWER_MOTOR> const& ); 

};



  
