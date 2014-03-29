/**
 * led.c
 *
 * This module controls the external LEDs
 *
 */
#include "ch.h"
#include "hal.h"
#include "led.h"
#include "db.h"
#include "threads.h"
#include "statemachine.h"
#include "genist_debug.h"

#define LED_PERIOD      1000
#define LED_COLOR_MASK  0x000000ff
#define LED_FULLSCALE   255


DECLARE_THREAD(ThreadBlinker, NORMALPRIO, 128)

#define LED_FREQUENCY      100000    /* 10 us resolution    */

#define BL_PATTERN_IND      0
#define BL_PATTERN_VAL      1
#define BL_PATTERN_MUL      2

#define BASE_VAL            (2)
#define BASE_MULT           (10)

static bool motionFlag = FALSE;

static int minLoad;
static int maxLoad;

static int timeon = 750;                 /* milliseconds, time led is on */
static int timeof = 750;                 /* milliseconds, time led is off */

static int led_state = 0;
static struct VirtualTimer vt;

static IndToColor indColor[] = {{LOAD_OK,LED_GREEN},{LOAD_UNDER,LED_BLUE},{LOAD_OVER,LED_RED} };

static MulToColor mulColor[] = {
    {MULT_100,LED_YELLOW},
    {MULT_1000,LED_CYAN},
    {MULT_10000,LED_PURPLE},
    {MULT_100000,LED_WHITE}
};

static ValToColor valColor[] = {
    {VAL_2,LED_YELLOW},
    {VAL_4,LED_CYAN},
    {VAL_6,LED_PURPLE},
    {VAL_8,LED_WHITE}
};

/* Blink code default values */
static Colorcode Blinkcode = {LED_BLUE,LED_PURPLE,LED_YELLOW};         /* indicator, value, multiplier */
static Timecode Blinkpattern[] = { {500,250,3}, {750,250,6},{500,250,3}};    /* indicator time on, indicator time off, value time on,... */

static uint32_t red = LED_RED;
static uint32_t green = LED_GREEN;
static uint32_t blue = LED_BLUE;

static void setBlinkPattern(uint32_t blinker, int div, int loops);

/**
 * virtual timer testing function
 */
void timer_handler(void *arg)
{
    (void)arg;
    if (led_state == 0)
    {
        LED_ON;
    }
    else
    {
        LED_OFF;
    }
    led_state ^= 1;
    chVTSetI(&vt, MS2ST(500), timer_handler, NULL);
}

/**
 * Turn LED on if color value is nonzero
 * @param pwmp
 */
static void LEDOn(PWMDriver *pwmp)
{
    (void)pwmp;
    if (red)
    {
        palSetPad(LED1_PAD, LED1_PIN); /* Before: palClearPad(LED1_PAD, LED1_PIN) */
    }
    if (green)
    {
        palSetPad(LED3_PAD, LED3_PIN);
    }
    if (blue)
    {
        palSetPad(LED2_PAD, LED2_PIN);
    }
}

static void LEDRedOff(PWMDriver *pwmp)
{
    (void)pwmp;
    palClearPad(LED1_PAD, LED1_PIN);

}

static void LEDGreenOff(PWMDriver *pwmp)
{
    (void)pwmp;
    palClearPad(LED3_PAD, LED3_PIN);
}
static void LEDBlueOff(PWMDriver *pwmp)
{
    (void)pwmp;
    palClearPad(LED2_PAD, LED2_PIN);

}

static PWMConfig LEDPwmConfig = {
        .frequency = LED_FREQUENCY,
        .period = LED_PERIOD,
        .callback = LEDOn,
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
 * Set motion flag
 * @param bool
 */
void setMotionFlag(bool motion)
{
    motionFlag = motion;

    if (motion == FALSE)
    {
        setBlinkPattern(LED_BLINK_ON_S,LED_BLINK_DIV_S, LED_BLINK_REP_S);
    }
    else
    {
        setBlinkPattern(LED_BLINK_OFF,LED_BLINK_DIV_M, LED_BLINK_REP_M);
    }
}
/**
 * Set LED color
 * @param color
 */
void setLEDColor(uint32_t color)
{

    red = (uint32_t)((color & LED_RED_MASK) >> LED_RED_SHIFT) * RED_MAX / LED_FULLSCALE;
    green = (uint32_t)((color & LED_GREEN_MASK) >> LED_GREEN_SHIFT) * GREEN_MAX / LED_FULLSCALE;
    blue = (uint32_t)((color & LED_BLUE_MASK) >> LED_BLUE_SHIFT) * BLUE_MAX / LED_FULLSCALE;

    pwmEnableChannel(&LEDX_PWM, LED1X_CHANNEL, red);
    pwmEnableChannel(&LEDX_PWM, LED2X_CHANNEL, blue);
    pwmEnableChannel(&LEDX_PWM, LED3X_CHANNEL, green);
}


/**
 * Set LED blinking pattern
 * @param pattern
 */
static void setBlinkPattern(uint32_t blinker, int div, int loops)
{
     timeof = (blinker & 0xFFFF0000) >> 16;
    timeon = (blinker & 0xFFFF) ;

    if (!div || (div > 10))
    {
        return;         /* use only 10 blinking styles */
    }
    if (!loops || (loops > 10))
    {
        return;           /* repeat maximum only 10 times */
    }
    Blinkpattern[BL_PATTERN_IND].timeon = timeon/div;
    Blinkpattern[BL_PATTERN_IND].timeof = timeof/div;
    Blinkpattern[BL_PATTERN_IND].repeat = loops;
    Blinkpattern[BL_PATTERN_VAL].timeon = timeon;
    Blinkpattern[BL_PATTERN_VAL].timeof = timeof;
    Blinkpattern[BL_PATTERN_VAL].repeat = loops;
    Blinkpattern[BL_PATTERN_MUL].timeon = timeon/div;
    Blinkpattern[BL_PATTERN_MUL].timeof = timeof/div;
    Blinkpattern[BL_PATTERN_MUL].repeat = loops;
}

/**
 * Set blink colors corresponding the value of load
 * @param load
  *
 */
uint8_t setBlinkCode(int load)
{
    int counter;
    int index, j,k;
    int prev, next, factor;
    INDICATOR indicator;
    VALUE value;
    MULTIPLIER multiplier;
    bool flag = FALSE;
    /* indicate load status using blinking LED */
    if (load < minLoad)
    {
        indicator = LOAD_UNDER;
     }
    else if (load > maxLoad)
    {
        indicator = LOAD_OVER;
    }
    else
    {
        indicator = LOAD_OK;
    }
    /* get the color corresponding the indicator */
    counter = sizeof(indColor)/sizeof (indColor[0]);

    for (index= 0; index < counter; index++ )
    {
        if (indColor[index].val == indicator)
        {
            Blinkcode.indicator = indColor[index].color;
            break;
        }
    }
    if (index == counter)
    {
        return 1;
    }
    /* converting load to value and multiplier */
    factor = BASE_MULT;                  /* decimal factor for load value */
    prev = BASE_VAL *  VAL_2 * factor * BASE_MULT;
    if (load < prev)
    {
        value = VAL_2;
        multiplier = MULT_100;
    }
    else
    {
        for (j = MULT_100 ; j <= MULT_100000; j++ )
        {
            factor *= BASE_MULT;
            for (k = VAL_2; k < VAL_10; k++)
            {
                next = (k * BASE_VAL) * factor;
                if ((load > prev) && (load <= next))
                {
                    value = k;
                    multiplier = j;
                    flag = TRUE;
                    break;
                }
                prev = next ;

            }
            if (flag)
            {
                break;
            }
        }
    }
    if (!flag)
    {
        value = VAL_8;
        multiplier = MULT_100000;
    }
    /* get the color corresponding the value */
    counter = sizeof(valColor)/sizeof (valColor[0]);
    for (index= 0; index < counter; index++ )
    {
        if (valColor[index].val == value)
        {
            Blinkcode.value = valColor[index].color;
            break;
        }
    }
    if (index == counter)
    {
        /* wrong argument value */
        return 1;
    }
    /* get the color corresponding the multiplier */
    counter = sizeof(mulColor)/sizeof (mulColor[0]);

    for (index= 0; index < counter; index++ )
    {
        if (mulColor[index].val == multiplier)
        {
            Blinkcode.multiplier = mulColor[index].color;
            break;
        }
    }
    if (index == counter)
    {
        return 1;
    }
    return 0;
}


/**
 * Initialize the LED driver
 */
void initLED(void)
{
    /* Power up LED */
    palSetPad(LED_STBY_PAD, LED_STBY_PIN);
    palSetPad(OUT_LEDPOWER_PORT, OUT_LEDPOWER_PIN);

    LEDPwmConfig.channels[LED1X_CHANNEL].mode = PWM_OUTPUT_ACTIVE_LOW;
    LEDPwmConfig.channels[LED2X_CHANNEL].mode = PWM_OUTPUT_ACTIVE_LOW;
    LEDPwmConfig.channels[LED3X_CHANNEL].mode = PWM_OUTPUT_ACTIVE_LOW;
    LEDPwmConfig.channels[LED1X_CHANNEL].callback = LEDRedOff;
    LEDPwmConfig.channels[LED2X_CHANNEL].callback = LEDBlueOff;
    LEDPwmConfig.channels[LED3X_CHANNEL].callback = LEDGreenOff;
    pwmStart(&LEDX_PWM, &LEDPwmConfig);
    LEDRedOff(&LEDX_PWM);
    LEDGreenOff(&LEDX_PWM);
    LEDBlueOff(&LEDX_PWM);

    minLoad = dbGetLowerFullWeight();
    maxLoad = dbGetUpperFullWeight();
    setBlinkPattern(LED_BLINK_ON_S,LED_BLINK_DIV_S, LED_BLINK_REP_S);
}

static msg_t ThreadBlinker(void *arg)
{
    int loop;
    uint32_t color;
    (void) arg;
    chRegSetThreadName("blinker");
    initLED();
//    setLEDColor(LED_PURPLE);
//    chVTSetI(&vt, MS2ST(5000), timer_handler, NULL);
//    return 0;
   
  	chThdSleepMilliseconds(1000);
    while (TRUE)
    {
        // use blinking pattern only on stationary mode 
        if (motionFlag == FALSE)
        {
            color = Blinkcode.indicator;
//            color = LED_WHITE;
            setLEDColor(color);
//            chprintf((BaseSequentialStream *)&CONSOLE, 
//            "SEND -- WHITE - Red:%d Blue:%d Green:%d \r\n",red,blue,green);
//        	chThdSleepMilliseconds(5000);
            loop = Blinkpattern[BL_PATTERN_IND].repeat;
            while (loop > 0)
            {
                if (Blinkpattern[BL_PATTERN_IND].timeon)
                {
                    palSetPad(LED_STBY_PAD, LED_STBY_PIN);
                    chThdSleepMilliseconds(Blinkpattern[BL_PATTERN_IND].timeon);
                }

                if (Blinkpattern[BL_PATTERN_IND].timeof)
                {
                    palClearPad(LED_STBY_PAD, LED_STBY_PIN);
                    chThdSleepMilliseconds(Blinkpattern[BL_PATTERN_IND].timeof);
                }
                loop-- ;
            }
 
            color = Blinkcode.value;
//            color = LED_GREEN;
            setLEDColor(color);
//            chprintf((BaseSequentialStream *)&CONSOLE, 
//            "SEND -- GREEN -- Red:%d Blue:%d Green:%d \r\n",red,blue,green);
//        	chThdSleepMilliseconds(2000);
            loop = Blinkpattern[BL_PATTERN_VAL].repeat;
            while (loop > 0)
            {
                if (Blinkpattern[BL_PATTERN_VAL].timeon)
                {
                    palSetPad(LED_STBY_PAD, LED_STBY_PIN);
                    chThdSleepMilliseconds(Blinkpattern[BL_PATTERN_VAL].timeon);
                }
                if (Blinkpattern[BL_PATTERN_VAL].timeof)
                {
                    palClearPad(LED_STBY_PAD, LED_STBY_PIN);
                    chThdSleepMilliseconds(Blinkpattern[BL_PATTERN_VAL].timeof);
                }
                loop-- ;
            }

            color = Blinkcode.multiplier;
//            color = LED_BLUE;
            setLEDColor(color);
//            chprintf((BaseSequentialStream *)&CONSOLE, 
//            "SEND -- BLUE -- Red:%d Blue:%d Green:%d \r\n",red,blue,green);
//        	chThdSleepMilliseconds(2000);
            loop = Blinkpattern[BL_PATTERN_MUL].repeat;
            while (loop > 0)
            {
                if (Blinkpattern[BL_PATTERN_MUL].timeon)
                {
                    palSetPad(LED_STBY_PAD, LED_STBY_PIN);
                    chThdSleepMilliseconds(Blinkpattern[BL_PATTERN_MUL].timeon);
                }

                if (Blinkpattern[BL_PATTERN_MUL].timeof)
                {
                    palClearPad(LED_STBY_PAD, LED_STBY_PIN);
                    chThdSleepMilliseconds(Blinkpattern[BL_PATTERN_MUL].timeof);
                }
                loop-- ;
            }

        } // no blinking pattern when in motion
        else {
            if (timeon)
            {
                palSetPad(LED_STBY_PAD, LED_STBY_PIN);
                chThdSleepMilliseconds(timeon);
            }

        }
        if (timeof)
        {
            palClearPad(LED_STBY_PAD, LED_STBY_PIN);
            chThdSleepMilliseconds(timeof);
        }

    }
    return 0; 
}

