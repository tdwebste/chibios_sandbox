#!/bin/sh
# uploadkey.sh
#
# This script uploads keys to the board
#
# Invocation:
#
# ./uploadkey.sh /dev/<tty_device>
#

prog_util.exe -e `cat blowfish.key` -s rsakey.der -p $1 -k
