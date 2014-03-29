#!/bin/sh
# uploadkey.sh
#
# This script uploads a software image into Bank A.
# 
#
# Invocation:
#
# ./upload.sh file.img /dev/<tty_device>
#
#

prog_util.exe -p $2 -b 1 -u $1
