/**
 * itmstream.c
 *
 *  Created on: 2013-04-28
 *      Author: jeromeg
 */

#include <stdarg.h>
#include "ch.h"
#include "hal.h"

volatile int32_t ITM_RxBuffer;

size_t ITMWrite(void *instance, const uint8_t *bp, size_t n)
{
    size_t i;
    (void) instance;
    for (i = 0; i < n; i++)
    {
        ITM_SendChar(bp[i]);
    }
    return n;
}

size_t ITMRead(void *instance, uint8_t *bp, size_t n)
{
    size_t count = 0;
    (void) instance;
    while ((count < n) && ITM_CheckChar())
    {
        bp[count++] = ITM_ReceiveChar();
    }
    return count;
}


msg_t ITMPut(void *instance, uint8_t b)
{
    (void) instance;
    ITM_SendChar(b);
    return 0;
}

msg_t ITMGet(void *instance)
{
    (void) instance;
    return ITM_ReceiveChar();
}

struct BaseSequentialStreamVMT ITMStreamOpts = {
        .write = ITMWrite,
        .read = ITMRead,
        .put = ITMPut,
        .get = ITMGet
};

BaseSequentialStream ITMStream = {
        .vmt = &ITMStreamOpts
};
