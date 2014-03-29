cJSON Code modified for Genist

The original cJSON code is modified here to eliminate references to malloc/free 
routines. ChibiOS malloc()/free() functions are used instead. References to 
sscanf() and sprintf() were also replaced because they call malloc().

JSON Shell operation (in genist_common)

The JSON shell is a command/response interface that runs over the RS485 interface.
A command consists of an upper case word optionally followed by a JSON string. The
entire command string is encapsulated between STX (0x02) and ETX (0x03) characters.
The shell will respond with either an "OK" or "ERROR" string optionally followed by
a JSON string.

Example:

Command:  <STX>SET {"PulsesPerRotation":50}<ETX> 
Response: <STX>OK<ETX>

sets the PulsesPerRotation database item to 50.

The command descriptions below assume that command/response strings are always 
encapsulated by <STX> and <ETX>. White space (CR, LF, tabs, etc) is ignored on 
input and not generated on output i.e. the JSON string in the response is a 
continuous string without line breaks. 

Commands:

CONSOLE

    Console mode has full control of the events, states and data
    Console mode must be disabled in the product
    This will be done by a build with Console mode enabled for testing
    And a build with Console mode disabled for application to trucks`

	Puts the RS485 into console mode. This mode allows human interaction with the
	board. On a terminal emulator like PuTTY, this can be achieved by blindly
	typing "<Ctrl-B>CONSOLE<Ctrl-C>".
	
	Typing "exit" at the console prompt returns the board to JSON shell mode.

INVALID
	
GET
	Gets the entire database in JSON format.
	Command:  GET {"Config"}
    Response: OK {"PulsesPerRotation":100,
        "NumberAxles":2,
        "LeadingAxles":2,
        "PressureAllUpperThreshold":11000,
        "PressureAllLowerThreshold":9000,
        "PressureLessOneUpperThreshold":6000,
        "PressureLessOneLowerThreshold":2000,
        "HighSpeedUpperThreshold":40000,
        "HighSpeedLowerThreshold":35000,
        "LowSpeedUpperThreshold":1000,
        "LowSpeedLowerThreshold":500,
        "ReverseUpperThreshold":-500,
        "ReverseLowerThreshold":-1000,
        "SpeedSensorIndex":1,
        "SpeedSensorRTD":0,
        "WheelDiameter":400,
        "Warnings":1,
        "LowerFullWeight":55000,
        "FactoryVersion":1,
        "CurrentVersion":1,
        "L1":1, "P1":1, 
        "L2":2, "P2":4,
        "L3":3, "P3":9,
        "L4":4, "P4":16}




mark the current by the storing null in the next record to be written 
end of log null marker
	Command:  GET {"FaultLog"}
    Response: OK {"FAULTCURRENT":2334,
                "FAULTEND":102400}

	Command:  GET {"FaultLogBEGIN:234,
                "FaultLogEND:512}
    Response: OK {"REC":234,
                    "UNIXTIME":1374391675,
                    "FAULTNUM":42,
                    "CKSUM":23525,
                    "REC":235,
                    "UNIXTIME":1374391675,
                    "FAULTNUM":42,
                    "CKSUM":23525,
                    ....
                    "REC":512,
                    "UNIXTIME":1374391675,
                    "FAULTNUM":42,
                    "CKSUM":23525}

if invalid requrest 
    Response: ERRFAULTRECS

if warn not set END:0}
mark the current by the storing null in the next record to be written 
end of log null marker
	Command:  GET {"WarnLog"}
    Response: OK {"WARNCURRENT":2334,
                "WARNEND":102400}

	Command:  GET {"WarnLogBEGIN:234,
                "WarnLogEND:512}
    Response: OK {"REC":234,
                    "UNIXTIME":1374391675,
                    "WARNNUM":42,
                    "CKSUM":23525,
                    "REC":235,
                    "UNIXTIME":1374391675,
                    "WARNNUM":42,
                    "CKSUM":23525,
                    ....
                    "REC":512,
                    "UNIXTIME":1374391675,
                    "WARNNUM":42,
                    "CKSUM":23525}

if warn not set Error or invalid requrest
    Response: ERRWARNRECS


SAVE

	Saves the database into Flash. The database is restored during the next power
	up.
	
	Example:
	Command:  SAVE
	Response: OK
		
SET JSON-String

	Sets one or more database items as specified in the JSON string. If any item
	is not recognized as a database item, an error is returned.
	
	Example:
	
	Command:  SET {"PulsesPerRotation":50, "PressureUpperThreshold":11000}
    Response: OK
	
-----------------------------------------------------------------------------

json_test

    This "utility" is a quick-and-dirty way of testing the JSON interface over RS485.
    To compile, type "make" in this directory. Feel free to modify/enhance this code.

Usage:

    json_test -p port-number -g		- gets database JSON string
    json_test -p port-number -s		- sets the PulsesPerRotation value to 50

    port-number is one less than the COM port number, e.g. to use COM5, use
4 as the port-number
	
