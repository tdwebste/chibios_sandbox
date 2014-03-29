/**
 * led.c
 *
 * This module controls the external LEDs.
 */
#include "ch.h"
#include "hal.h"
#include "statemachine.h"

#include "builtintest.h"
#include "genist_debug.h"


/**
 * Initialize the high side driver
 */
void InitTest(void)
{
    /* Power up LED */
    palClearPad(OUT_AXLE1_PORT, OUT_AXLE1_PIN);
    palClearPad(OUT_AXLE2_PORT, OUT_AXLE2_PIN);
    palClearPad(OUT_AXLE3_PORT, OUT_AXLE3_PIN);
    palClearPad(OUT_AXLE4_PORT, OUT_AXLE4_PIN);
    
    palClearPad(OUT_BACKFORTH_PORT, OUT_BACKFORTH_PIN);
    palClearPad(OUT_BACKUP_PORT, OUT_BACKUP_PIN);
    palClearPad(OUT_ACTIVECOOL_PORT, OUT_ACTIVECOOL_PIN);
    palClearPad(OUT_FLOCK_PORT, OUT_FLOCK_PIN);

    palClearPad(OUT_LOCK_PORT, OUT_LOCK_PIN);
    palClearPad(OUT_BULBCHECK_PORT, OUT_BULBCHECK_PIN);
    palClearPad(OUT_LEDPOWER_PORT, OUT_LEDPOWER_PIN);
    palClearPad(OUT_SENSPOWER_PORT, OUT_SENSPOWER_PIN);
}

