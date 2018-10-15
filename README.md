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

/contiki/examples/zolertia/zoul $ sudo make TARGET=zoul BOARD=remote-revb zoul-demo.upload
