/**
 * Copyright 2013 Genist Systems Inc.
 * hall_sensor.c
 *
 * This module detects whether the hazard lights have been turned on.
 *  Created on: 2013-04-29
 *      Author: jeromeg
 */

#include <string.h>
#include "ch.h"
#include "hal.h"
#include "db.h"
#include "genist_debug.h"
#include "threads.h"
#include "statemachine.h"

/**
User Hazard Mode is enabled by
STATE_USER_O
PRE0_USER
- fourways on for atleast 3 ticks averaged over 5 ticks
              but less than 8 ticks
PRE1_USER
- fourways off for atleast 5 seconds
              but less than 20 seconds
PRE2_USER
- fourways on again (same)
PRE3_USER
STATE_USER_H


STATE_USER_H PRE1_USER PRE0_USER
User Hazard Mode is disable by
- fourways off for at least 20 seconds
or over speed
STATE_USER_O
*/

#define FULL_SCALE          65536
#define HAZARD_FREQUENCY    1000
#define HAZ_CYCLE           3

#define PRE0_USER           0
#define PRE1_USER           1
#define PRE2_USER           2
#define PRE3_USER           3

#define NUM_HAZ             5
static struct VirtualTimer vtsample;
static struct VirtualTimer vtoff;
static struct VirtualTimer vtcancel;

static uint32_t hazArray[NUM_HAZ];
static uint32_t hazIndex;
static int32_t hazSum;
static int32_t hazCount;

static void HazardPeriodCB(ICUDriver *icup);
//static void HazardOverflowCB(ICUDriver *icup);
static UserState HazardState;
static int8_t PendHazState;
static int8_t prewindow;
static uint8_t hazOver;
static int32_t hazCount;

// overflow too long use virtual timer to set a shorter overflow
const ICUConfig HazardSensorConfig = {
        .mode = ICU_INPUT_ACTIVE_HIGH,
        .frequency = HAZARD_FREQUENCY,
        .width_cb = NULL,
        .period_cb = HazardPeriodCB,
        .overflow_cb = NULL,
        .channel = HAZ_TIM_CHANNEL
};
//        .overflow_cb = HazardOverflowCB,


/**
 * Callback from ICU driver when the next edge is detected on one turn signal.
 * The other turn signal is sampled to see if both are activated.
 * @param icup
 */
static void HazardPeriodCB(ICUDriver *icup)
{
    (void) icup;
    chSysLockFromIsr(); 
    if (chVTIsArmedI(&vtsample))
    {
        chSysUnlockFromIsr(); 
        return;
    }
    chVTSetIsr(&vtsample, MS2ST(100), TimerFlashSample, NULL);
    if (chVTIsArmedI(&vtoff))
    {
        chVTResetIsr(&vtoff);
    }
    chVTSetIsr(&vtoff, S2ST(dbGetTimeoutHazOff()), TimerFlashOff, NULL);
    if (chVTIsArmedI(&vtcancel))
    {
        chVTResetIsr(&vtcancel);
    }
    chVTSetIsr(&vtcancel, S2ST(dbGetTimeoutHazCancel()), TimerFlashCancel, NULL);
    chSysUnlockFromIsr(); 
}

/**
 * virtual timer to triger sampling of other hazard turn signal
 */
void TimerFlashSample(void *arg)
{
    (void)arg;
    uint32_t oldHaz;
    uint16_t lsignal, rsignal;

    oldHaz = hazArray[hazIndex];
    lsignal = palReadPad(HAZ_PORT, HAZ_PIN2);
    rsignal = palReadPad(HAZ_PORT, HAZ_PIN1);
    if (!( rsignal | lsignal ))
    {
        // missed signal or fallowing edge
        return;
    }
    else if (( rsignal ^ lsignal ))
    {
        // only one, turn signal
        return;
    }
    chprintf((BaseSequentialStream *)&CONSOLE,
        "matched  L %d R %d off %d cancel %d\r\n",
        rsignal, lsignal, 
        dbGetTimeoutHazOff(),dbGetTimeoutHazCancel());
    if (chVTIsArmedI(&vtsample))
    {
        chprintf((BaseSequentialStream *)&CONSOLE,
        "vtsample ");
    }
    else
    {
        chprintf((BaseSequentialStream *)&CONSOLE,
        "unsetvtsample ");
    }
    if (chVTIsArmedI(&vtoff))
    {
        chprintf((BaseSequentialStream *)&CONSOLE,
        "vtoff ");
    }
    else
    {
        chprintf((BaseSequentialStream *)&CONSOLE,
        "unsetvtoff ");
    }
    if (chVTIsArmedI(&vtcancel))
    {
        chprintf((BaseSequentialStream *)&CONSOLE,
        "vtcancel \r\n");
    }
    else
    {
        chprintf((BaseSequentialStream *)&CONSOLE,
        "unsetvtcancel ");
    }


    hazArray[hazIndex] = !(lsignal ^ rsignal);
    hazSum += hazArray[hazIndex] - oldHaz;
    hazIndex = (hazIndex + 1) % NUM_HAZ;
    hazOver = 0;

//    chprintf((BaseSequentialStream *)&CONSOLE,
//        "hazcount %d hazsum %d dectect %d oldhaz %d hazindex %d hazArray %d\r\n",
//         hazCount, hazSum, dbGetFlashesHazDetect(), oldHaz, hazIndex, hazArray[hazIndex]);
    chprintf((BaseSequentialStream *)&CONSOLE,
        "Sample PendHazState %d\r\n", PendHazState);
    if (hazCount < dbGetFlashesCancelHazDetect() )
    {
        if (hazSum > dbGetFlashesHazDetect() )
        {
            if (PendHazState == PRE0_USER)
            {
                PendHazState = PRE1_USER;
            }
            else if (PendHazState == PRE2_USER)
            {
                PendHazState = PRE3_USER;
                chprintf((BaseSequentialStream *)&CONSOLE, "Haz 1\r\n");
                HazardState = STATE_USER_H;
                SendEvent(EVENT_USER_H);
            }
            else if (PendHazState == PRE1_USER)
            {
                hazCount++;
            }
            else
            {
            chprintf((BaseSequentialStream *)&CONSOLE,
            "pend haz INVALID\r\n");
            }
        }
    }
    else
    {
        if (HazardState == STATE_USER_H)
        {
            SendEvent(EVENT_USER_O);
        }
        HazardState = STATE_USER_O;
        PendHazState = PRE0_USER;
        memset(hazArray, 0, sizeof(hazArray));
        hazSum = 0;
        hazCount = 0;
    }
}


/**
 * virtual timer to measure hazard timeout 
 */
void TimerFlashOff(void *arg)
{
    (void)arg;
    chprintf((BaseSequentialStream *)&CONSOLE,
        "Off PendHazState %d\r\n", PendHazState);
    if (PendHazState == PRE1_USER)
    {
        PendHazState = PRE2_USER;
    }
    memset(hazArray, 0, sizeof(hazArray));
    hazSum = 0;
    hazCount = 0;
}

/**
 * virtual timer to cancel hazard 
 */
void TimerFlashCancel(void *arg)
{
    (void)arg;
    chprintf((BaseSequentialStream *)&CONSOLE,
        "Cancel PendHazState %d\r\n", PendHazState);
    if (PendHazState != PRE0_USER)
    {
        if (HazardState == STATE_USER_H)
        {
            chprintf((BaseSequentialStream *)&CONSOLE, "Haz 0\r\n");
            HazardState = STATE_USER_O;
            SendEvent(EVENT_USER_O);
        }
        PendHazState = PRE0_USER;
    }
    memset(hazArray, 0, sizeof(hazArray));
    hazSum = 0;
    hazCount = 0;
}


/**
 * Callback from ICU driver when the next edge has not occurred before the
 * timer overflows. This indicates that the hazard signals have been turned off.
 * @param icup
static void HazardOverflowCB(ICUDriver *icup)
{
    (void) icup;
    chprintf((BaseSequentialStream *)&CONSOLE,
        "Overflow PendHazState %d\r\n", PendHazState);
    TimerFlashCancel(NULL);
}
 */

/**
 * Initialize Hazard sensor
 */
void initHazard(void)
{
    HazardState = STATE_USER_O;
    prewindow = 0;
    hazIndex = 0;
    memset(hazArray, 0, sizeof(hazArray));
    hazSum = 0;
    hazCount = 0;

    chThdSleepMilliseconds(1000); // wait for startup noise to settle
    icuStart(&HAZ_SENSOR, &HazardSensorConfig);
    icuEnable(&HAZ_SENSOR);
    // intialize 
    chVTSet(&vtcancel, S2ST(dbGetTimeoutHazCancel()), TimerFlashCancel, NULL);
}

