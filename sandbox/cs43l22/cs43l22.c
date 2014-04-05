#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "stm32f4xx.h"
#include "i2s_bits.h"

#include "cs43l22.h"
#include "cs43l22_pins.h"
#include "cs43l22_commands.h"

extern SerialUSBDriver SDU1;

#define dbg_print(msg, ...) chprintf((BaseSequentialStream*)&SDU1, msg, ##__VA_ARGS__)

/* I2C driver configuration */
static const I2CConfig _i2s_i2c_config = {
	OPMODE_I2C ,		// Operation mode
	I2S_OI2C_SPEED ,	// Clock frequency
	FAST_DUTY_CYCLE_2	// Duty cycle
};


/* Initialise one of the I2C pads */
#define _i2s_init_i2c_pad(port,pad) \
	palSetPadMode( port , pad , \
			PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_MID2 \
			| PAL_MODE_ALTERNATE(4) )

/* Initialise an I2S pad */
#define _i2s_init_i2s_pad(port,pad) \
	palSetPadMode( port , pad , \
			PAL_STM32_OTYPE_PUSHPULL | PAL_STM32_OSPEED_MID2 \
			| PAL_STM32_ALTERNATE( I2S_OUT_FUNCTION ) )

/*
 * Initialise GPIO ports to handle:
 *	- the CS43L22's I2C channel,
 *	- the CS43L22's I2S channel.
 */
static void _i2s_init_gpio( void )
{
	dbg_print("Initializing GPIO...");
	// Reset pin
	palSetPadMode( I2S_ORESET_PORT , I2S_ORESET_PAD ,
			PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_MID2 );

	// SDL/SDA pins
	_i2s_init_i2c_pad( I2C_OSCL_PORT , I2C_OSCL_PAD );
	_i2s_init_i2c_pad( I2C_OSDA_PORT , I2C_OSDA_PAD );

	// I2S WS/MCK/SCK/SD pins
	_i2s_init_i2s_pad( I2S_OWS_PORT , I2S_OWS_PAD );
	_i2s_init_i2s_pad( I2S_OMCK_PORT , I2S_OMCK_PAD );
	_i2s_init_i2s_pad( I2S_OSCK_PORT , I2S_OSCK_PAD );
	_i2s_init_i2s_pad( I2S_OSD_PORT , I2S_OSD_PAD );
	chprintf((BaseSequentialStream*)&SDU1, "DONE\r\n");
}

/* Reset the CS43L22 */
static void _i2s_reset_output( void )
{
    chprintf((BaseSequentialStream*)&SDU1, "Resetting Output...");
    palClearPad( I2S_ORESET_PORT , I2S_ORESET_PAD );
    chThdSleep( I2S_ORESET_DELAY*100 );
    palSetPad( I2S_ORESET_PORT , I2S_ORESET_PAD );
    chThdSleep( I2S_ORESET_DELAY*100 );
    chprintf((BaseSequentialStream*)&SDU1, "DONE\r\n");
}

static msg_t _cs43l22_set(u8 reg, u8 value)
{
	u8 txBuffer[2];
	txBuffer[0] = reg;
	txBuffer[1] = value;
	msg_t rv = i2cMasterTransmitTimeout(
			&I2S_OI2C_DRIVER, I2S_OI2C_ADDRESS,
			txBuffer,
			2,
			NULL,
			0,
			I2S_OI2C_TIMEOUT);
	if (rv)
	{
		dbg_print("I2C 0x%0.2x <- 0x%0.2x ERROR %d\r\n",
				reg, value, rv);
		dbg_print("status = 0x%x\r\n",
				i2cGetErrors(&I2S_OI2C_DRIVER));
	}
	else
	{
		dbg_print("I2C 0x%x <- 0x%x 0K\r\n", reg, value);
	}
	return rv;
}

/* Get a register from the CS43L22 through I2C */
static u8 _cs43l22_get( u8 reg )
{
	u8 data;
	msg_t rv = i2cMasterTransmitTimeout(
			& I2S_OI2C_DRIVER , I2S_OI2C_ADDRESS ,
			&reg , 1 ,
			&data , 1 ,
			I2S_OI2C_TIMEOUT );
	if ( rv ) {
		dbg_print("I2C 0x%0.2x >- ??? ERROR %d\r\n" ,
				reg , rv );
		dbg_print("    status = 0x%x\r\n" ,
				i2cGetErrors( & I2S_OI2C_DRIVER ) );
	}
	return data;
}

/* Initialise the I2S interface to the CS43L22 */
static void _cs43l22_init_i2s( u32 frequency )
{
	rccEnableSPI3(TRUE);

	
	u32 plln = ( RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN ) >> 6;
	u32 pllr = ( RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR ) >> 28;
	u32 pllm = (u32)( RCC->PLLCFGR & RCC_PLLCFGR_PLLM );
	u32 clock = (u32)( ( ( HSE_VALUE / pllm ) * plln ) / pllr );
	dbg_print( "PLL REG=0x%.x N=%d R=%d M=%d CLK = %d\r\n" ,
			RCC->PLLI2SCFGR , plln , pllr , pllm , clock );

	u16 frq = (u16)(( ( ( clock / 256 ) * 10 ) / frequency ) + 5 ) / 10;
	dbg_print( "frq base = 0x%x\r\n" , frq );
	if ( ( frq & 1 ) == 0 ) {
		frq = frq >> 1;
	} else {
		frq = STM32F4_I2S_PR_ODD | ( frq >> 1 );
	}
	chprintf( (void*)&SD2 , "frq = 0x%x\r\n" , frq );

	*((u32*)(SPI3_BASE + STM32F4_I2S_OFFS_CFG)) =
		  STM32F4_I2S_CFG_MODE_I2S
		| STM32F4_I2S_CFG_STD_I2S
		| STM32F4_I2S_CFG_CFG_MS_TX
		| STM32F4_I2S_CFG_RCPOL_OFF
		| STM32F4_I2S_CFG_DLEN_16
		| STM32F4_I2S_CFG_CLEN_16;
	*((u32*)(SPI3_BASE + STM32F4_I2S_OFFS_PR)) = frq
		| STM32F4_I2S_PR_MCLK_ON;
	*((u32*)(SPI3_BASE + STM32F4_I2S_OFFS_CFG)) |= STM32F4_I2S_CFG_ENABLED;
}

static void _cs43l22_init( void )
{
	i2cStart (&(I2S_OI2C_DRIVER), &_i2s_i2c_config);

	_cs43l22_set(0x02, 0x01);
	_cs43l22_set(0x04, 0xAF);
	_cs43l22_set(0x05, 0x81);
	_cs43l22_set(0x06, 0x04);
	sndOutputVolume( 200 ); 
	
	_cs43l22_set ( 0x1c, 0x33);
	_cs43l22_set ( 0x1e, CS43L22_B3_BEEP_CFG_CONT);
	_cs43l22_set(0x02, 0x9e);

#if 0
	// Make sure the device is powered down
	_cs43l22_set( CS43L22_REG_PWR_CTL1 , CS43L22_PWR1_DOWN );
	// Activate headphone channels
	_cs43l22_set( CS43L22_REG_PWR_CTL2 ,
			CS43L22_PWR2_SPKA_ON | CS43L22_PWR2_SPKB_ON
			| CS43L22_PWR2_HDA_ON | CS43L22_PWR2_HDB_ON );
	// Unmute headphones
	_cs43l22_set( CS43L22_REG_PB_CTL1, CS43L22_PB2_HPB_MUTE_OFF | CS43L22_PB2_HPA_MUTE_OFF);
	// Set serial clock
	_cs43l22_set( CS43L22_REG_CLOCK_CTL , CS43L22_CLK_AUTO_ON
			| CS43L22_CLK_MCDIV_ON );
	// Set input data format
	_cs43l22_set( CS43L22_REG_INT_CTL1 , CS43L22_IC1_SLAVE
			| CS43L22_IC1_SCPOL_OFF | CS43L22_IC1_DSP_OFF
			| CS43L22_IC1_DIF_I2S | CS43L22_IC1_AWL_32 );

	_cs43l22_set( CS43L22_REG_HP_VOLUME_A, 0x0);
	_cs43l22_set( CS43L22_REG_HP_VOLUME_B, 0x0);
	

	_cs43l22_set ( 0x1c, 0x33);
	_cs43l22_set ( 0x1e, CS43L22_B3_BEEP_CFG_CONT);

	_cs43l22_set( 0x0, 0x99 );
	_cs43l22_set( 0x80, 0x47 );
	_cs43l22_set( 0x32, _cs43l22_get(0x32) | (1<<7));
	_cs43l22_set( 0x32, _cs43l22_get(0x32) & ~(1<<7));
	_cs43l22_set( 0x0, 0x0 );

	_cs43l22_init_i2s(11025);
	
	// Fire it up
	_cs43l22_set( CS43L22_REG_PWR_CTL1 , CS43L22_PWR1_UP );

	// Analog soft ramp/zero cross disabled
	_cs43l22_set( CS43L22_REG_AZCSR ,
			CS43L22_AZCSR_SRB_OFF | CS43L22_AZCSR_SRA_OFF
			| CS43L22_AZCSR_ZCB_OFF | CS43L22_AZCSR_ZCA_OFF );
	// Digital soft ramp disabled
	_cs43l22_set( CS43L22_REG_MISC_CTL , CS43L22_MISC_DEEMPHASIS_ON );
	// Limiter: no soft ramp/zero cross, no attack level
	_cs43l22_set( CS43L22_REG_LIM_CTL1 , CS43L22_LIM1_SRD_OFF
			| CS43L22_LIM1_ZCD_OFF );
	// Initial volume and tone controls
	_cs43l22_set( CS43L22_REG_TONE_CTL , 0xf );
	_cs43l22_set( CS43L22_REG_PCM_A , 0x00 );
	_cs43l22_set( CS43L22_REG_PCM_B , 0x00 );
	sndOutputVolume( 200 );
#endif

	dbg_print("ID: %x\r\n", (int)_cs43l22_get(CS43L22_REG_GET_ID));
	dbg_print("Revision: %x\r\n", (int)_cs43l22_get(CS43L22_CHIP_ID));
	dbg_print("Chip Status: 0x%x\r\n", sndGetStatus());
}

void sndOutputVolume( u8 volume )
{
	if ( volume > 0xe6 ) {
		volume -= 0xe7;
	} else {
		volume += 0x19;
	}
	_cs43l22_set( CS43L22_REG_MASTER_VOLUME_A , volume );
	_cs43l22_set( CS43L22_REG_MASTER_VOLUME_B , volume );
}

u8 sndGetStatus( void )
{
	return _cs43l22_get( CS43L22_REG_STATUS );
}

void sndInit( u32 frequency )
{
	_i2s_init_gpio( );
	_i2s_reset_output( );
	_cs43l22_init( );
	dbg_print("Sorta up, status %d\r\n", (int)sndGetStatus());
	//_cs43l22_init_i2s( frequency );
//#ifdef USE_DMA_FOR_SEXY_BEEPS
//	_cs43l22_init_dma( );
//#endif // USE_DMA_FOR_SEXY_BEEPS
//	_snd_next_ok = TRUE;
}
