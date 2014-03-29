/**
 * fake_speed.c
 * Fake a speed sensor using a timer resource.
 *
 * A fake quadrature encoded signal is created using TIM3. Channels 2 and 4
 * are looped back or connected to another board externally to the speed sensor
 * input pins.
 *  Created on: 2013-04-29
 *      Author: jeromeg
 */

#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "genist_debug.h"
#include "db.h"
#include "fake.h"

#define PWM_FREQ_HIGH   25000
#define PWM_FREQ_LOW    1200

static PWMConfig FakeSpeedConfig = {
        .frequency = PWM_FREQ_HIGH,
        .period = 100,
        .callback = NULL,
        .channels = {
                {
                        .mode = PWM_OUTPUT_ACTIVE_HIGH,
                        .callback = NULL,
                },
                {
                        .mode = PWM_OUTPUT_ACTIVE_HIGH,
                        .callback = NULL,
                },
                {
                        .mode = PWM_OUTPUT_ACTIVE_HIGH,
                        .callback = NULL,
                },
                {
                        .mode = PWM_OUTPUT_ACTIVE_LOW,
                        .callback = NULL,
                },
        },
        .cr2 = 0
};

/**
 * Create waveform
 * @param arg
 */
void startFakeSpeed(void)
{
    FakeSpeedConfig.channels[FAKE_SPEED1_CHANNEL].mode = PWM_OUTPUT_ACTIVE_HIGH;
    FakeSpeedConfig.channels[FAKE_SPEED2_CHANNEL].mode = PWM_OUTPUT_ACTIVE_LOW;
    pwmStart(&FAKE_SPEED, &FakeSpeedConfig);
    pwmEnableChannel(&FAKE_SPEED, FAKE_SPEED1_CHANNEL, 50);
    pwmEnableChannel(&FAKE_SPEED, FAKE_SPEED2_CHANNEL, 55);
    dbgprintf("Fake Speed Sensor started!\r\n");
}

void setFakeSpeed(int speed)
{
    uint32_t ticks;
    float f = (float) abs(speed) / dbGetWheelScaleFactor() / SEC_PER_HOUR;
    ticks = (FakeSpeedConfig.frequency / f);

    if (ticks > 65535)
    {
        /* Switch to low gear */
        dbgprintf("Fake Speed Low Gear\r\n");
        pwmDisableChannel(&FAKE_SPEED, FAKE_SPEED1_CHANNEL);
        pwmDisableChannel(&FAKE_SPEED, FAKE_SPEED2_CHANNEL);
        pwmStop(&FAKE_SPEED);
        FakeSpeedConfig.frequency = PWM_FREQ_LOW;
        pwmStart(&FAKE_SPEED, &FakeSpeedConfig);
        ticks = (FakeSpeedConfig.frequency / f);
    }
    else if (FakeSpeedConfig.frequency == PWM_FREQ_LOW)
    {
        /* Switch to high gear */
        dbgprintf("Fake Speed High Gear\r\n");
        pwmDisableChannel(&FAKE_SPEED, FAKE_SPEED1_CHANNEL);
        pwmDisableChannel(&FAKE_SPEED, FAKE_SPEED2_CHANNEL);
        pwmStop(&FAKE_SPEED);
        FakeSpeedConfig.frequency = PWM_FREQ_HIGH;
        pwmStart(&FAKE_SPEED, &FakeSpeedConfig);
        ticks = (FakeSpeedConfig.frequency / f);
    }
    pwmChangePeriod(&FAKE_SPEED, (pwmcnt_t)ticks);
    pwmEnableChannel(&FAKE_SPEED, FAKE_SPEED1_CHANNEL, (pwmcnt_t)ticks / 2);
    if (speed > 0)
    {
        pwmEnableChannel(&FAKE_SPEED, FAKE_SPEED2_CHANNEL, (pwmcnt_t)ticks / 2 - (pwmcnt_t)ticks / 4);
    }
    else if (speed < 0)
    {
        pwmEnableChannel(&FAKE_SPEED, FAKE_SPEED2_CHANNEL, (pwmcnt_t)ticks / 2 + (pwmcnt_t)ticks / 4);
    }
    else
    {
        pwmDisableChannel(&FAKE_SPEED, FAKE_SPEED1_CHANNEL);
        pwmDisableChannel(&FAKE_SPEED, FAKE_SPEED2_CHANNEL);
    }


}
