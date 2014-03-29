Genist autogeneration of board files.

This directory contains the files necessary to autogenerate the board.c, board.h
and board.mk from xml files describing the pinout for each board. The auto-
generated files are placed directly into the local chibios boards/<board-name>
directory.

The scripts require the fmpp tool (http://fmpp.sourceforge.net/) and java.

To autogenerate the files, type:

fmpp -C config_<board-name>.fmpp
