SRC_FILES=" \
../system_pre_init.c \
../hal.c \
../main.c \
../USB_config2/UsbIsr.c \
../USB_config2/descriptors.c \
../USB_app/usbEventHandling.c \
../../../../USB_API/USB_Common/usb.c \
../../../../USB_API/USB_Common/usbdma.c \
../../../../driverlib/MSP430F5xx_6xx/ucs.c \
../../../../driverlib/MSP430F5xx_6xx/ldopwr.c \
../../../../driverlib/MSP430F5xx_6xx/gpio.c \
../../../../driverlib/MSP430F5xx_6xx/dma.c \
../../../../driverlib/MSP430F5xx_6xx/wdt_a.c \
../../../../driverlib/MSP430F5xx_6xx/adc10_a.c \
../../../../driverlib/MSP430F5xx_6xx/pmm.c \
../../../../driverlib/MSP430F5xx_6xx/comp_b.c \
../../../../driverlib/MSP430F5xx_6xx/sysctl.c \
../../../../driverlib/MSP430F5xx_6xx/mpy32.c \
../../../../driverlib/MSP430F5xx_6xx/tlv.c \
../../../../driverlib/MSP430F5xx_6xx/timer_a.c \
../../../../driverlib/MSP430F5xx_6xx/timer_b.c \
../../../../driverlib/MSP430F5xx_6xx/ref.c \
../../../../driverlib/MSP430F5xx_6xx/timer_d.c \
../../../../driverlib/MSP430F5xx_6xx/adc12_a.c \
../../../../driverlib/MSP430F5xx_6xx/sfr.c \
"

for i in $SRC_FILES  `find ../USB_API -name '*.h'` `find ../driverlib -name '*.h' ` ../*.ld ; do 
  echo "$i"
  head "$i" -n 31 > headb
  if [ -r heada ]; then diff -s heada headb; fi
  mv headb heada
  echo
  echo

done

#../memoryCommand.c \
#../power_motor.c \


