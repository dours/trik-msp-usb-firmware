# set these to compile 
REDHAT_GCC = /home/trik/msp430-gcc-8.3.1.25_linux64/
#SUPPORT_FILE_DIRECTORY = $(REDHAT_GCC)/include
SUPPORT_FILE_DIRECTORY = /home/trik/msp430-gcc-support-files/include

# set this to flash a demo board with the BSL
BSL_SCRIPTER = /home/trik/ti/BSL-Scripter/bsl-scripter-linux-64

# define this to run on the said demo board instead of the actual TRIK board
# they are set in the buildall script 
#DEVICEU = MSP430F5510
#DEVICEL = msp430f5510
#BOARD=OLIMEXINO_5510
#BOARD=TRIK
EXAMPLE_NAME = TRIKFirmware
OUTPUT_NAME = $(EXAMPLE_NAME)-$(BOARD)-$(DEVICEL)
GCC_BIN = $(REDHAT_GCC)/bin
CC = $(GCC_BIN)/msp430-elf-gcc
OBJCOPY = $(GCC_BIN)/msp430-elf-objcopy
GDB = $(GCC_BIN)/msp430-elf-gdb
PROJECT_ROOT=..
DRIVERLIB_DIR = $(PROJECT_ROOT)/driverlib/MSP430F5xx_6xx
MSP430_FILE = $(PROJECT_ROOT)/msp430USB.ld
USB_CONFIG = $(PROJECT_ROOT)/USB_config2
USB_API = $(PROJECT_ROOT)/USB_API


CFLAGS = -D$(BOARD) -I $(SUPPORT_FILE_DIRECTORY) -I $(USB_API) -I $(DRIVERLIB_DIR) -I $(PROJECT_ROOT) -I $(USB_CONFIG) -D__$(DEVICEU)__ -DDEPRECATED -mmcu=$(DEVICEL) -O3 -Os -g -fdata-sections -w
LFLAGS = -L $(SUPPORT_FILE_DIRECTORY)  -T $(MSP430_FILE) -T $(SUPPORT_FILE_DIRECTORY)/$(DEVICEL).ld -Wl,--gc-sections

SRC_FILES = \
../system_pre_init.c \
../hal.c \
../main.c \
../memoryCommand.c \
../power_motor.c \
../USB_config2/UsbIsr.c \
../USB_config2/descriptors.c \
../USB_app/usbEventHandling.c \
../USB_API/USB_Common/usb.c \
../USB_API/USB_Common/usbdma.c \
../driverlib/MSP430F5xx_6xx/ucs.c \
../driverlib/MSP430F5xx_6xx/ldopwr.c \
../driverlib/MSP430F5xx_6xx/gpio.c \
../driverlib/MSP430F5xx_6xx/dma.c \
../driverlib/MSP430F5xx_6xx/wdt_a.c \
../driverlib/MSP430F5xx_6xx/adc10_a.c \
../driverlib/MSP430F5xx_6xx/pmm.c \
../driverlib/MSP430F5xx_6xx/comp_b.c \
../driverlib/MSP430F5xx_6xx/sysctl.c \
../driverlib/MSP430F5xx_6xx/tlv.c \
../driverlib/MSP430F5xx_6xx/timer_a.c \
../driverlib/MSP430F5xx_6xx/ref.c \
../driverlib/MSP430F5xx_6xx/adc12_a.c \
../driverlib/MSP430F5xx_6xx/sfr.c \


$(OUTPUT_NAME).txt: $(OUTPUT_NAME).hex
	srec_cat -O $@ -TITXT $< -I

$(OUTPUT_NAME).hex : $(OUTPUT_NAME).out
	$(OBJCOPY) -O ihex $< $@

$(OUTPUT_NAME).out  : $(SRC_FILES)
	$(CC) $(CFLAGS) $(LFLAGS) $? -o $(OUTPUT_NAME).out

clean: 
	rm *.out *.hex $(EXAMPLE_NAME)*.txt

# calls a program to flash an MCU via the USB bsl (at least) 
flash: $(OUTPUT_NAME).hex
	echo bsl-script | $(BSL_SCRIPTER) 

UNUSED_SRC_FILES = \
../driverlib/MSP430F5xx_6xx/mpy32.c \
../driverlib/MSP430F5xx_6xx/timer_b.c \
../driverlib/MSP430F5xx_6xx/timer_d.c \
../../../../driverlib/MSP430F5xx_6xx/eusci_a_spi.c \
../../../../driverlib/MSP430F5xx_6xx/battbak.c \
../../../../driverlib/MSP430F5xx_6xx/usci_a_spi.c \
../../../../driverlib/MSP430F5xx_6xx/crc.c \
../../../../driverlib/MSP430F5xx_6xx/eusci_b_spi.c \
../../../../driverlib/MSP430F5xx_6xx/rtc_b.c \
../../../../driverlib/MSP430F5xx_6xx/pmap.c \
../../../../driverlib/MSP430F5xx_6xx/rtc_c.c \
../../../../driverlib/MSP430F5xx_6xx/ram.c \
../../../../driverlib/MSP430F5xx_6xx/eusci_b_i2c.c \
../../../../driverlib/MSP430F5xx_6xx/dac12_a.c \
../../../../driverlib/MSP430F5xx_6xx/usci_b_spi.c \
../../../../driverlib/MSP430F5xx_6xx/usci_b_i2c.c \
../../../../driverlib/MSP430F5xx_6xx/ctsd16.c \
../../../../driverlib/MSP430F5xx_6xx/rtc_a.c \
../../../../driverlib/MSP430F5xx_6xx/tec.c \
../../../../driverlib/MSP430F5xx_6xx/sd24_b.c \
../../../../driverlib/MSP430F5xx_6xx/usci_a_uart.c \
../../../../driverlib/MSP430F5xx_6xx/flashctl.c \
../../../../driverlib/MSP430F5xx_6xx/oa.c \
../../../../driverlib/MSP430F5xx_6xx/lcd_b.c \
../../../../driverlib/MSP430F5xx_6xx/eusci_a_uart.c \
../../../../driverlib/MSP430F5xx_6xx/aes.c \

