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

// Use the MSPOverUSB class to work with the MSP 
// The class searches for an MPS by USB vendor/product ID on initialization
// It differentiates MSP430f5510 vs 5528 automatically by PID. 

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
  // raw values for analog ports A1-A6 are at indexes 0-5 of this array
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

// maybe this class could simplify calling MSPOverUSB::setMotorPowers
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

  bool log; 

  // what type of ADC block do we have ? 
  bool adc10, adc12; 

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

  // synchronously read new values from the MSP
  RawSensorValues askMSP();

  // request new values and then clear some of the counters
  // the indexes of counters to clear are given in [encoders], [hwps]
  RawSensorValues askMSPAndResetCounters(vector<int> const& encoders, vector<int> const& hwps);

  // accepts a packet of requests to change power for some of the motors
  // synchronously sends it as one USB packet to the MSP 
  void setMotorPowers(std::array<std::pair<bool, int8_t>, N_POWER_MOTOR> const& ); 

};



  
