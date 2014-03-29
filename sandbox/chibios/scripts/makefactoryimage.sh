#!/bin/sh
# makeimage.sh
#
# This script builds an application that executes from Flash Bank F and
# creates a signed and encrypted uploadable image.
# This file must be made executable.
#
# Invocation:
#
# ./makefactoryimage.sh
#

make -C ../app BANK=F
prog_util.exe -e `cat blowfish.key` -s rsakey.der -a "Genist SPIF  `date +%Y%b%d` `git log -1 --abbrev-commit | grep ^commit | cut -d' ' -f2`" ../app/build/chF.bin factory.img
