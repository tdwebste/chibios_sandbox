#!/bin/sh
# uploadfactoryimage.sh
#
# This script uploads a software image into Bank F.
# 
#
# Invocation:
#
# ./uploadfactoryimage.sh /dev/<tty_device>
#
#

prog_util.exe -p $1 -b 3 -u factory.img
