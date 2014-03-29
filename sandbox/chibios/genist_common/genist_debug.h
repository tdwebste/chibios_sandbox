/**
 * genist_debug.h
 *
 *  Created on: 2013-04-28
 *      Author: jeromeg
 */

#ifndef GENIST_DEBUG_H_
#define GENIST_DEBUG_H_
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#ifdef BOARD_GENIST_STM32F3_DISCOVERY
extern SerialUSBDriver SDU1;
#endif
#ifdef USE_ITM_STREAM
extern BaseSequentialStream ITMStream;
#define dbgprintf(args...) chprintf(&ITMStream, args )
#else
extern uint8_t dbgprint_enable;
#define dbgprintf(args...) if (dbgprint_enable) { chprintf((BaseSequentialStream *)&CONSOLE, args ); }
#endif
#endif /* GENIST_DEBUG_H_ */
