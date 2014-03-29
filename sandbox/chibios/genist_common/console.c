/*
 Copyright (C) 2013 Genist
 */

#include <stdlib.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "test.h"
#include "ms5803.h"
#include "rtd.h"
#include "led.h"
#include "genist_debug.h"
#include "threadlist.h"
#include "inputs.h"
#include "statemachine.h"
#include "shell.h"
#include "db.h"
#define SHELL_WA_SIZE   THD_WA_SIZE(2048)
#define TEST_WA_SIZE    THD_WA_SIZE(256)

/**
 * debug print enable flag
 */
uint8_t dbgprint_enable = FALSE;
/**
 * Display memory status
 * @param chp
 * @param argc
 * @param argv
 */
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

/**
 * Display thread status
 * @param chp
 * @param argc
 * @param argv
 */
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
                (uint32_t)(tp->p_refs - 1), states[tp->p_state],
                (uint32_t) tp->p_time, tp->p_name);
        tp = chRegNextThread(tp);
    } while (tp != NULL);
}

#if 0
/**
 * Run Chibios test suite
 * @param chp
 * @param argc
 * @param argv
 */
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
    if (tp == NULL)
    {
        chprintf(chp, "out of memory\r\n");
        return;
    }
    chThdWait(tp);
}
#endif
extern void SetPressure(SPIDriver *spip, int32_t val);
extern void SetAirTemp(SPIDriver *spip, int32_t val);

/**
 * database operations
 * @param chp
 * @param argc
 * @param argv
 */
static void cmd_db(BaseSequentialStream *chp, int argc, char *argv[])
{
    uint8_t status = 0;
    int32_t p1=0;
    int32_t p2=0;
    char *endptr;

    if (argc < 1)
    {
        chprintf(chp, "Usage: db save                      - save database\r\n");
        chprintf(chp, "       db restore                   - restore database\r\n");
        chprintf(chp, "       db set <index/name> <value>  - set database item\r\n");
        chprintf(chp, "       db get <index/name>          - get database item\r\n");
        chprintf(chp, "       db list                      - list database items\r\n");
        chprintf(chp, "       db json                      - list database as JSON string\r\n");
        return;
    }
    if (argc > 1)
    {
        p1 = strtol(argv[1], &endptr, 0);
        if (*endptr != 0)
        {
            p1 = dbLookup(argv[1]);
            if (p1 < 0)
            {
                chprintf(chp, "Invalid index %s\r\n", argv[1]);
                return;
            }
        }
    }

    if (!strcmp(argv[0], "save"))
    {
        status = dbSave();
    }
    else if (!strcmp(argv[0], "restore"))
    {
        status = dbRestore();
    }
    else if (!strcmp(argv[0], "get"))
    {
        if (argc > 1)
        {
            chprintf(chp, "%s = %d\r\n", dbGetString(p1), dbGetInt(p1));
        }
        else
        {
            chprintf(chp, "error: missing index\r\n");
        }
    }
    else if (!strcmp(argv[0], "set"))
    {
        if (argc > 2)
        {
            p2 = strtol(argv[2], &endptr, 0);
            if (*endptr != 0)
            {
                chprintf(chp, "Invalid value %s\r\n", argv[2]);
                return;
            }

            dbSetInt(p1, p2);
            chprintf(chp, "%s = %d\r\n", dbGetString(p1), dbGetInt(p1));
        }
        else
        {
            chprintf(chp, "error: missing index and/or value\r\n");
        }
    }
    else if (!strcmp(argv[0], "list"))
    {
        p1 = 0;
        while (dbGetString(p1))
        {
            chprintf(chp, "%2d: %s = %d\r\n", p1, dbGetString(p1), dbGetInt(p1));
            p1++;
        }
    }
    else if (!strcmp(argv[0], "json"))
    {
        cJSON *dbObject = dbGetJSON("\"Config\"");
        char *json = cJSON_Print(dbObject);
        chprintf(chp, "%s\n", json);
        chHeapFree(json);
        cJSON_Delete(dbObject);
    }
    else
    {
        chprintf(chp, "Invalid command %s\r\n", argv[0]);
        return;
    }
    if (status != 0)
    {
        chprintf(chp, "db error: %d\r\n", status);
    }
}

/**
 * Display or override load pressure
 * @param chp
 * @param argc
 * @param argv
 */
static void cmd_pressure(BaseSequentialStream *chp, int argc, char *argv[])
{
    SPIDriver *dev;
    char *endptr;
    int32_t val;
    if (argc < 1)
    {
        chprintf(chp, "Usage: pressure <device>            - read value\r\n");
        chprintf(chp, "       pressure <device> [<value>]  - set value\r\n");
        chprintf(chp, "       pressure <device> disable    - disable override\r\n");
        chprintf(chp, "\r\n<device> = load / supply\r\n");
        return;
    }
    if (!strcmp(argv[0], "load"))
    {
        dev = &LPRESSURE_SPI;
    }
    else if (!strcmp(argv[0], "supply"))
    {
        dev = &SPRESSURE_SPI;
    }
    else
    {
        chprintf(chp, "Invalid device %s\r\n", argv[0]);
        return;
    }
    if (argc == 1)
    {
        chprintf(chp,"Pressure = %d Temp = %f Stat = %d\r\n", ReadPressure(dev), ReadPressureTemp(dev), ReadPressureStat(dev));
    }
    else
    {
        if (!strcmp(argv[1], "disable"))
        {
            SetPressure(dev, -100000);
        }
        else
        {
            val = strtol(argv[1], &endptr, 0);
            if (*endptr == 0)
            {
                SetPressure(dev, val);
            }
            else
            {
                chprintf(chp, "Invalid value %s\r\n", argv[1]);
            }
        }
    }
}

/**
 * Display temperature
 * @param chp
 * @param argc
 * @param argv
 */
static void cmd_temp(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argv;
    if (argc >= 1)
    {
        chprintf(chp, "Usage: temp                        - read temperature values\r\n");
        chprintf(chp, "\r\n<device> = air / pwm\r\n");
        return;
    }

    chprintf(chp,"Sense 2    = %f (%f)\r\n", ReadSense2, ReadSense2Voltage());
    chprintf(chp,"AirTemp    = %f\r\n", ReadAirTemp());
    chprintf(chp,"PwmTemp    = %f (%f)\r\n", ReadPWMTemp(), ReadPWMVoltage());
    chprintf(chp,"SupplyTemp = %f\r\n", ReadPressureTemp(&SPRESSURE_SPI));
    chprintf(chp,"LoadTemp   = %f\r\n", ReadPressureTemp(&LPRESSURE_SPI));
    chprintf(chp,"Supply Pressure  = %d\r\n", ReadPressure(&SPRESSURE_SPI));
    chprintf(chp,"Load Pressure  = %d\r\n", ReadPressure(&LPRESSURE_SPI));
}

/**
 * Display voltage
 * @param chp
 * @param argc
 * @param argv
 */
static void cmd_voltage(BaseSequentialStream *chp, int argc, char *argv[])
{
    char *endptr;
    int channel;
    if (argc < 1)
    {
        chprintf(chp, "Usage: voltage <channel>            - display voltage\r\n");
        return;
    }
    channel = strtol(argv[0], &endptr, 0);
    if (!((*endptr == 0) && (channel >= 0) && (channel <= 8)))
    {
        chprintf(chp, "Invalid channel %s\r\n", argv[0]);
        return;
    }


    chprintf(chp,"Channel %d: %f V\r\n", channel, GetVoltage(channel));
}

/**
 * Display speed
 * @param chp
 * @param argc
 * @param argv
 */
/*
static void cmd_speed(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void)argv;
    if (argc >= 1)
    {
        chprintf(chp, "Usage: speed                         - read speed values\r\n");
        return;
    }
    chprintf(chp,"%s Speed = %f\r\n", GetSpeedSensorID(), ReadSpeed());
}
*/

/**
 * Set LED colour
 * @param chp
 * @param argc
 * @param argv
 */
static void cmd_led(BaseSequentialStream *chp, int argc, char *argv[])
{
    char *endptr;
    uint32_t color;
    if (argc < 1)
    {
        chprintf(chp, "Usage: led <color>                  - change LED color\r\n");
        return;
    }
    color = strtol(argv[0], &endptr, 0);
    if (*endptr != 0)
    {
        chprintf(chp, "Invalid color %s\r\n", argv[0]);
        return;
    }
    setLEDColor(color);
}

/**
 * Set light
 * @param chp
 * @param argc
 * @param argv
 */
static void cmd_light(BaseSequentialStream *chp, int argc, char *argv[])
{
    char *lighton;
    char entry;
    uint32_t color;
    if (argc < 1)
    {
        chprintf(chp, "Usage: light on|off|red|blue|green|yellow|purple         - turn light ON|OFF|coor \r\r\n");
        return;
    }
    lighton = argv[0];
    entry = *argv[0];
    if (!strcmp(lighton,"on"))
    {
    	LED_ON;
    	return;
    }
    else if (!strcmp(lighton,"off"))
    {
    	LED_OFF;
    	return;
    }

    if (entry == 'r')  /* red */
    {
    	color = LED_RED;
    }
    else if (entry == 'b')
    {
    	color = LED_BLUE;
    }
    else if (entry == 'y')
    {
    	color = LED_YELLOW;
    }

    else if (entry == 'g')
    {
    	color = LED_GREEN;
    }

    else if (entry == 'p')
    {
    	color = LED_PURPLE;
    }

    else
    {
        chprintf(chp, "Invalid color %s\r\n", argv[0]);
        return;
    }
    setLEDColor(color);
    return;
}

/**
 * Set LED blinking pattern
 * @param chp
 * @param argc
 * @param argv
 */
static void cmd_blink(BaseSequentialStream *chp, int argc, char *argv[])
{
    char *endptr;
    uint32_t blinker;

    if (argc < 1)
    {
        chprintf(chp, "Usage: blink <0|1>                  - change LED blink pattern for stationary|mobile \r\n");
        return;
    }
    blinker = strtol(argv[0], &endptr, 0);
    if (*endptr != 0)
    {
        chprintf(chp, "Invalid entry %s\r\n", argv[0]);
        return;
    }
    if (blinker)
    {
    	setMotionFlag(TRUE);
    }
    else
    {
    	setMotionFlag(FALSE);
    }
}

static const ShellCommand commands[] =
{
{ "mem", cmd_mem },
{ "threads", cmd_threads },
{ "pressure", cmd_pressure },
//{ "speed", cmd_speed },
{ "temp", cmd_temp },
//{ "axle", cmd_axle },
{ "voltage", cmd_voltage },
{ "led", cmd_led },
{ "db", cmd_db },
{ "light",cmd_light },
{ "blink",cmd_blink },
{ NULL, NULL } };

static const ShellConfig shell_cfg1 =
{ (BaseSequentialStream *) &CONSOLE, commands };
/*
 * Application entry point.
 */
Thread *startConsole(void)
{
    dbgprint_enable = TRUE;
    return shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
}

