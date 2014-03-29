/*
 Copyright (C) 2013 Genist
 */

#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "test.h"
#include "rtd.h"
#include "genist_debug.h"
#include "fake.h"
#include "shell.h"
#include "usbcfg.h"
#include "threads.h"

#define ESCAPE_CHAR     27
/*===========================================================================*/
/* USB related stuff.                                                        */
/*===========================================================================*/

/*
 * DP resistor control is not possible on the STM32F3-Discovery, using stubs
 * for the connection macros.
 */

#define usb_lld_connect_bus(usbp)
#define usb_lld_disconnect_bus(usbp)

/* Virtual serial port over USB.*/
SerialUSBDriver SDU1;
static uint8_t relayOn;
#define SHELL_WA_SIZE   THD_WA_SIZE(2048)
#define RELAY_WA_SIZE   THD_WA_SIZE(256)
#define TEST_WA_SIZE    THD_WA_SIZE(256)
static BaseSequentialStream *relay_to;
static BaseSequentialStream *relay_from;
extern void spinLED(void);
DECLARE_THREAD(ThreadRelay, NORMALPRIO, 256)

static msg_t ThreadRelay(void *arg)
{
    (void) arg;
    uint8_t c;
    chRegSetThreadName("relay");
    while (1)
    {
        /* Send character from relay to console */
        c = chSequentialStreamGet(relay_from);
        {
            if (relayOn)
            {
                chSequentialStreamPut(relay_to, c);
            }
        }
    }
    return 0;
}

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[])
{
    size_t n, size;

    (void) argv;
    if (argc > 0)
    {
        chprintf(chp, "Usage: mem\r\n");
        return;
    }
    n = chHeapStatus(NULL, &size);
    chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
    chprintf(chp, "heap fragments   : %u\r\n", n);
    chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[])
{
    static const char *states[] =
    { THD_STATE_NAMES };
    Thread *tp;

    (void) argv;
    if (argc > 0)
    {
        chprintf(chp, "Usage: threads\r\n");
        return;
    }
    chprintf(chp, "    addr    stack prio refs     state time\r\n");
    tp = chRegFirstThread();
    do
    {
        chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu %s\r\n", (uint32_t) tp,
                (uint32_t) tp->p_ctx.r13, (uint32_t) tp->p_prio,
                (uint32_t) (tp->p_refs - 1), states[tp->p_state],
                (uint32_t) tp->p_time, tp->p_name);
        tp = chRegNextThread(tp);
    } while (tp != NULL );
}

static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[])
{
    Thread *tp;

    (void) argv;
    if (argc > 0)
    {
        chprintf(chp, "Usage: test\r\n");
        return;
    }
    tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(), TestThread,
            chp);
    if (tp == NULL )
    {
        chprintf(chp, "out of memory\r\n");
        return;
    }
    chThdWait(tp);
}

static void cmd_speed(BaseSequentialStream *chp, int argc, char *argv[])
{

    (void) argv;
    if (argc < 1)
    {
        chprintf(chp, "Usage: speed <value>\r\n");
        return;
    }
    setFakePwm(strtol(argv[0], NULL, 0), PWM_STATE_NORMAL);
    setFakeSpeed(strtol(argv[0], NULL, 0));
    //chprintf(chp, "speed = %d\r\n", strtol(argv[0], NULL, 0));

}

static void cmd_relay(BaseSequentialStream *chp, int argc, char *argv[])
{
    uint8_t c;
    (void) argv;
    if (argc > 0)
    {
        chprintf(chp, "Usage: relay <value>\r\n");
        return;
    }
    chprintf(chp, "Starting relay. ESC to exit\r\n");
    relayOn = TRUE;
    do
    {
        /* Send character from console to relay */
        c = chSequentialStreamGet(chp);
        {
            if (c != ESCAPE_CHAR)
            {
                chSequentialStreamPut(relay_from, c);
            }
        }
    } while (c != ESCAPE_CHAR);
    relayOn = FALSE;
}

static const ShellCommand commands[] =
{
{ "mem", cmd_mem },
{ "threads", cmd_threads },
{ "test", cmd_test },
{ "speed", cmd_speed },
{ "relay", cmd_relay },
{ NULL, NULL } };

static const ShellConfig shell_cfg1 =
{ (BaseSequentialStream *) &CONSOLE, commands };
/*
 * Application entry point.
 */
int main(void)
{
    Thread *shelltp = NULL;

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();
    sdStart(&CONSOLE_RELAY, NULL);

    /*
     * Shell manager initialization.
     */
    shellInit();

    /*
     * Initializes a serial-over-USB CDC driver.
     */
    sduObjectInit(&CONSOLE);
    sduStart(&CONSOLE, &serusbcfg);

    /*
     * Activates the USB driver and then the USB bus pull-up on D+.
     * Note, a delay is inserted in order to not have to disconnect the cable
     * after a reset.
     */
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1000);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    relayOn = FALSE;
    relay_from = (BaseSequentialStream *)&CONSOLE_RELAY;
    relay_to = (BaseSequentialStream *)&CONSOLE;
    startThreadRelay();

    startFakePwm();
    startFakeSpeed();
    setFakePwm(10000, PWM_STATE_NORMAL);

    /*
     * Normal main() thread activity, in this demo it does nothing except
     * sleeping in a loop and check the button state, when the button is
     * pressed the test procedure is launched.
     */
    dbgprintf("Hello there!\r\n");
#if 0
    while (TRUE)
    {
        /*
         for (speed = 10000; speed < 200000; speed += 10000)
         {
         setFakeSpeed(speed);
         chThdSleepMilliseconds(5000);
         }
         */
        speed = 20000;
        setFakePwm(speed, PWM_STATE_AIRGAP_WARNING);
        //dbgprintff("Fake airgap warning PWM: %d\n", speed);
        chThdSleepMilliseconds(10000);
        setFakePwm(speed, PWM_STATE_EL_WARNING);
        //dbgprintff("Fake EL PWM: %d\n", speed);
        chThdSleepMilliseconds(10000);
        setFakePwm(-speed, PWM_STATE_EL_WARNING);
        //dbgprintff("Fake EL PWM: %d\n", -speed);
        chThdSleepMilliseconds(10000);
        setFakePwm(0, PWM_STATE_NORMAL);
        //dbgprintff("Fake slow PWM: %d\n", 0);
        chThdSleepMilliseconds(10000);

        for (speed = 20000; speed > -20000; speed -= 1000)
        {
//            setFakeSpeed(speed);
            setFakePwm(speed, PWM_STATE_NORMAL);
            dbgprintff("Fake normal PWM: %d\n", speed);
            chThdSleepMilliseconds(10000);
        }
    }
#else
    while (TRUE)
    {
        if (!shelltp)
        {
            if (SDU1.config->usbp->state == USB_ACTIVE)
            {
                /* Spawns a new shell.*/
                shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
            }
        }
        else
        {
            /* If the previous shell exited.*/
            if (chThdTerminated(shelltp))
            {
                /* Recovers memory of the previous shell.*/
                chThdRelease(shelltp);
                shelltp = NULL;
            }
        }
        spinLED();
        chThdSleepMilliseconds(500);
    }

#endif
}

