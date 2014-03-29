/**
 * fake_pwm.c
 * Fake the Rhein Tacho pwm sensor using a timer resource.
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
#include "pwm_rheintacho.h"

#define PWM_FREQ_HIGH       1000000 /* 1 us resolution                  */
#define PWM_FREQ_LOW        10000   /* 100 us resolution                */


static PWMConfig FakePwmConfig = {
        .frequency = PWM_FREQ_HIGH,
        .period = 100,
        .callback = NULL,
        .channels = {
                {
                        .mode = PWM_OUTPUT_DISABLED,
                        .callback = NULL,
                },
                {
                        .mode = PWM_OUTPUT_DISABLED,
                        .callback = NULL,
                },
                {
                        .mode = PWM_OUTPUT_DISABLED,
                        .callback = NULL,
                },
                {
                        .mode = PWM_OUTPUT_DISABLED,
                        .callback = NULL,
                },
        },
        .cr2 = 0
};

/**
 * Create waveform
 * @param arg
 */
void startFakePwm(void)
{
    FakePwmConfig.channels[FAKE_PWM_CHANNEL].mode = PWM_OUTPUT_ACTIVE_HIGH;
    pwmStart(&FAKE_PWM, &FakePwmConfig);
    pwmEnableChannel(&FAKE_PWM, FAKE_PWM_CHANNEL, 45);
    dbgprintf("Fake PWM Sensor started!\r\n");
}

void setFakePwm(int speed, int state)
{
    uint32_t ticks;
    uint16_t pulse_width;
    float f = (float) abs(speed) / dbGetWheelScaleFactor() / SEC_PER_HOUR;
    ticks = (FakePwmConfig.frequency / f);

    if (ticks > 65535)
    {
        /* Switch to low gear */
        dbgprintf("Fake PWM Low Gear\r\n");
        pwmDisableChannel(&FAKE_PWM, FAKE_PWM_CHANNEL);
        pwmStop(&FAKE_PWM);
        FakePwmConfig.frequency = PWM_FREQ_LOW;
        pwmStart(&FAKE_PWM, &FakePwmConfig);
        ticks = PWM_STOP_PERIOD / 100;
    }
    else if (FakePwmConfig.frequency == PWM_FREQ_LOW)
    {
        /* Switch to high gear */
        dbgprintf("Fake PWM High Gear\r\n");
        pwmDisableChannel(&FAKE_PWM, FAKE_PWM_CHANNEL);
        pwmStop(&FAKE_PWM);
        FakePwmConfig.frequency = PWM_FREQ_HIGH;
        pwmStart(&FAKE_PWM, &FakePwmConfig);
        ticks = (FakePwmConfig.frequency / f);
    }
    pwmChangePeriod(&FAKE_PWM, (pwmcnt_t)ticks);
    if (state == PWM_STATE_AIRGAP_WARNING)
    {
        pulse_width = PWM_DIR_LR_US;
    }
    else if (speed > 0)
    {
        if (state == PWM_STATE_NORMAL)
        {
            pulse_width = PWM_DIR_DRR_US;
        }
        else
        {
            pulse_width = PWM_DIR_DRR_EL_US;
        }
    }
    else if (speed < 0)
    {
        if (state == PWM_STATE_NORMAL)
        {
            pulse_width = PWM_DIR_DRL_US;
        }
        else
        {
            pulse_width = PWM_DIR_DRL_EL_US;
        }
    }
    else
    {
        pulse_width = PWM_DIR_STOP_US;
    }
    if (FakePwmConfig.frequency == PWM_FREQ_LOW)
    {
        pulse_width = pulse_width / 100;
    }
    pwmEnableChannel(&FAKE_PWM, FAKE_PWM_CHANNEL, pulse_width);
}
