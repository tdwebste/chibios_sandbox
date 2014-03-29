/*
    Copyright (C) 2013 Genist

*/

/*
 * STM32F37x driver configuration for Genist V2 board.
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

#define STM32F37x_MCUCONF
#define HAL_USE_ICU         TRUE
#define HAL_USE_PWM         TRUE
#define HAL_USE_COMP        TRUE

/*
 * HAL driver system settings.
 */
#define STM32_NO_INIT                       FALSE
#define STM32_PVD_ENABLE                    FALSE
#define STM32_PLS                           STM32_PLS_LEV0
#define STM32_HSI_ENABLED                   TRUE
#define STM32_LSI_ENABLED                   TRUE
#define STM32_HSE_ENABLED                   FALSE
#define STM32_LSE_ENABLED                   FALSE
#define STM32_SW                            STM32_SW_PLL
#define STM32_PLLSRC                        STM32_PLLSRC_HSI
#define STM32_PREDIV_VALUE                  1
//#define STM32_PLLMUL_VALUE                  9 // crystal
#define STM32_PLLMUL_VALUE                  16
#define STM32_HPRE                          STM32_HPRE_DIV1
#define STM32_PPRE1                         STM32_PPRE1_DIV2
#define STM32_PPRE2                         STM32_PPRE2_DIV1
#define STM32_MCOSEL                        STM32_MCOSEL_NOCLOCK
#define STM32_ADCPRE                        STM32_ADCPRE_DIV6
#define STM32_SDPRE                         STM32_SDPRE_DIV12
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
#define STM32_GPT_USE_TIM15                 FALSE
#define STM32_GPT_USE_TIM16                 FALSE
#define STM32_GPT_USE_TIM17                 FALSE
#define STM32_GPT_USE_TIM19                 FALSE
#define STM32_GPT_TIM1_IRQ_PRIORITY         7
#define STM32_GPT_TIM2_IRQ_PRIORITY         7
#define STM32_GPT_TIM3_IRQ_PRIORITY         7
#define STM32_GPT_TIM4_IRQ_PRIORITY         7
#define STM32_GPT_TIM8_IRQ_PRIORITY         7
#define STM32_GPT_TIM15_IRQ_PRIORITY        7
#define STM32_GPT_TIM16_IRQ_PRIORITY        7
#define STM32_GPT_TIM17_IRQ_PRIORITY        7
#define STM32_GPT_TIM19_IRQ_PRIORITY        7

/*
 * ICU driver system settings.
 */
#define STM32_ICU_USE_TIM1                  FALSE
#define STM32_ICU_USE_TIM2                  FALSE
#define STM32_ICU_USE_TIM3                  TRUE
#define STM32_ICU_USE_TIM4                  FALSE
#define STM32_ICU_USE_TIM8                  FALSE
#define STM32_ICU_USE_TIM15                 TRUE
#define STM32_ICU_USE_TIM16                 TRUE
#define STM32_ICU_USE_TIM17                 TRUE
#define STM32_ICU_USE_TIM19                 TRUE
#define STM32_ICU_TIM1_IRQ_PRIORITY         7
#define STM32_ICU_TIM2_IRQ_PRIORITY         7
#define STM32_ICU_TIM3_IRQ_PRIORITY         7
#define STM32_ICU_TIM4_IRQ_PRIORITY         7
#define STM32_ICU_TIM8_IRQ_PRIORITY         7
#define STM32_ICU_TIM15_IRQ_PRIORITY        7
#define STM32_ICU_TIM16_IRQ_PRIORITY        7
#define STM32_ICU_TIM17_IRQ_PRIORITY        7
#define STM32_ICU_TIM19_IRQ_PRIORITY        7

/*
 * PWM driver system settings.
 */
#define STM32_PWM_USE_ADVANCED              FALSE
#define STM32_PWM_USE_TIM1                  FALSE
#define STM32_PWM_USE_TIM2                  FALSE
#define STM32_PWM_USE_TIM3                  FALSE
#define STM32_PWM_USE_TIM4                  FALSE
#define STM32_PWM_USE_TIM5                  TRUE
#define STM32_PWM_USE_TIM8                  FALSE
#define STM32_PWM_TIM1_IRQ_PRIORITY         7
#define STM32_PWM_TIM2_IRQ_PRIORITY         7
#define STM32_PWM_TIM3_IRQ_PRIORITY         7
#define STM32_PWM_TIM4_IRQ_PRIORITY         7
#define STM32_PWM_TIM5_IRQ_PRIORITY         7
#define STM32_PWM_TIM8_IRQ_PRIORITY         7

/*
 * SERIAL driver system settings.
 */

#define STM32_SERIAL_USE_USART1             FALSE
#define STM32_SERIAL_USE_USART2             TRUE
#define STM32_SERIAL_USE_USART3             TRUE
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
#define STM32_SPI_USE_SPI3                  TRUE
#define STM32_SPI_SPI1_DMA_PRIORITY         1
#define STM32_SPI_SPI2_DMA_PRIORITY         1
#define STM32_SPI_SPI3_DMA_PRIORITY         1
#define STM32_SPI_SPI1_IRQ_PRIORITY         10
#define STM32_SPI_SPI2_IRQ_PRIORITY         10
#define STM32_SPI_SPI3_IRQ_PRIORITY         10
#define STM32_SPI_DMA_ERROR_HOOK(spip)      chSysHalt()

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
#define STM32_USB_USE_USB1                  FALSE
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

/*
 * COMP driver system settings
 */
#define STM32_COMP_USE_COMP1                TRUE

/* GENIST Configuration */
#define GENIST_USE_HALL
#define GENIST_USE_LSM
#define GENIST_USE_HAZARD
#define GENIST_USE_CONSOLE
#define GENIST_USE_VARUC
#define GENIST_USE_PRESSURE

/* Hall Sensor configuration */
#define HALL_SENSOR         ICUD17
#define HALL_PORT           GPIOB
#define HALL_PIN            GPIOB_HALL_A
#define HALL_TIM_CHANNEL    ICU_CHANNEL_1

/* PWM Sensor configuration */
#define PWM_SPEED_SENSOR    ICUD3
#define PWM_SPEED_CHANNEL   ICU_CHANNEL_1

/* Variable reluctance sensor configuration */
#define VARUC_SENSOR        ICUD15
#define VARUC_PORT          GPIOB
#define VARUC_PIN           GPIOB_VAR_RUC2
#define VARUC_TIM_CHANNEL   ICU_CHANNEL_1

/* Pressure Sensor configuration */
#define LPRESSURE_SPI       SPID3
#define LPRESSURE_PORT      GPIOA
#define LPRESSURE_PIN       GPIOA_SPI1_NSS

#define SPRESSURE_SPI       SPID3
#define SPRESSURE_PORT      GPIOC
#define SPRESSURE_PIN       GPIOC_SPI3_NSS

/* LSM configuration */
#define LSM_I2C             I2CD1

/* Hazard configuration */
#define HAZ_SENSOR          ICUD16
#define HAZ_PORT            GPIOB
#define HAZ_PIN2            GPIOB_HAZ_2
#define HAZ_PIN1            GPIOB_HAZ_1
#define HAZ_TIM_CHANNEL     ICU_CHANNEL_1

/* Proximity configuration */
#define PROX_1_PAD          GPIOD
#define PROX_1_PIN          GPIOD_PROX_1
#define PROX_2_PAD          GPIOD
#define PROX_2_PIN          GPIOD_PROX_2

/* LED Configuration */
#define LED_STBY_PAD        GPIOC            
#define LED_STBY_PIN        GPIOC_STBY
#define LED1_PAD            GPIOA
#define LED1_PIN            GPIOA_LED1_X
#define LED2_PAD            GPIOA
#define LED2_PIN            GPIOA_LED2_X
// layout inversion
//#define LED3_PAD            GPIOC
//#define LED3_PIN            GPIOC_LED3_X
#define LED3_PAD            GPIOB
#define LED3_PIN            GPIOB_IN4

#define LEDX_PWM            PWMD5
#define LED1X_CHANNEL       1
#define LED2X_CHANNEL       2
#define LED3X_CHANNEL       3

/* Variable Reluctance configuration */
#define VAR_RUC             ICUD15
#define VAR_PAD             GPIOB
#define VAR_PIN             GPIOB_VR_DIR
#define VAR_TIM_CHANNEL     ICU_CHANNEL_1

/* Console configuration */
#define CONSOLE             SD3

/* low to Enable sensors */
#define OUT_ST_DIS_1_PORT   GPIOE
#define OUT_ST_DIS_1_PIN    GPIOE_ST_DIS1
#define OUT_ST_DIS_2_PORT   GPIOE
#define OUT_ST_DIS_2_PIN    GPIOE_ST_DIS2
#define OUT_ST_DIS_3_PORT   GPIOE
#define OUT_ST_DIS_3_PIN    GPIOE_ST_DIS3

#define OUT_ST_DIS_4_PORT   GPIOD
#define OUT_ST_DIS_4_PIN    GPIOD_ST_DIS4

/* front free lock X3_2*/
#define OUT_FLOCK_PORT          GPIOD
#define OUT_FLOCK_PIN           GPIOD_EXT_8

/* second steering lock X3_2*/
#define OUT_LOCK_PORT           GPIOD
#define OUT_LOCK_PIN            GPIOD_EXT_7

/* front lift X4_1*/
#define OUT_AXLE1_PORT            GPIOD
#define OUT_AXLE1_PIN            GPIOD_EXT_6

/* backup light X4_2*/
#define OUT_BACKUP_PORT            GPIOD
#define OUT_BACKUP_PIN            GPIOD_EXT_5

/* second lift X1_1*/
#define OUT_AXLE2_PORT            GPIOC
#define OUT_AXLE2_PIN            GPIOC_EXT_4

/* third lift X1_2*/
#define OUT_AXLE3_PORT            GPIOC
#define OUT_AXLE3_PIN            GPIOC_EXT_3

/* backup light forth lift (opt power) X2_1*/
#define OUT_BACKUP2_PORT        GPIOC
#define OUT_BACKUP2_PIN                GPIOC_EXT_2

/* active cooling X2_2*/
#define OUT_ACTIVECOOL_PORT        GPIOC
#define OUT_ACTIVECOOL_PIN        GPIOC_EXT_1

/* forth lift X9_1*/
#define OUT_AXLE4_PORT            GPIOE
#define OUT_AXLE4_PIN            GPIOE_EXT_12

/* startup bulb X9_2*/
#define OUT_BULBCHECK_PORT        GPIOC
#define OUT_BULBCHECK_PIN        GPIOC_EXT_11

/* LED power X16_1*/
#define OUT_LEDPOWER_PORT        GPIOC
#define OUT_LEDPOWER_PIN        GPIOC_EXT_10

/* wheel sensor power  X16_2*/
#define OUT_SENSPOWER_PORT        GPIOA
#define OUT_SENSPOWER_PIN        GPIOA_EXT_9

