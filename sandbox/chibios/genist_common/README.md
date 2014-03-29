genist_common

This directory contains source and header files that are generic to all boards.
The source files for various peripherals and functions are board-independent.
The mcuconf_vx.h files map actual pin assignments on a specific board to the
source file. One of these configuration files is included in the mcuconf.h
file in app directory.

Useful ChibiOS console commands

Typing the command alone will typically show usage.

mem 		- displays memory usage
threads		- displays active threads
pressure	- displays pressure values
speed		- displays speed values
temp		- displays temperature values
axle		- manually control axles
voltage		- displays raw voltage read from a SDADC channel
led			- sets LED colour
db			- manipulates database