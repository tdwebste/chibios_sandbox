/**
 * @file main.c
 *
 * Copyright (C) 2013 Genist
 *
 * @addtogroup SPIF
 * @{
 */

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "ms5803.h"
#include "rtd.h"
#include "led.h"
#include "genist_debug.h"
#include "threadlist.h"
#include "inputs.h"
// #include "statemachine.h"
#include "builtintest.h"
#include "shell.h"
#include "db.h"
#define DE_ENABLE_TIME  10U
#define DE_DISABLE_TIME 10U
#define DEAT_SHIFT      21
#define DEDT_SHIFT      16
#define START_JSON
#ifndef START_JSON
extern Thread *startConsole(void);
#else
extern Thread *startJSON(void);
#endif

/**
 * Serial port configuration
 * RS-485 features enabled
 * 1 Mb/s 8N1
 */
SerialConfig sdConfig = {
    .speed = 115200,
    .cr1 = (DE_ENABLE_TIME << DEAT_SHIFT) | (DE_DISABLE_TIME << DEDT_SHIFT),
    .cr2 = USART_CR2_STOP1_BITS | USART_CR2_LINEN,
    .cr3 = USART_CR3_DEM
};

/**
 * Application entry point.
 */
int main(void)
{
    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */

    halInit();
    chSysInit();
    sdStart(&CONSOLE, &sdConfig);
    chprintf((BaseSequentialStream *)&CONSOLE, "GENIST SPIF\r\n");

    /*
     * Shell manager initialization.
     */
    shellInit();

    /* Get database from Flash */
    chprintf((BaseSequentialStream *)&CONSOLE, "Restoring database\r\n");
    dbRestore();
    chThdSleepMilliseconds(1000);

    // disable analog status
    // enableAstatus(0);

    palClearPad(OUT_AXLE1_PORT, OUT_AXLE1_PIN);
    palClearPad(OUT_AXLE2_PORT, OUT_AXLE2_PIN);
    palClearPad(OUT_AXLE3_PORT, OUT_AXLE3_PIN);
    palClearPad(OUT_AXLE4_PORT, OUT_AXLE4_PIN);
    palClearPad(OUT_BACKUP2_PORT, OUT_BACKUP2_PIN);
    palClearPad(OUT_BACKUP_PORT, OUT_BACKUP_PIN);
    palClearPad(OUT_ACTIVECOOL_PORT, OUT_ACTIVECOOL_PIN);
    palClearPad(OUT_FLOCK_PORT, OUT_FLOCK_PIN);
    palClearPad(OUT_LOCK_PORT, OUT_LOCK_PIN);
    palClearPad(OUT_BULBCHECK_PORT, OUT_BULBCHECK_PIN);
    palClearPad(OUT_LEDPOWER_PORT, OUT_LEDPOWER_PIN);
    palClearPad(OUT_SENSPOWER_PORT, OUT_SENSPOWER_PIN);
    palClearPad(LED_STBY_PAD, LED_STBY_PIN);

//    enableAstatus(0);

//    StateMachineMailboxInit();
//    startThreadStateMachine();
//    startThreadSpeed();
//    startThreadBlinker();
//    startThreadPressureSensor();
    startThreadSDADC();
    //startThreadMonitor();
#ifdef GENIST_USE_RTD
    startThreadRTD();
#endif
//    initHazard();
    /*
     * Normal main() thread activity, it does nothing except sleeping in a loop.
     */
#ifdef START_JSON
    startJSON();
#endif
    while (TRUE)
    {
#ifndef START_JSON
        if (!shelltp)
        {
            /* Spawns a new shell.*/
            shelltp = startConsole();
        }
        else
        {
            /* If the previous shell exited.*/
            if (chThdTerminated(shelltp))
            {
                /* Recovers memory of the previous shell.*/
                chThdRelease(shelltp);
                shelltp = NULL;
                dbgprint_enable = FALSE;
            }
        }
#else
#endif
        chThdSleepMilliseconds(10000);
    }
}

/**
 * @}
 */
