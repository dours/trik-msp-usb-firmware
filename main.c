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
#include "outBuffer.h"

/*
 * NOTE: Modify hal.h to select a specific evaluation board and customize for
 * your own board.
 */
#include "hal.h"

// we must know something about the compiler to declare interrupt routines appropriately
#if ! (defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__) || (defined(__GNUC__) && (__MSP430__)))
#error Compiler not found!
#endif

// Global flags set by events
volatile uint8_t dataReceivedEvent = FALSE;  // Flag set by event handler to 
                                               // indicate data has been 
                                               // received into USB buffer
volatile uint8_t dataSentEvent = FALSE;  

struct OutBuffer* const usbOutBuffer = (struct OutBuffer*)IEP2_X_BUFFER_ADDRESS;
volatile struct OutBuffer  buf;
struct OutBuffer* const theOutBuffer = &buf; 

// this one points into theOutBuffer.adcBuffer
volatile uint16_t* adcBufferOffset;
void initAdc() { 
  bool status = ADC10_A_init(ADC10_A_BASE, ADC10_A_SAMPLEHOLDSOURCE_SC, ADC10_A_CLOCKSOURCE_ADC10OSC, ADC10_A_CLOCKDIVIDER_8);
  ADC10_A_setupSamplingTimer(ADC10_A_BASE, ADC10_A_CYCLEHOLD_16_CYCLES, ADC10_A_MULTIPLESAMPLESENABLE);
  ADC10_A_configureMemory(ADC10_A_BASE, ADC_CHANNELS_SAMPLED - 1, ADC10_A_VREFPOS_AVCC, ADC10_A_VREFNEG_AVSS); // why is it called configure **Memory** ? 
  ADC10_A_disableReferenceBurst(ADC10_A_BASE); 
  adcBufferOffset= theOutBuffer->adcBuffer;
  ADC10_A_enableInterrupt(ADC10_A_BASE, ADC10_A_COMPLETED_INT); 
  ADC10_A_enable(ADC10_A_BASE); 
  ADC10_A_startConversion(ADC10_A_BASE, ADC10_A_REPEATED_SEQOFCHANNELS); 
}


uint8_t Ep1InEvent() { 
#ifdef OLIMEXINO_5510
  GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN1); // to measure timing 
#endif
  dataSentEvent = TRUE; 
  return 1; 
}

uint8_t Ep1OutEvent() { 
#ifdef OLIMEXINO_5510
  GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN2);
#endif
  dataReceivedEvent = TRUE; 
  return 1; 
}


uint8_t initCalled = 0; 

/*----------------------------------------------------------------------------+
 | Main Routine                                                                |
 +----------------------------------------------------------------------------*/
void main (void)
{
    WDT_A_hold(WDT_A_BASE); // Stop watchdog timer

    // Minimum Vcore setting required for the USB API is PMM_CORE_LEVEL_2 .
    PMM_setVCore(PMM_CORE_LEVEL_3);

// TODO: is this okay for TRIK?    USBHAL_initPorts();           // Config GPIOS for low-power (output low)

    // we rely on the USB library to handle all the setup packets, configuration requests
    // and descriptors, but we implement all the actual data transfers by manually programming
    // the registers of the USB block. This is not an officially supported use of the USB API
    // (because it only supports HID, CDC, MSC, PHDC USB classes and not a generic 
    // USB devices with bulk endpoints). 
    USBHAL_initClocks(12000000);   // Config clocks. MCLK=SMCLK=FLL=8MHz; ACLK=REFO=32kHz
    USB_setup(TRUE, TRUE); // Init USB & events; if a host is present, connect

    __enable_interrupt();  // Enable interrupts globally

// We manually configure endpoints 0x01, 0x81 (first endpoint for out and in)
    USBIEPCNF_1 &= ~EPCNF_DBUF; // this hack is to ensure that we do NOT use double buffering to send data to the host
    USBIEPCNF_1 &= ~EPCNF_TOGGLE;
    USBIEPCNF_1  |= EPCNF_USBIE; 
    USBIEPBBAX_1 = (IEP2_X_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3;
    USBIEPBCTX_1 = sizeof(struct OutBuffer); // allow to send the first 64 bytes from the X buffer 
    // an interrupt will be generated after the send completes, the interrupt handler will 
    // reenable the send operation again and so on 
    USBOEPCNF_1 &= ~EPCNF_DBUF;
    USBOEPCNF_1 &= ~EPCNF_TOGGLE; // wow I must reset this bit manually or else the first DATA0 packet is lost!
    USBOEPBBAX_1 =  (OEP1_X_BUFFER_ADDRESS - START_OF_USB_BUFFER) >> 3; 
    USBOEPCNF_1 |=  EPCNF_USBIE; 
    USBOEPBCTX_1 = 0; // clears the NAK bit, all other zeroes are irrelevant


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
                if (!initCalled) { 
			initCalled = 1; 
			memset(usbOutBuffer, 0, sizeof(struct OutBuffer)); 
			memset(theOutBuffer, 0, sizeof(struct OutBuffer)); 
			initAdc(); 
			initPowerMotor();
			encoderInit();
		} 
		if (dataReceivedEvent) { 
			executeMemoryCommandBuffer((void*)OEP1_X_BUFFER_ADDRESS, USBOEPBCTX_1 & 0x7f);
			dataReceivedEvent = 0;
			USBOEPBCTX_1 = 0; // clears the NAK bit, all other zeroes are irrelevant
		}
		if (dataSentEvent) {
		    memcpy(usbOutBuffer, theOutBuffer, sizeof(struct OutBuffer)); 
                    USBIEPBCTX_1 = sizeof(struct OutBuffer); // allow to send another 64 bytes from the X buffer 
		}
#ifdef OLIMEXINO_5510
		GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
#endif
#if 0 
                // Sleep if there are no bytes to process.
                __disable_interrupt();
                if (!USBCDC_getBytesInUSBBuffer(CDC0_INTFNUM)) {
                
                    // Enter LPM0 until awakened by an event handler
                    __bis_SR_register(LPM0_bits + GIE);
                }

                __enable_interrupt();

#endif
                break;
                
            // These cases are executed while your device is disconnected from
            // the host (meaning, not enumerated); enumerated but suspended
            // by the host, or connected to a powered hub without a USB host
            // present.
            case ST_PHYS_DISCONNECTED:
            case ST_ENUM_SUSPENDED:
            case ST_PHYS_CONNECTED_NOENUM_SUSP:
//                __bis_SR_register(LPM3_bits + GIE);
//                _NOP();
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

// TODO: a bug exist here: the adc buffer seems to be circular shifted by 2 positions, but why?
#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = ADC10_VECTOR
__interrupt void ADC_ISR (void)
#else 
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC_ISR (void)
#endif
{
  *(adcBufferOffset) = ADC10MEM0;
  ++adcBufferOffset;
  if (adcBufferOffset == &(theOutBuffer->seqno)) {
      adcBufferOffset = (theOutBuffer->adcBuffer);         
      ++(theOutBuffer->seqno);
  }
#ifdef OLIMEXINO_5510
  GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN3); // to measure timing 
#endif
}

#if defined(__TI_COMPILER_VERSION__) || (__IAR_SYSTEMS_ICC__)
#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR (void)
#else 
void __attribute__ ((interrupt(UNMI_VECTOR))) UNMI_ISR (void)
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
