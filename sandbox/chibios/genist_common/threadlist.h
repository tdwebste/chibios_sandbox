/**
 * threadlist.h
 *
 *  Created on: 2013-04-30
 *      Author: jeromeg
 */

#ifndef THREADLIST_H_
#define THREADLIST_H_

/* Add other threads here */
void startThreadSpeed(void);
void startThreadBlinker(void);
void startThreadPressureSensor(void);
void startThreadSDADC(void);
void startThreadRTD(void);
void startThreadMonitor(void);
void startThreadStateMachine(void);
#endif /* THREADLIST_H_ */
