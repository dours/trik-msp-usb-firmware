
#include "power_motor.hpp"
#include "sensor.h"
#include <string.h>

#define TEST
#ifdef TEST
#define MCU_CODE 10
#define FIRMWARE_VERSION 11
  int16_t sensors[10];
  uint32_t encoders[4]; 
  void initTest() { memset(sensors, 0, sizeof(sensors)); memset(encoders, 0, sizeof(encoders)); } 
  int setPeriod(uint16_t x) { encoders[0] = x; }
  int setDutyPercent(uint8_t n, int8_t v) { encoders[n] = v; } 
  void resetEncoder(uint8_t n) { encoders[n] = 0; }
  uint16_t getHardwareDefence(const uint8_t n) { return encoders[n]; } 
  uint16_t getJAxValue(const uint8_t n) { return sensors[n]++; } 
  uint16_t getPowerSensor() { return sensors[7]++; } 
  uint16_t getMotorSensor() { return sensors[8]++; } 
  uint16_t getTempSensor() { return sensors[9]++; } 
  int setAllMotors(uint8_t* data) { int i; for (i = 0; i < 4; ++i) encoders[i] = data[i]; } 
  uint32_t getEncoderValue(uint8_t n) { return encoders[n]++; } 
#endif

#ifndef FIRMWARE_VERSION
#error FIRMWARE_VERSION must be defined from command line
#endif

#ifndef MCU_CODE
#error MCU_CODE must be defined from command line
#endif

volatile unsigned char resetFlag = 0;
static const unsigned char * TLV = (const unsigned char *)0x1A00;

static int getMspVersionCode() {
	int code = 0xFF;
	switch(TLV[4]) {
		case 0x55 :  code = TLV[5]; break;
		default:
		switch(TLV[5]) {
			default: break;
			case 0x80:
			switch(TLV[4]) {
				default: break;
				case 0x31: code = 0x10;
			}
		}
	}
	return code;
}


void	execWriteCmd(uint8_t reg, uint8_t* data, int8_t data_len){
	switch(reg)
	{
	//set pwm period
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
		//todo :
		setPeriod((data[1]<<8)|data[0]);
		break;
	case 0x14:
		setDutyPercent(0,data[0]);
		break;
	case 0x15:
		setDutyPercent(1,data[0]);
		break;
	case 0x16:
		setDutyPercent(2,data[0]);
		break;
	case 0x17:
		setDutyPercent(3,data[0]);
		break;
	case 0x18:
		if (data_len == 4)
			setAllMotors(data);
		break;
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:
		resetEncoder((reg&0x0F));
		break;
	case 0xEE:
		break;
	case 0xff:
		resetFlag = 1;
		break;
	default:
		break;
	}
}

int8_t	execReadCmd(uint8_t reg, uint8_t* data){
	short int data_len = 1;
	switch (reg)
	{
	case 0x18:
	case 0x19:
	case 0x1a:
	case 0x1b:
		data[0] = getHardwareDefence((reg&0x0F)-8);
		break;
	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23:
	case 0x24:
	case 0x25:{
			uint16_t value = getJAxValue(reg&0x0F);
			data[1] = (value >> 8)&0x0f;
			data[0] = (value)&0xFF;
			data_len = 2;
			break;
	}
	case 0x26:{
		uint16_t value = getPowerSensor();
		data[1] = (value >> 8)&0x0f;
		data[0] = (value)&0xFF;
		data_len = 2;
		break;
	}
	case 0x27:{
		uint16_t value = getMotorSensor();
		data[1] = (value >> 8)&0x0f;
		data[0] = (value)&0xFF;
		data_len = 2;
		break;
	}
	case 0x28:{
		uint16_t value = getTempSensor();
		data[1] = (value >> 8)&0x0f;
		data[0] = (value)&0xFF;
		data_len = 2;
		break;
	}
	case 0x30:
	case 0x31:
	case 0x32:
	case 0x33:{
		uint32_t value = getEncoderValue(reg&0x0f);
		data[3] = (value >> 24) & 0xFF;
		data[2] = (value >> 16) & 0xFF;
		data[1] = (value >> 8) & 0xFF;
		data[0] = (value) & 0xFF;
		data_len = 4;
		break;
	}
	case 0xEE:
		data[0] = FIRMWARE_VERSION;
		data[1] = getMspVersionCode(); // 0x10 for f5510, 0x28 for f5528
		if (data[1] != MCU_CODE) // current firmware runs on incorrect target
			data[0] = -data[0]; // fw version code is negated in this case
		data_len = 2;
		break;
	default:
		data[0] = 0xff;
		break;
	}
	return data_len;
}


