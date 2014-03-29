#!/bin/sh
# lockkey.sh
#
# This script locks keys from further modification
#
# Invocation:
#
# ./lockkey.sh /dev/<tty_device>
#

prog_util.exe -p $1 -l
