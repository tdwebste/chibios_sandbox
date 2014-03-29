/*
    Copyright (C) 2013 Genist Systems

*/

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "rtd.h"
#include "threads.h"
#ifdef CONFIG_RTD
adcsample_t avg_rtd;
adcsample_t avg_airtemp;
adcsample_t avg_sense;
adcsample_t avg_vrefint;

DECLARE_THREAD(ThreadRTD, NORMALPRIO, 512)

BinarySemaphore adcOutputBinSem;

msg_t printADCOutputThread(void *arg) {

    (void)arg;
  chRegSetThreadName("ADCOutputThread");

  while (TRUE) {
    /* wait for semaphore */
    if (chBSemWait(&adcOutputBinSem) == RDY_OK) {
      test_print(" rtd:");
      test_printn(avg_rtd);
      test_print("\r\n");

    }
    else {
      test_print("sem reset");
    }
  }
  return 0;
}

void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n);
void adc_err_cb(ADCDriver *adcp, adcerror_t err);


/* Total number of channels to be sampled by a single ADC operation.*/
#define ADC_GRP1_NUM_CHANNELS   4

/* Depth of the conversion buffer, channels are sampled four times each.*/
#define ADC_GRP1_BUF_DEPTH      4

/*
 * ADC samples buffer.
 */
static adcsample_t samples[ADC_GRP1_NUM_CHANNELS * ADC_GRP1_BUF_DEPTH];

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 4 samples of 2 channels, SW triggered.
 * Channels:    temp (ADC_IN16) recommended sampling time 17.1 uS (41.5 cycles sample time)
 *              Vrefint (ADC_IN17)  
 *
 *
 *              RCC configured PLL -> 72MHz -> AHB (/1) -> APB2 (/1) -> ADCPRE ( / 8 ) 9MHz
 *              
 * 
 */
static const ADCConversionGroup adcgrpcfg = {
    .circular     = FALSE,                   /* circular buffer */
    .num_channels = ADC_GRP1_NUM_CHANNELS,   /* num channels */
    .end_cb       = adccb,                   /* callback */
    .error_cb     = adc_err_cb,              /* error callback */
  /* HW dependent part.*/

  /*******************  Bit definition for ADC_CR1 register  ***************
  ADC_CR1_AWDCH       AWDCH[4:0] bits (Analog watchdog channel select bits)
  ADC_CR1_EOCIE       Interrupt enable for EOC
  ADC_CR1_AWDIE       Analog Watchdog interrupt enable
  ADC_CR1_JEOCIE      Interrupt enable for injected channels
  ADC_CR1_SCAN        Scan mode
  ADC_CR1_AWDSGL      Enable the watchdog on a single channel in scan mode
  ADC_CR1_JAUTO       Automatic injected group conversion
  ADC_CR1_DISCEN      Discontinuous mode on regular channels
  ADC_CR1_JDISCEN     Discontinuous mode on injected channels
  ADC_CR1_DISCNUM     DISCNUM[2:0] bits (Discontinuous mode channel count)
  ADC_CR1_DISCNUM_0   Bit 0
  ADC_CR1_DISCNUM_1   Bit 1
  ADC_CR1_DISCNUM_2   Bit 2
  ADC_CR1_JAWDEN      Analog watchdog enable on injected channels
  ADC_CR1_AWDEN       Analog watchdog enable on regular channels */
    .ll = {
        .adc = {
            .cr1 = 0,                            /* ADC CR1 reg */


  /*******************  Bit definition for ADC_CR2 register  ***************
  ADC_CR2_ADON        A/D Converter ON / OFF
  ADC_CR2_CONT        Continuous Conversion
  ADC_CR2_CAL         A/D Calibration
  ADC_CR2_RSTCAL      Reset Calibration
  ADC_CR2_DMA         Direct Memory access mode
  ADC_CR2_ALIGN       Data Alignment
  ADC_CR2_JEXTSEL     JEXTSEL[2:0] bits (External event select for injected group)
  ADC_CR2_JEXTTRIG    External Trigger Conversion mode for injected channels
  ADC_CR2_EXTSEL      EXTSEL[2:0] bits (External Event Select for regular group)
  ADC_CR2_EXTTRIG     External Trigger Conversion mode for regular channels
  ADC_CR2_JSWSTART    Start Conversion of injected channels
  ADC_CR2_SWSTART     Start Conversion of regular channels
  ADC_CR2_TSVREFE     Temperature Sensor and VREFINT Enable */
            .cr2 = ADC_CR2_EXTTRIG |  ADC_CR2_TSVREFE |
                   ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2 |
                   ADC_CR2_SWSTART,   /* ADC CR2 reg */

/******************  Bit definition for ADC_SMPR1 register  ************
  ADC_SMPR1_SMP10     SMP10[2:0] bits (Channel 10 Sample time selection)
  ADC_SMPR1_SMP11     SMP11[2:0] bits (Channel 11 Sample time selection)
  ADC_SMPR1_SMP12     SMP12[2:0] bits (Channel 12 Sample time selection)
  ADC_SMPR1_SMP13     SMP13[2:0] bits (Channel 13 Sample time selection)
  ADC_SMPR1_SMP14     SMP14[2:0] bits (Channel 14 Sample time selection)
  ADC_SMPR1_SMP15     SMP15[2:0] bits (Channel 15 Sample time selection)
  ADC_SMPR1_SMP16     SMP16[2:0] bits (Channel 16 Sample time selection)
  ADC_SMPR1_SMP17     SMP17[2:0] bits (Channel 17 Sample time selection) */

            .smpr1 = ADC_SMPR1_SMP_SENSOR(ADC_SAMPLE_239P5) | ADC_SMPR1_SMP_VREF(ADC_SAMPLE_239P5),

/******************  Bit definition for ADC_SMPR2 register  **********
  ADC_SMPR2_SMP0      SMP0[2:0] bits (Channel 0 Sample time selection)
  ADC_SMPR2_SMP1      SMP1[2:0] bits (Channel 1 Sample time selection)
  ADC_SMPR2_SMP2      SMP2[2:0] bits (Channel 2 Sample time selection)
  ADC_SMPR2_SMP3      SMP3[2:0] bits (Channel 3 Sample time selection)
  ADC_SMPR2_SMP4      SMP4[2:0] bits (Channel 4 Sample time selection)
  ADC_SMPR2_SMP5      SMP5[2:0] bits (Channel 5 Sample time selection)
  ADC_SMPR2_SMP6      SMP6[2:0] bits (Channel 6 Sample time selection)
  ADC_SMPR2_SMP7      SMP7[2:0] bits (Channel 7 Sample time selection)
  ADC_SMPR2_SMP8      SMP8[2:0] bits (Channel 8 Sample time selection)
  ADC_SMPR2_SMP9      SMP9[2:0] bits (Channel 9 Sample time selection) */
            .smpr2 = ADC_SMPR2_SMP_AN8(ADC_SAMPLE_239P5) | ADC_SMPR2_SMP_AN3(ADC_SAMPLE_239P5),  /* ADC SMPR2 reg */

/*******************  Bit definition for ADC_SQR1 register  **************
  ADC_SQR1_SQ13       SQ13[4:0] bits (13th conversion in regular sequence)
  ADC_SQR1_SQ14       SQ14[4:0] bits (14th conversion in regular sequence)
  ADC_SQR1_SQ15       SQ15[4:0] bits (15th conversion in regular sequence)
  ADC_SQR1_SQ16       SQ16[4:0] bits (16th conversion in regular sequence)
  ADC_SQR1_L          L[3:0] bits (Regular channel sequence length) */
            .sqr1 = ADC_SQR1_NUM_CH(ADC_GRP1_NUM_CHANNELS),   /* ADC SQR1 reg */

  /*******************  Bit definition for ADC_SQR2 register  **********
  ADC_SQR2_SQ7        SQ7[4:0] bits (7th conversion in regular sequence)
  ADC_SQR2_SQ8        SQ8[4:0] bits (8th conversion in regular sequence)
  ADC_SQR2_SQ9        SQ9[4:0] bits (9th conversion in regular sequence)
  ADC_SQR2_SQ10       SQ10[4:0] bits (10th conversion in regular sequence)
  ADC_SQR2_SQ11       SQ11[4:0] bits (11th conversion in regular sequence)
  ADC_SQR2_SQ12       SQ12[4:0] bits (12th conversion in regular sequence) */
            .sqr2 = 0, /* ADC SQR2 reg */

  /*************  Bit definition for ADC_SQR3 register  ****************
  ADC_SQR3_SQ1        SQ1[4:0] bits (1st conversion in regular sequence)
  ADC_SQR3_SQ2        SQ2[4:0] bits (2nd conversion in regular sequence)
  ADC_SQR3_SQ3        SQ3[4:0] bits (3rd conversion in regular sequence)
  ADC_SQR3_SQ4        SQ4[4:0] bits (4th conversion in regular sequence)
  ADC_SQR3_SQ5        SQ5[4:0] bits (5th conversion in regular sequence)
  ADC_SQR3_SQ6        SQ6[4:0] bits (6th conversion in regular sequence) */

            .sqr3 = ADC_SQR3_SQ1_N(ADC_CHANNEL_IN3)|
                    ADC_SQR3_SQ2_N(ADC_CHANNEL_IN8)|
                    ADC_SQR3_SQ3_N(ADC_CHANNEL_SENSOR)|
                    ADC_SQR3_SQ4_N(ADC_CHANNEL_VREFINT) /* ADC SQR3 reg */
        }
    }
};

void adc_err_cb(ADCDriver *adcp, adcerror_t err) {
  (void) adcp;
  test_print("ADC Error:");
  test_println(err ? "Overflow\n" : "DMA Failure\n");
}

/*
 * ADC end conversion callback.
 * Write temperature out to USART2
 */

#define AVG_SLOPE 4.3    /* obtained from 32f373xx datasheet */

void adccb(ADCDriver *adcp, adcsample_t *buffer, size_t n) {
    (void) adcp;
    (void) buffer;
    (void) n;

  /* Note, only in the ADC_COMPLETE state because the ADC driver fires an
     intermediate callback when the buffer is half full.*/

    /* Calculates the average values from the ADC samples.*/
    avg_rtd     = (samples[0] + samples[4] + samples[8]  + samples[12]) / 4;
    avg_airtemp = (samples[1] + samples[5] + samples[9]  + samples[13]) / 4;
    avg_sense   = (samples[2] + samples[6] + samples[10] + samples[14]) / 4;
    avg_vrefint = (samples[3] + samples[7] + samples[11] + samples[15]) / 4;

    chSysLockFromIsr();
    chBSemSignalI(&adcOutputBinSem);
    chSysUnlockFromIsr();

}

void testRTD(void)
{
  chBSemInit(&adcOutputBinSem, TRUE); /* init to not taken */
  
  /***
   ***   Sample the temperature from the on-board temp sensor
   ***
   ***
   ***/
  adcStart(&ADCD1, NULL);

  /*
   * Creates the example thread.
   */

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state, when the button is
   * pressed the test procedure is launched.
   */

  /* set console output destination */
  setStreamDest(&SD2);

  /* connect PT1000 / RTD pin to resistance bridge */
  palClearPad(RTD_PAD, RTD_PIN);

  while (TRUE) {
    adcStartConversion(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
    if (chBSemWait(&adcOutputBinSem) != RDY_OK) {
      chDbgPanic("ERROR - adcOutputBinSem reset");
    }

    /* calculate Temperature */
    chThdSleepMilliseconds(1000);
  }
}

static msg_t ThreadRTD(void *arg)
{
    (void) arg;
    chRegSetThreadName("rtd");
    testRTD();

    while (TRUE)
    {
        chThdSleepMilliseconds(1000);
    }
    return 0;
}
#endif
