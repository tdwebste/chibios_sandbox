/**
 * db.c
 * Database access.
 * The items here will be stored in Flash as part of the configuration. For now
 * the values are hardcoded.
 *
 *  Created on: 2013-04-30
 *      Author: jeromeg
 */
#include "db.h"
#define WHEEL_DIAMETER_M   0.4  /* Wheel diameter in meters */
#define PULSES_PER_ROTATION 100 /* Pulses per rotation of the wheel */

/**
 * Calculates the number of meters traveled per pulse.
 * @return
 */
float dbGetWheelScaleFactor(void)
{
    return 2 * PI * WHEEL_DIAMETER_M / PULSES_PER_ROTATION;
}

/**
 * Get the threshold between high and low pressure
 * @return
 */
int32_t dbGetFullPressureUpperThreshold(void)
{
    return 10100;
}

/**
 * Get the threshold between high and low pressure
 * @return
 */
int32_t dbGetFullPressureLowerThreshold(void)
{
    return 10100;
}

/**
 * Get the high speed upper threshold. High Speed condition is declared when
 * the speed is above this threshold.
 * @return
 */
int32_t dbGetHighSpeedUpperThreshold(void)
{
    return 40000;
}

/**
 * Get the high speed lower threshold. High Speed condition is exited and Low
 * Speed condition is entered when the speed is below this threshold.
 * @return
 */
int32_t dbGetHighSpeedLowerThreshold(void)
{
    return 35000;
}

/**
 * Get low speed upper threshold. Low Speed condition is entered when the speed
 * above this threshold.
 * @return
 */
int32_t dbGetLowSpeedUpperThreshold(void)
{
    return 1000;
}

/**
 * Get low speed lower threshold. Stopped condition is entered when the speed is
 * below this threshold.
 * @return
 */
int32_t dbGetLowSpeedLowerThreshold(void)
{
    return 500;
}

/**
 * Get reversed upper threshold. Reversed condition is entered when the speed
 * (in reverse) is above this threshold.
 * @return
 */
int32_t dbGetReversedUpperThreshold(void)
{
    return -1000;
}

/**
 * Get reversed lower threshold. Reversed condition is exited and Stopped
 * condition is entered when the speed (in reverse) is below this threshold.
 * @return
 */
int32_t dbGetReversedLowerThreshold(void)
{
    return -500;
}

/**
 * Return which sensor has been selected for speed measurement.
 * @return
 */
int8_t dbGetSensorIndex(void)
{
    return 0;
}
