# List of lsm303dlhc
ACCSRC = \
	 $(CHIBIOS_LOCAL)/dev/lsm303dlhc/lsm303dlhc.c \
	 $(CHIBIOS_LOCAL)/dev/lsm303dlhc/stm32f37x_i2c.c \
	 $(CHIBIOS_LOCAL)/dev/lsm303dlhc/stm32f37x_syscfg.c \
	 $(CHIBIOS_LOCAL)/dev/lsm303dlhc/stm32f37x_exti.c 
#	 $(CHIBIOS_LOCAL)/dev/lsm303dlhc/lsm303main.c
# Required includ directories
ACCINC = $(CHIBIOS)/os/hal/platforms/STM32F37x \
         $(CHIBIOS)/os/hal/platforms/STM32/GPIOv2 \
         $(CHIBIOS_LOCAL)/dev/lsm303dlhc
