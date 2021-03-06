/*
    Copyright (C) 2013 Genist

*/

/*
 * STM32F37x drivers configuration.
 * The following settings override the default settings present in
 * the various device driver implementation headers.
 * Note that the settings for each driver only have effect if the whole
 * driver is enabled in halconf.h.
 *
 * IRQ priorities:
 * 15...0       Lowest...Highest.
 *
 * DMA priorities:
 * 0...3        Lowest...Highest.
 */

#define STM32F30x_MCUCONF
#define HAL_USE_ICU         TRUE
#define HAL_USE_PWM         TRUE
#define HAL_USE_USB         TRUE
#define HAL_USE_SERIAL_USB  TRUE
/*
 * HAL driver system settings.
 */
#define STM32_NO_INIT                       FALSE
#define STM32_PVD_ENABLE                    FALSE
#define STM32_PLS                           STM32_PLS_LEV0
#define STM32_HSI_ENABLED                   TRUE
#define STM32_LSI_ENABLED                   TRUE
#define STM32_HSE_ENABLED                   TRUE
#define STM32_LSE_ENABLED                   FALSE
#define STM32_SW                            STM32_SW_PLL
#define STM32_PLLSRC                        STM32_PLLSRC_HSE
#define STM32_PREDIV_VALUE                  1
#define STM32_PLLMUL_VALUE                  9
#define STM32_HPRE                          STM32_HPRE_DIV1
#define STM32_PPRE1                         STM32_PPRE1_DIV2
#define STM32_PPRE2                         STM32_PPRE2_DIV1
#define STM32_MCOSEL                        STM32_MCOSEL_NOCLOCK
#define STM32_ADCPRE                        STM32_ADCPRE_DIV6
#define STM32_SDADCPRE                      STM32_SDADCPRE_DIV12
#define STM32_USART1SW                      STM32_USART1SW_PCLK
#define STM32_USART2SW                      STM32_USART2SW_PCLK
#define STM32_USART3SW                      STM32_USART3SW_PCLK
#define STM32_UART4SW                       STM32_UART4SW_PCLK
#define STM32_UART5SW                       STM32_UART5SW_PCLK
#define STM32_I2C1SW                        STM32_I2C1SW_SYSCLK
#define STM32_I2C2SW                        STM32_I2C2SW_SYSCLK
#define STM32_TIM1SW                        STM32_TIM1SW_PCLK2
#define STM32_TIM8SW                        STM32_TIM8SW_PCLK2
#define STM32_RTCSEL                        STM32_RTCSEL_LSI
#define STM32_USB_CLOCK_REQUIRED            TRUE
#define STM32_USBPRE                        STM32_USBPRE_DIV1P5

/*
 * CAN driver system settings.
 */
#define STM32_CAN_USE_CAN1                  FALSE
#define STM32_CAN_CAN1_IRQ_PRIORITY         11

/*
 * EXT driver system settings.
 */
#define STM32_EXT_EXTI0_IRQ_PRIORITY        6
#define STM32_EXT_EXTI1_IRQ_PRIORITY        6
#define STM32_EXT_EXTI2_IRQ_PRIORITY        6
#define STM32_EXT_EXTI3_IRQ_PRIORITY        6
#define STM32_EXT_EXTI4_IRQ_PRIORITY        6
#define STM32_EXT_EXTI5_9_IRQ_PRIORITY      6
#define STM32_EXT_EXTI10_15_IRQ_PRIORITY    6
#define STM32_EXT_EXTI16_IRQ_PRIORITY       6
#define STM32_EXT_EXTI17_IRQ_PRIORITY       6
#define STM32_EXT_EXTI18_IRQ_PRIORITY       6
#define STM32_EXT_EXTI19_IRQ_PRIORITY       6
#define STM32_EXT_EXTI20_23_IRQ_PRIORITY    6
#define STM32_EXT_EXTI30_32_IRQ_PRIORITY    6
#define STM32_EXT_EXTI33_IRQ_PRIORITY       6

/*
 * GPT driver system settings.
 */
#define STM32_GPT_USE_TIM1                  FALSE
#define STM32_GPT_USE_TIM2                  FALSE
#define STM32_GPT_USE_TIM3                  FALSE
#define STM32_GPT_USE_TIM4                  FALSE
#define STM32_GPT_USE_TIM8                  FALSE
#define STM32_GPT_USE_TIM19                 FALSE
#define STM32_GPT_TIM1_IRQ_PRIORITY         7
#define STM32_GPT_TIM2_IRQ_PRIORITY         7
#define STM32_GPT_TIM3_IRQ_PRIORITY         7
#define STM32_GPT_TIM4_IRQ_PRIORITY         7
#define STM32_GPT_TIM8_IRQ_PRIORITY         7

/*
 * ICU driver system settings.
 */
#define STM32_ICU_USE_TIM1                  FALSE
#define STM32_ICU_USE_TIM2                  FALSE
#define STM32_ICU_USE_TIM3                  FALSE
#define STM32_ICU_USE_TIM4                  FALSE
#define STM32_ICU_USE_TIM8                  TRUE
#define STM32_ICU_USE_TIM19                 FALSE
#define STM32_ICU_TIM1_IRQ_PRIORITY         7
#define STM32_ICU_TIM2_IRQ_PRIORITY         7
#define STM32_ICU_TIM3_IRQ_PRIORITY         7
#define STM32_ICU_TIM4_IRQ_PRIORITY         7
#define STM32_ICU_TIM8_IRQ_PRIORITY         7

/*
 * PWM driver system settings.
 */
#define STM32_PWM_USE_ADVANCED              FALSE
#define STM32_PWM_USE_TIM1                  FALSE
#define STM32_PWM_USE_TIM2                  FALSE
#define STM32_PWM_USE_TIM3                  TRUE
#define STM32_PWM_USE_TIM4                  TRUE
#define STM32_PWM_USE_TIM8                  FALSE
#define STM32_PWM_TIM1_IRQ_PRIORITY         7
#define STM32_PWM_TIM2_IRQ_PRIORITY         7
#define STM32_PWM_TIM3_IRQ_PRIORITY         7
#define STM32_PWM_TIM4_IRQ_PRIORITY         7
#define STM32_PWM_TIM8_IRQ_PRIORITY         7

/*
 * SERIAL driver system settings.
 */

#define STM32_SERIAL_USE_USART1             TRUE
#define STM32_SERIAL_USE_USART2             TRUE
#define STM32_SERIAL_USE_USART3             FALSE
#define STM32_SERIAL_USE_UART4              FALSE
#define STM32_SERIAL_USE_UART5              FALSE
#define STM32_SERIAL_USART1_PRIORITY        12
#define STM32_SERIAL_USART2_PRIORITY        12
#define STM32_SERIAL_USART3_PRIORITY        12
#define STM32_SERIAL_UART4_PRIORITY         12
#define STM32_SERIAL_UART5_PRIORITY         12

/*
 * SPI driver system settings.
 */
#define STM32_SPI_USE_SPI1                  TRUE
#define STM32_SPI_USE_SPI2                  FALSE
#define STM32_SPI_USE_SPI3                  FALSE
#define STM32_SPI_SPI1_DMA_PRIORITY         1
#define STM32_SPI_SPI2_DMA_PRIORITY         1
#define STM32_SPI_SPI3_DMA_PRIORITY         1
#define STM32_SPI_SPI1_IRQ_PRIORITY         10
#define STM32_SPI_SPI2_IRQ_PRIORITY         10
#define STM32_SPI_SPI3_IRQ_PRIORITY         10
#define STM32_SPI_DMA_ERROR_HOOK(spip)      chSysHalt()

//#define STM32_SPI_SPI1_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1,2)
//#define STM32_SPI_SPI1_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1,3)

//#define STM32_SPI_SPI2_RX_DMA_STREAM        STM32_DMA_STREAM_ID(1,4)
//#define STM32_SPI_SPI2_TX_DMA_STREAM        STM32_DMA_STREAM_ID(1,5)

//#define STM32_SPI_SPI3_RX_DMA_STREAM        STM32_DMA_STREAM_ID(2,1)
//#define STM32_SPI_SPI3_TX_DMA_STREAM        STM32_DMA_STREAM_ID(2,2)
/*
 * UART driver system settings.
 */
#define STM32_UART_USE_USART1               FALSE
#define STM32_UART_USE_USART2               FALSE
#define STM32_UART_USE_USART3               FALSE
#define STM32_UART_USART1_IRQ_PRIORITY      12
#define STM32_UART_USART2_IRQ_PRIORITY      12
#define STM32_UART_USART3_IRQ_PRIORITY      12
#define STM32_UART_USART1_DMA_PRIORITY      0
#define STM32_UART_USART2_DMA_PRIORITY      0
#define STM32_UART_USART3_DMA_PRIORITY      0
#define STM32_UART_DMA_ERROR_HOOK(uartp)    chSysHalt()

#define STM32_UART_USART2_RX_DMA_STREAM     STM32_DMA_STREAM_ID(1,6)
#define STM32_UART_USART2_TX_DMA_STREAM     STM32_DMA_STREAM_ID(1,7)

/*
 * USB driver system settings.
 */
#define STM32_USB_USE_USB1                  TRUE
#define STM32_USB_LOW_POWER_ON_SUSPEND      FALSE
#define STM32_USB_USB1_HP_IRQ_PRIORITY      13
#define STM32_USB_USB1_LP_IRQ_PRIORITY      14

/*
 * ADC driver system settings.
 */
#define STM32_ADC_USE_ADC1                  TRUE
#define STM32_ADC_ADC1_DMA_STREAM           STM32_DMA_STREAM_ID(1, 1)
#define STM32_ADC_ADC1_DMA_PRIORITY         2
#define STM32_ADC_ADC1_IRQ_PRIORITY         6
#define STM32_ADC_ADC1_DMA_IRQ_PRIORITY     6

/*
 * SDADC driver system settings.
 */
#define STM32_ADC_USE_SDADC1                TRUE
#define STM32_ADC_SDADC1_DMA_STREAM         STM32_DMA_STREAM_ID(2, 3)
#define STM32_ADC_SDADC1_DMA_PRIORITY       3
#define STM32_ADC_SDADC1_IRQ_PRIORITY       6
#define STM32_ADC_SDADC1_DMA_IRQ_PRIORITY   6

#define STM32_ADC_USE_SDADC2                TRUE
#define STM32_ADC_SDADC2_DMA_STREAM         STM32_DMA_STREAM_ID(2, 4)
#define STM32_ADC_SDADC2_DMA_PRIORITY       3
#define STM32_ADC_SDADC2_IRQ_PRIORITY       6
#define STM32_ADC_SDADC2_DMA_IRQ_PRIORITY   6

#define STM32_ADC_USE_SDADC3                TRUE
#define STM32_ADC_SDADC3_DMA_STREAM         STM32_DMA_STREAM_ID(2, 5)
#define STM32_ADC_SDADC3_DMA_PRIORITY       3
#define STM32_ADC_SDADC3_IRQ_PRIORITY       6
#define STM32_ADC_SDADC3_DMA_IRQ_PRIORITY   6

/* GENIST Configuration */

/* PWM timer configuration for fake speed */
#define FAKE_SPEED          PWMD3
#define FAKE_SPEED1_CHANNEL 0
#define FAKE_SPEED2_CHANNEL 1

/* GPIO configuration for fake speed */
#define FAKE_SPEED1_PAD     GPIOB
#define FAKE_SPEED1_PIN     GPIOB_TEST_SPEED1_OUT
#define FAKE_SPEED2_PAD     GPIOB
#define FAKE_SPEED2_PIN     GPIOB_TEST_SPEED2_OUT

/* PWM timer configuration for fake PWM */
#define FAKE_PWM            PWMD4
#define FAKE_PWM_CHANNEL    2

/* GPIO configuration for fake PWM */
#define FAKE_PWM_PAD        GPIOB
#define FAKE_PWM_PIN        GPIOB_TEST_PWM_OUT

#define CONSOLE             SDU1
#define CONSOLE_RELAY       SD1
