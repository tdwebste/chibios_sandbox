# List of all the common board related files.
GENISTSRC = \
			${CHIBIOS_LOCAL}/genist_common/itmstream.c

# List of  hardware specific files.
GENISTHWSRC = \
			${CHIBIOS_LOCAL}/genist_common/led.c \
			${CHIBIOS_LOCAL}/genist_common/rtd.c \
			${CHIBIOS_LOCAL}/genist_common/db.c \
			${CHIBIOS_LOCAL}/genist_common/pressure.c \
			${CHIBIOS_LOCAL}/genist_common/hazard_sensor.c \
			${CHIBIOS_LOCAL}/genist_common/sdadc.c \
			${CHIBIOS_LOCAL}/genist_common/console.c \
			${CHIBIOS_LOCAL}/genist_common/json_shell.c
			

# List of test specific files.
GENISTTESTSRC = \
			${CHIBIOS_LOCAL}/genist_common/fake_speed.c \
			${CHIBIOS_LOCAL}/genist_common/fake_pwm.c


# Required include directories
GENISTINC = ${CHIBIOS_LOCAL}/genist_common
