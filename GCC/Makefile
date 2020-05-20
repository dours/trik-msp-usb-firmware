DEVICE = MSP430F5510
EXAMPLE_NAME = TRIKFirmware
USB_BASE = ./../../../..
REDHAT_GCC = /home/trik/msp430-gcc-8.3.1.25_linux64/
GCC_BIN = $(REDHAT_GCC)/bin
CC = $(GCC_BIN)/msp430-elf-gcc
OBJCOPY = $(GCC_BIN)/msp430-elf-objcopy
BSL_SCRIPTER = /home/trik/ti/BSL-Scripter/bsl-scripter-linux-64
GDB = $(GCC_BIN)/msp430-elf-gdb
#SUPPORT_FILE_DIRECTORY = $(REDHAT_GCC)/include
SUPPORT_FILE_DIRECTORY = /home/trik/msp430-gcc-support-files/include
DRIVERLIB_DIR = $(USB_BASE)/driverlib/MSP430F5xx_6xx
MSP430_FILE = $(USB_BASE)/USB_API/msp430USB.ld
PROJECT_ROOT = $(USB_BASE)/examples/CDC_virtualCOMport/$(EXAMPLE_NAME)
USB_CONFIG = $(PROJECT_ROOT)/USB_config2
USB_API = $(USB_BASE)/USB_API


CFLAGS = -I $(SUPPORT_FILE_DIRECTORY) -I $(USB_BASE) -I $(DRIVERLIB_DIR) -I $(PROJECT_ROOT) -I $(USB_CONFIG) -I $(USB_API) -D__$(DEVICE)__ -DDEPRECATED -mmcu=msp430f5510 -O3 -Os -g -fdata-sections -w
LFLAGS = -L $(SUPPORT_FILE_DIRECTORY)  -T $(MSP430_FILE) -T $(SUPPORT_FILE_DIRECTORY)/msp430f5510.ld -Wl,--gc-sections

SRC_FILES = \
../system_pre_init.c \
../hal.c \
../main.c \
../rxtxHandlers.c \
../USB_config2/UsbIsr.c \
../USB_config2/descriptors.c \
../USB_app/usbConstructs.c \
../USB_app/usbEventHandling.c \
../../../../USB_API/USB_Common/usb.c \
../../../../USB_API/USB_Common/usbdma.c \
../../../../driverlib/MSP430F5xx_6xx/dac12_a.c \
../../../../driverlib/MSP430F5xx_6xx/usci_b_spi.c \
../../../../driverlib/MSP430F5xx_6xx/usci_b_i2c.c \
../../../../driverlib/MSP430F5xx_6xx/ctsd16.c \
../../../../driverlib/MSP430F5xx_6xx/rtc_a.c \
../../../../driverlib/MSP430F5xx_6xx/tec.c \
../../../../driverlib/MSP430F5xx_6xx/ucs.c \
../../../../driverlib/MSP430F5xx_6xx/ldopwr.c \
../../../../driverlib/MSP430F5xx_6xx/battbak.c \
../../../../driverlib/MSP430F5xx_6xx/sd24_b.c \
../../../../driverlib/MSP430F5xx_6xx/gpio.c \
../../../../driverlib/MSP430F5xx_6xx/dma.c \
../../../../driverlib/MSP430F5xx_6xx/wdt_a.c \
../../../../driverlib/MSP430F5xx_6xx/adc10_a.c \
../../../../driverlib/MSP430F5xx_6xx/pmm.c \
../../../../driverlib/MSP430F5xx_6xx/usci_a_uart.c \
../../../../driverlib/MSP430F5xx_6xx/comp_b.c \
../../../../driverlib/MSP430F5xx_6xx/sysctl.c \
../../../../driverlib/MSP430F5xx_6xx/mpy32.c \
../../../../driverlib/MSP430F5xx_6xx/flashctl.c \
../../../../driverlib/MSP430F5xx_6xx/oa.c \
../../../../driverlib/MSP430F5xx_6xx/lcd_b.c \
../../../../driverlib/MSP430F5xx_6xx/tlv.c \
../../../../driverlib/MSP430F5xx_6xx/eusci_a_uart.c \
../../../../driverlib/MSP430F5xx_6xx/aes.c \
../../../../driverlib/MSP430F5xx_6xx/timer_a.c \
../../../../driverlib/MSP430F5xx_6xx/rtc_c.c \
../../../../driverlib/MSP430F5xx_6xx/timer_b.c \
../../../../driverlib/MSP430F5xx_6xx/ram.c \
../../../../driverlib/MSP430F5xx_6xx/eusci_b_i2c.c \
../../../../driverlib/MSP430F5xx_6xx/ref.c \
../../../../driverlib/MSP430F5xx_6xx/timer_d.c \
../../../../driverlib/MSP430F5xx_6xx/pmap.c \
../../../../driverlib/MSP430F5xx_6xx/eusci_b_spi.c \
../../../../driverlib/MSP430F5xx_6xx/adc12_a.c \
../../../../driverlib/MSP430F5xx_6xx/rtc_b.c \
../../../../driverlib/MSP430F5xx_6xx/eusci_a_spi.c \
../../../../driverlib/MSP430F5xx_6xx/usci_a_spi.c \
../../../../driverlib/MSP430F5xx_6xx/sfr.c \
../../../../driverlib/MSP430F5xx_6xx/crc.c \


$(EXAMPLE_NAME).hex : $(EXAMPLE_NAME).out
	$(OBJCOPY) -O ihex $< $@

$(EXAMPLE_NAME).out  : $(SRC_FILES)
	$(CC) $(CFLAGS) $(LFLAGS) $? -o $(EXAMPLE_NAME).out

debug: C3_EchoToHost
	$(GDB) $(EXAMPLE_NAME).out

clean: 
	rm *.out *.hex

flash: $(EXAMPLE_NAME).hex
	echo bsl-script | $(BSL_SCRIPTER) 

