#include "ch.h"
#include "hal.h"
#include "led.h"
#include "threads.h"

/**
 * Spin LED wheel on Discovery Card
 */
void spinLED(void)
{
    static uint16_t led;
    led = (led << 1) & 0xff00;
    if (!led)
    {
        led = 0x0100;
    }
    palSetPort(GPIOE, led);
    palClearPort(GPIOE, (~led) & 0xff00);
}

