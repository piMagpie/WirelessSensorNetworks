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


# Hello world
agazor@agazorPC ~/Desktop/contiki/examples/hello-world $ sudo make TARGET=zoul BOARD=remote-revb hello-world.upload savetarget


# contiki layers

http://anrg.usc.edu/contiki/index.php/Network_Stack

The protocol stack or network stack is an implementation of a computer networking protocol suite or protocol family. The terms are often used interchangeably; strictly speaking, the suite is the definition of the Communications protocols, and the stack is the software implementation of them.[1]

Individual protocols within a suite are often designed with a single purpose in mind. This modularization makes design and evaluation easier. Because each protocol module usually communicates with two others, they are commonly imagined as layers in a stack of protocols. The lowest protocol always deals with low-level interaction with the communications hardware. Every higher layer adds more features and capability. User applications usually deal only with the topmost layers (see also OSI model).[2]

https://en.wikipedia.org/wiki/Protocol_stack

# several usbs
sudo make TARGET=zoul BOARD=remote-revb udp-client-zoul.upload login MOTES=/dev/ttyUSB1

# ZOLERTIA - COMMUNICATION COMPUTER & MOTE USING SERIAL COMMUNICATION

agazor@agazorPC ~/Desktop/projectWSN/ContikiOS/tools $ sudo ./tunslip6 -s /dev/ttyUSB1 -t tun01 aaaa::1/64
********SLIP started on ``/dev/ttyUSB1''
opened tun device ``/dev/tun01''
ifconfig tun01 inet `hostname` mtu 1500 up
ifconfig tun01 add aaaa::1/64
ifconfig tun01 add fe80::0:0:0:1/64
ifconfig tun01

tun01     Link encap:UNSPEC  HWaddr 00-00-00-00-00-00-00-00-00-00-00-00-00-00-00-00  
          inet addr:127.0.1.1  P-t-P:127.0.1.1  Mask:255.255.255.255
          inet6 addr: fe80::1/64 Scope:Link
          inet6 addr: aaaa::1/64 Scope:Global
          UP POINTOPOINT RUNNING NOARP MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:500 
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)

*** Address:aaaa::1 => aaaa:0000:0000:0000
Got configuration message of type P
Setting prefix aaaa::
Server IPv6 addresses:
 aaaa::212:4b00:14b5:edd4
 fe80::212:4b00:14b5:edd4




