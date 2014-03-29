/**
 * threads.h
 *
 *  Created on: 2013-04-30
 *      Author: jeromeg
 */

#ifndef THREADS_H_
#define THREADS_H_

#define DECLARE_THREAD(threadname, prio, stacksize) \
static msg_t threadname(void *arg); \
static WORKING_AREA(wa##threadname, stacksize); \
\
void start##threadname(void) \
{ \
    chThdCreateStatic(wa##threadname, sizeof(wa##threadname), prio, \
            threadname, NULL ); \
}

#endif /* THREADS_H_ */
