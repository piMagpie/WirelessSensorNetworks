# WirelessSensorNetworks

## Configuration

Use full link

https://github.com/Zolertia/Resources/wiki/Getting-Started-with-Zolertia-products

Installing the specific contiki for zolertia.

git clone https://github.com/contiki-os/contiki.git ContikiOS


# Dependencies

apt-get install gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi 
libusb-1.0 python-usb


# First example 

## compilation
/contiki/examples/zolertia/zoul $ sudo make TARGET=zoul BOARD=remote-revb zoul-demo.upload

## connection
Desktop/contiki/examples/zolertia/zoul $ sudo make login

agazor@agazorPC ~/Desktop/contiki/examples/zolertia/zoul $ sudo make login
using saved target 'zoul'
../../../tools/sky/serialdump-linux -b115200 /dev/ttyUSB0
connecting to /dev/ttyUSB0 (115200) [OK]
-----------------------------------------
Counter = 0x00000006
VDD = 3311 mV
Temperature = 50238 mC
ADC1 = 2272 raw
ADC3 = 272 raw
-----------------------------------------
Counter = 0x00000007
VDD = 3311 mV
Temperature = 50238 mC
ADC1 = 2276 raw
ADC3 = 272 raw
-----------------------------------------
