
This project contains 
* a firmware for MSP430F5510, F5528 MCUs, which connects them with the OMAP
  processor on the TRIK board via USB. It supports packet reading of TRIK
  sensors and controlling its motors
* a simple library (see MSPOverUSB.h) to interface to the firmware from OMAP
  or a PC (for testing purposes). 

The project is compilable for the TRIK board and for OLIMEXINO-5510 (for
testing purposes).

Building:
* set the variables in the head of GCC/Makefile to build with msp-gcc (other
  compilers were not tested)
* call GCC/buildall to build for all the MSP devices and boards
* call buildHost in the root to build libMSPOverUSB.so and a console app for
  testing 

The project uses parts of USB_API and driverlib by TI. 

