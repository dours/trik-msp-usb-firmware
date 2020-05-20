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

#include <USB_API/USB_Common/defMSP430USB.h>
#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/usb.h"                 // USB-specific functions

/*
 * NOTE: Modify hal.h to select a specific evaluation board and customize for
 * your own board.
 */
#include "hal.h"

// define this to run on the said demo board instead of the actual TRIK board
#define OLIMEXINO_5510 

// Global flags set by events
volatile uint8_t bCDCDataReceived_event = FALSE;  // Flag set by event handler to 
                                               // indicate data has been 
                                               // received into USB buffer


void initAdc() { 
  bool status = ADC10_A_init(ADC10_A_BASE, ADC10_A_SAMPLEHOLDSOURCE_SC, ADC10_A_CLOCKSOURCE_ADC10OSC, ADC10_A_CLOCKDIVIDER_32);
  ADC10_A_enable(ADC10_A_BASE); 
  ADC10_A_setupSamplingTimer(ADC10_A_BASE, ADC10_A_CYCLEHOLD_16_CYCLES, ADC10_A_MULTIPLESAMPLESENABLE);
  ADC10_A_configureMemory(ADC10_A_BASE, ADC10_A_INPUT_A3, ADC10_A_VREFPOS_AVCC, ADC10_A_VREFNEG_AVSS); // why is it called configure **Memory** ? 
  ADC10_A_disableReferenceBurst(ADC10_A_BASE); 
  ADC10_A_enableInterrupt(ADC10_A_BASE, ADC10_A_COMPLETED_INT); 
  ADC10_A_startConversion(ADC10_A_BASE, ADC10_A_REPEATED_SEQOFCHANNELS); 
}


uint8_t Ep1InEvent() { 
  GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN1);
  ++(*((uint16_t*)IEP2_X_BUFFER_ADDRESS));
  USBIEPBCTX_1 = 64; // allow to send another 64 bytes from the X buffer 
  return 1; 
}

uint8_t Ep1OutEvent() { 
  GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN2);
  return 1; 
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

    // we rely on the USB library to handle all the setup packets, configuration requests
    // and descriptors, but we implement all the actual data transfers by manually programming
    // the registers of the USB block. This is not an officially supported use of the USB API
    // (because it only supports HID, CDC, MSC, PHDC USB classes and not a generic 
    // USB devices with bulk endpoints). 
    USBHAL_initClocks(8000000);   // Config clocks. MCLK=SMCLK=FLL=8MHz; ACLK=REFO=32kHz
    USB_setup(TRUE, TRUE); // Init USB & events; if a host is present, connect
    USBIEPCNF_1 &= ~EPCNF_DBUF; // this hack is to ensure that we do NOT use double buffering to send data to the host
    memset(IEP2_X_BUFFER_ADDRESS, 0, 64); 

    __enable_interrupt();  // Enable interrupts globally
//    initAdc();

// We manually configure endpoints 0x01, 0x81 (first endpoint for in and out)
    USBIEPCNF_1  |= EPCNF_USBIE; 
    USBIEPBBAX_1 = (IEP2_X_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3;
    USBIEPBCTX_1 = 64; // allow to send the first 64 bytes from the X buffer 
    // an interrupt will be generated after the send completes, the interrupt handler will 
    // reenable the send operation again and so on 

#ifdef OLIMEXINO_5510
   // these are used on the olimexino demo board to measure execution time of some parts of the code
   GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
   GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
#endif
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
		GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
#if 0 
                // Sleep if there are no bytes to process.
                __disable_interrupt();
                if (!USBCDC_getBytesInUSBBuffer(CDC0_INTFNUM)) {
                
                    // Enter LPM0 until awakened by an event handler
                    __bis_SR_register(LPM0_bits + GIE);
                }

                __enable_interrupt();

                // Exit LPM because of a data-receive event, and
                // fetch the received data
                if (USBCDC_getBytesInUSBBuffer(CDC0_INTFNUM) ) {
                
                    // Clear flag early -- just in case execution breaks
                    // below because of an error
                    bCDCDataReceived_event = FALSE;

		    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
		    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN1);
   		    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN4);
               }
#endif
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
