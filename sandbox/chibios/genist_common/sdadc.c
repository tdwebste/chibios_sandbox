/**
 * sdadc.c
 *
 * This module handles all inputs into the SDADC.
 *
 */
#include <string.h>
#include "genist_debug.h"

#include "chprintf.h"
#include <float.h>
#include "ch.h"
#include "hal.h"
#include "threads.h"
#include "ertj1vt302j.h"
#include "pwm_rheintacho.h"

/* Common settings for SDADC */
#define SDADC_CR1_REFV_EXT          0
#define SDADC_CR1_REFV_INT_1_2V     SDADC_CR1_REFV_0
#define SDADC_CR1_REFV_INT_1_8V     SDADC_CR1_REFV_1
#define SDADC_CR1_REFV_VDDSD        (SDADC_CR1_REFV_1 | SDADC_CR1_REFV_0)

#define SDADC_GAIN_SINGLE_ENDED SDADC_CONFR_GAIN_1X         /**< Internal gain 1 is selected */
#define SDADC_GAIN_DIFFERENTIAL SDADC_CONFR_GAIN_4X         /**< Internal gain 8 is selected */
#define GAIN_MULT_SINGLE_ENDED  1                           /**< SDADC internal gain. Must match above */
#define GAIN_MULT_DIFFERENTIAL  4                           /**< SDADC internal gain. Must match above */
#define SDADC_CR1_VREF          SDADC_CR1_REFV_INT_1_2V     /**< SDADC VREF setting = 1.8V */
#define SDADC_VREF              1.2f                        /**< SDADC VREF = 1.8V */
#define SDADC_INIT_TIMEOUT      30                          /**< about two SDADC clock cycles after INIT is set */
#define SDADC_RESOLUTION        32767                       /**< full scale resolution 2e15 - 1 */
//#define SDADC_BUFFER_SIZE       32
#define SDADC_BUFFER_SIZE       2        /* zaki is testing a smaller buffer */
#define SDADC_CFG_SINGLE_ENDED  0
#define SDADC_CFG_DIFFERENTIAL  1
/* #define SDADC_NUM_CHANNELS      3 */
#define SDADC_NUM_CHANNELS      3    /* zaki */
#define SENSE2_SDADC_CHANNEL    0
#define AIRTEMP_SDADC_CHANNEL   2
#define PWM_SDADC_CHANNEL       8

#define INVALID_CHANNEL         0xff
DECLARE_THREAD(ThreadSDADC, NORMALPRIO, 1024)

static void sdadcEndCallback(ADCDriver *adcdp, adcsample_t *buffer, size_t n);
BinarySemaphore sdadcBinSem;

/**
 * No conversion
 * @param v
 * @return
 */
static float SenseConvert(float v)
{
    //return v * 1000;
    return v;
}

/**
 * Convert PWM voltage to temperature
 * @param v
 * @return
 */
static float Channel8Convert(float v)
{
    return PWMVoltageToTemperature(v);
}

/**
 * Convert ERT voltage to temperature
 * @param v
 * @return
 */
static float Channel2Convert(float v)
{
    return ERTVoltageToTemperature(v);
}

typedef struct
{
    uint8_t channel;
    uint8_t gain;
    int32_t sum;
    float (*conv)(float v);
} SDADCChannel;

/**
 * Array of samples for all defined SDADC channels. Samples are interleaved
 * in descending order according to injected conversion order.
 */
static int16_t SampleArray[SDADC_BUFFER_SIZE][SDADC_NUM_CHANNELS];

/**
 * List of SDADC channels. The list must be in ascending channel order, which
 * corresponds to the injected conversion order.
 */
SDADCChannel SDADCChannelList[] =
{
    {
        .channel = 0,
        .gain = GAIN_MULT_SINGLE_ENDED,
        .sum = 0,
        .conv = SenseConvert
    },
    {
        .channel = 2,
        .gain = GAIN_MULT_SINGLE_ENDED,
        .sum = 0,
        .conv = Channel2Convert
    },
    {
        .channel = 8,
        .gain = GAIN_MULT_DIFFERENTIAL,
        .sum = 0,
        .conv = Channel8Convert
    },
    {
        .channel = 0xff,
        .gain = 0,
        .sum = 0,
        .conv = NULL
    }
};

/**
 * Get array index for the given channel
 * @param channel
 * @return index
 */
uint8_t GetChannelIndex(uint8_t channel)
{
    uint8_t ret = 0;
    while (SDADCChannelList[ret].channel != INVALID_CHANNEL)
    {
        if (SDADCChannelList[ret].channel == channel)
        {
            return ret;
        }
        ret++;
    }
    return INVALID_CHANNEL;
}
/**
 * ADC error callback function
 * @param adcdp
 * @param err
 */
static void adc_err_cb(ADCDriver *adcdp, adcerror_t err)
{
    (void) adcdp;
    (void) err;
    chDbgPanic("ADC Error:Overflow, DMA Failure");
}

/**
 * SDADC configuration groups
 */
static ADCConfig sdadcConfig =
{

0,        /* Internal VDDSD reference */
//    .cr1 = SDADC_CR1_VREF,
//    .confxr =
    {
        /* Config 0: Single ended configuration */
        SDADC_CONFR_COMMON_VSSSD | SDADC_CONFR_SE_OFFSET | SDADC_GAIN_SINGLE_ENDED,
        /* Config 1: Differential configuration */
        SDADC_CONFR_COMMON_VSSSD | SDADC_CONFR_SE_DIFF | SDADC_GAIN_DIFFERENTIAL,
        /* Config 2: Not used */
        0
    }
};

/**
 * Callback function when conversion is complete. Note that this is called
 * through the DMA interrupt, not the ADC interrupt.
 * @param adcp
 * @param buffer
 * @param n
 */
static void sdadcEndCallback(ADCDriver *adcp, adcsample_t *buffer, size_t n)
{
    (void) buffer;
    (void) n;
    /* Note, only in the ADC_COMPLETE state because the ADC driver fires an
     intermediate callback when the buffer is half full.*/
    if (adcp->state == ADC_COMPLETE)
    {
        chSysLockFromIsr()
        ;
        chBSemSignalI(&sdadcBinSem);
        chSysUnlockFromIsr();
    }
}

/**
 * SDADC conversion group.
 * Channels used:
 *
 * Channel 2 - air temp sensor
 * Channel 8 - PWM temp sensor
 */
ADCConversionGroup sdadcConvGroup =
{
    .circular = FALSE, /* circular buffer */
    .num_channels = SDADC_NUM_CHANNELS, /* number of channels */
    .end_cb = sdadcEndCallback, /* callback */
    .error_cb = adc_err_cb, /* error callback */

    .u =
    {
        .sdadc =
        {
            /* CR2 */
            .cr2 = SDADC_CR2_ADON | SDADC_CR2_JSWSTART,
            .jchgr = SDADC_JCHG(0) | SDADC_JCHG(2) | SDADC_JCHG(8),
            .confchr =
            {
                SDADC_CONFCHR1_CH0(SDADC_CFG_SINGLE_ENDED) |
                SDADC_CONFCHR1_CH2(SDADC_CFG_SINGLE_ENDED),
                SDADC_CONFCHR2_CH8(SDADC_CFG_DIFFERENTIAL)
            }
        }
    }
};

/**
 * Thread to launch A/D conversions and analyze results.
 * @param arg
 * @return
 */
msg_t ThreadSDADC(void *arg)
{
    uint16_t i, j;
    SDADCChannel *pbuf;
    (void) arg;
    chRegSetThreadName("SDADCs");
    chBSemInit(&sdadcBinSem, TRUE); /* init to taken */

    /* Calibrate offsets */
    adcStart(&SDADCD1, &sdadcConfig);
    adcSTM32Calibrate(&SDADCD1);

    /* Clear buffers */
    for (i = 0; i < SDADC_NUM_CHANNELS; i++)
    {
        SDADCChannelList[i].sum = 0;
    }

    while (1)
    {
        /* Convert all channels */
        adcStartConversion(&SDADCD1, &sdadcConvGroup, (adcsample_t *)&SampleArray[0][0], SDADC_BUFFER_SIZE);

        if (chBSemWait(&sdadcBinSem) == RDY_OK)
        {
            i = 0;
            /*
             * Iterate through interleaved samples and calculate the sum
             * for each channel.
             */
            while (SDADCChannelList[i].channel != INVALID_CHANNEL)
            {
                pbuf = &SDADCChannelList[i];
                chSysLock();
                pbuf->sum = 0;
                for (j = 0; j < SDADC_BUFFER_SIZE; j++)
                {
                    pbuf->sum += SampleArray[j][i];
                }
                chSysUnlock();
                i++;
            }
        } else
        {
            chDbgPanic("ERROR - reg conversion sem reset");
        }
    }
    return 0;
}

/**
 * Get float value of channel.
 * @param channel
 * @return
 */
float GetValue(uint8_t channel)
{
    float ret;
    SDADCChannel *pbuf;
    uint8_t index = GetChannelIndex(channel);
    if (index == INVALID_CHANNEL)
    {
        port_halt();
    }
    pbuf = &SDADCChannelList[index];
    chSysLock();
    ret = (float) pbuf->sum * SDADC_VREF / SDADC_RESOLUTION / (pbuf->gain * 2)
            / SDADC_BUFFER_SIZE;
    chSysUnlock();
    return pbuf->conv(ret);
}

/**
 * Get voltage value of channel.
 * @param channel
 * @return
 */
float GetVoltage(uint8_t channel)
{
    float ret;
    SDADCChannel *pbuf;
    uint8_t index = GetChannelIndex(channel);
    if (index == INVALID_CHANNEL)
    {
        return -10000.0f;
    }
    pbuf = &SDADCChannelList[index];
    chSysLock();
    ret = (float) pbuf->sum * SDADC_VREF / SDADC_RESOLUTION / (pbuf->gain * 2)
            / SDADC_BUFFER_SIZE;
    chSysUnlock();
    return ret;
}

/**
 * Get the PWM sample average and convert it to temperature.
 * @return
 */
float ReadSense2(void)
{
    return GetValue(SENSE2_SDADC_CHANNEL);
}


float ReadSense2Voltage(void)
{
    return GetVoltage(SENSE2_SDADC_CHANNEL);
}


/**
 * Get the PWM sample average and convert it to temperature.
 * @return
 */
float ReadPWMTemp(void)
{
    return GetValue(PWM_SDADC_CHANNEL);
}

float ReadPWMVoltage(void)
{
    return GetVoltage(PWM_SDADC_CHANNEL);
}

/**
 * Get the Air Temp sample average and convert it to temperature.
 * @return
 */
float ReadAirTemp(void)
{
    return GetValue(AIRTEMP_SDADC_CHANNEL);
}
