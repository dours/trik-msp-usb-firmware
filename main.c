/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*  
 * ======== main.c ========
 * Local Echo Demo:
 *
 * This example simply echoes back characters it receives from the host.  
 * Unless the terminal application has a built-in echo feature turned on, 
 * typing characters into it only causes them to be sent; not displayed locally.
 * This application causes typing in Hyperterminal to feel like typing into any 
 * other PC application ? characters get displayed.  
 *
 * ----------------------------------------------------------------------------+
 * Please refer to the Examples Guide for more details.
 * ---------------------------------------------------------------------------*/
#include <string.h>

#include "driverlib.h"

#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/usb.h"                 // USB-specific functions
#include "USB_API/USB_CDC_API/UsbCdc.h"
#include "USB_app/usbConstructs.h"

/*
 * NOTE: Modify hal.h to select a specific evaluation board and customize for
 * your own board.
 */
#include "hal.h"

// Global flags set by events
volatile uint8_t bCDCDataReceived_event = FALSE;  // Flag set by event handler to 
                                               // indicate data has been 
                                               // received into USB buffer

#define BUFFER_SIZE 256
char dataBuffer[BUFFER_SIZE] = "";
char nl[2] = "\n";
uint16_t count;                    


void initAdc() { 
  bool status = ADC10_A_init(ADC10_A_BASE, ADC10_A_SAMPLEHOLDSOURCE_SC, ADC10_A_CLOCKSOURCE_ADC10OSC, ADC10_A_CLOCKDIVIDER_32);
  ADC10_A_enable(ADC10_A_BASE); 
  ADC10_A_setupSamplingTimer(ADC10_A_BASE, ADC10_A_CYCLEHOLD_16_CYCLES, ADC10_A_MULTIPLESAMPLESENABLE);
  ADC10_A_configureMemory(ADC10_A_BASE, ADC10_A_INPUT_A3, ADC10_A_VREFPOS_AVCC, ADC10_A_VREFNEG_AVSS); // why is it called configure **Memory** ? 
  ADC10_A_disableReferenceBurst(ADC10_A_BASE); 
  ADC10_A_enableInterrupt(ADC10_A_BASE, ADC10_A_COMPLETED_INT); 
  ADC10_A_startConversion(ADC10_A_BASE, ADC10_A_REPEATED_SEQOFCHANNELS); 
}


// a circular buffer for ADC samples 
#define ADC_BUF_SIZE (128)
struct {
  int16_t buf[ADC_BUF_SIZE]; 
  uint8_t head, tail; // head is first occupied, tail is first unoccupied
  bool overflow; 
} adcBuf; 
void adcBufInit() { adcBuf.head = adcBuf.tail = 0; adcBuf.overflow = false; }
uint8_t adcBufNext(uint8_t x) { 
  uint8_t nextValue = x + 1;
  if (nextValue == ADC_BUF_SIZE) return 0; else return nextValue; 
}
void adcBufAddSample(int16_t sample) { 
  adcBuf.buf[adcBuf.tail] = sample; 
  adcBuf.tail = adcBufNext(adcBuf.tail); 
  if (adcBuf.tail == adcBuf.head) adcBuf.overflow = true; 
}
uint8_t adcBufSize() { 
  if (adcBuf.tail >= adcBuf.head) return adcBuf.tail - adcBuf.head; 
  else return (ADC_BUF_SIZE - adcBuf.head) + adcBuf.tail; 
}
bool adcBufIsEmpty() { return adcBuf.head == adcBuf.tail; } 
void adcBufSendPacket() { 
  bool oneChunk = adcBuf.tail >= adcBuf.head; 
  uint8_t size = (oneChunk) ? (adcBuf.tail - adcBuf.head) : (ADC_BUF_SIZE - adcBuf.head); 
  size = (size > 32) ? 32 : size; // sendData can send more, but we do not want to delay recieving too much 
  USBCDC_sendDataInBackground((uint8_t*)(adcBuf.buf + adcBuf.head), size * 2, CDC0_INTFNUM, 0); // if it hangs (it shouldn't), just reboot the MSP
  adcBuf.head += size; 
  if (adcBuf.head == ADC_BUF_SIZE) adcBuf.head = 0; 
}
  

/*----------------------------------------------------------------------------+
 | Main Routine                                                                |
 +----------------------------------------------------------------------------*/
void main (void)
{
    WDT_A_hold(WDT_A_BASE); // Stop watchdog timer

    // Minimum Vcore setting required for the USB API is PMM_CORE_LEVEL_2 .
    PMM_setVCore(PMM_CORE_LEVEL_2);

// TODO: is this okay for TRIK?    USBHAL_initPorts();           // Config GPIOS for low-power (output low)
    USBHAL_initClocks(8000000);   // Config clocks. MCLK=SMCLK=FLL=8MHz; ACLK=REFO=32kHz
    USB_setup(TRUE, TRUE); // Init USB & events; if a host is present, connect

    __enable_interrupt();  // Enable interrupts globally
    adcBufInit(); 
    initAdc(); 

    while (1)
    {
        uint8_t ReceiveError = 0, SendError = 0;
        uint16_t count;
        
        // Check the USB state and directly main loop accordingly
        switch (USB_getConnectionState())
        {
            // This case is executed while your device is enumerated on the
            // USB host
            case ST_ENUM_ACTIVE:
                if (!adcBufIsEmpty()) adcBufSendPacket(); 
#if 0 
                // Sleep if there are no bytes to process.
                __disable_interrupt();
                if (!USBCDC_getBytesInUSBBuffer(CDC0_INTFNUM)) {
                
                    // Enter LPM0 until awakened by an event handler
                    __bis_SR_register(LPM0_bits + GIE);
                }

                __enable_interrupt();

#endif
                // Exit LPM because of a data-receive event, and
                // fetch the received data
                if (USBCDC_getBytesInUSBBuffer(CDC0_INTFNUM)) {
                
                    // Clear flag early -- just in case execution breaks
                    // below because of an error
                    bCDCDataReceived_event = FALSE;

                    count = USBCDC_receiveDataInBuffer((uint8_t*)dataBuffer,
                        BUFFER_SIZE,
                        CDC0_INTFNUM);
#if 0 
                    // Count has the number of bytes received into dataBuffer
                    // Echo back to the host.
                    if (USBCDC_sendDataInBackground((uint8_t*)dataBuffer,
                            count, CDC0_INTFNUM, 1)){
                        // Exit if something went wrong.
                        SendError = 0x01;
                        break;
                    }
#endif
                }
                break;
                
            // These cases are executed while your device is disconnected from
            // the host (meaning, not enumerated); enumerated but suspended
            // by the host, or connected to a powered hub without a USB host
            // present.
            case ST_PHYS_DISCONNECTED:
            case ST_ENUM_SUSPENDED:
            case ST_PHYS_CONNECTED_NOENUM_SUSP:
                __bis_SR_register(LPM3_bits + GIE);
                _NOP();
                break;

            // The default is executed for the momentary state
            // ST_ENUM_IN_PROGRESS.  Usually, this state only last a few
            // seconds.  Be sure not to enter LPM3 in this state; USB
            // communication is taking place here, and therefore the mode must
            // be LPM0 or active-CPU.
            case ST_ENUM_IN_PROGRESS:
            default:;
        }

        if (ReceiveError || SendError){
            // TO DO: User can place code here to handle error
        }
    }  //while(1)
}                               // main()


int16_t testCounter = 0;
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC_ISR (void) {
  int16_t value = ADC10MEM0; // ADC10_A_getResults(ADC10_A_BASE);
  ++testCounter;
//  ADC10_A_clearInterrupt(ADC10_A_BASE, ADC10_A_COMPLETED_INTFLAG); 
  adcBufAddSample(value); 
  adcBufAddSample(testCounter); 
}

/*  
 * ======== UNMI_ISR ========
 */
#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR (void)
#elif defined(__GNUC__) && (__MSP430__)
void __attribute__ ((interrupt(UNMI_VECTOR))) UNMI_ISR (void)
#else
#error Compiler not found!
#endif
{
    switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG ))
    {
        case SYSUNIV_NONE:
            __no_operation();
            break;
        case SYSUNIV_NMIIFG:
            __no_operation();
            break;
        case SYSUNIV_OFIFG:
            UCS_clearFaultFlag(UCS_XT2OFFG);
            UCS_clearFaultFlag(UCS_DCOFFG);
            SFR_clearInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
            break;
        case SYSUNIV_ACCVIFG:
            __no_operation();
            break;
        case SYSUNIV_BUSIFG:
            // If the CPU accesses USB memory while the USB module is
            // suspended, a "bus error" can occur.  This generates an NMI.  If
            // USB is automatically disconnecting in your software, set a
            // breakpoint here and see if execution hits it.  See the
            // Programmer's Guide for more information.
            SYSBERRIV = 0; // clear bus error flag
            USB_disable(); // Disable
    }
}

//Released_Version_5_20_06_02
