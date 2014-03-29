#!/bin/sh
# makeimage.sh
#
# This script builds an application that executes from Flash Bank A and
# creates a signed and encrypted uploadable image.
# This file must be made executable.
#
# Invocation:
#
# ./makeimage.sh
#

make -C ../app BANK=A
prog_util.exe -e `cat blowfish.key` -s rsakey.der -a "Genist SPIF  `date +%Y%b%d` `git log -1 --abbrev-commit | grep ^commit | cut -d' ' -f2`" ../app/build/chA.bin upgrade.img
