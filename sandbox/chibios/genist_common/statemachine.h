/**
 * statetransition.h
 * Copyright Genist Inc.
 *
 *  Created on: 2013-04-28
 *      Author: jeromeg
 */

#ifndef STATEMACHINE_H_
#define STATEMACHINE_H_

#define AXLE_LOWER      0
#define AXLE_RAISE      1
#define REAR_UNLOCK     0
#define REAR_LOCK       1
#define FRONT_UNLOCK    3
#define FRONT_LOCK      4

#define chVTSetIsr(vtp, time, vtfunc, par) {                                \
    chSysLockFromIsr();                                                     \
    chVTSetI(vtp, time, vtfunc, par);                                       \
    chSysUnlockFromIsr();                                                   \
}


#define chVTResetIsr(vtp) {                                                 \
  chSysLockFromIsr();                                                       \
  if (chVTIsArmedI(vtp))                                                    \
    chVTResetI(vtp);                                                        \
  chSysUnlockFromIsr();                                                     \
}


/**
 * Speed states
 */
typedef enum {
    STATE_SP_S,//!< Stopped
    STATE_SP_L,//!< Low speed
    STATE_SP_R,//!< Reversed
    STATE_SP_H //!< High speed
} SpeedState;

/**
 * Pressure states
 */
typedef enum {
    STATE_PR_F_H,     //!< High pressure
    STATE_PR_L1H,     //!< middle pressure
    STATE_PR_L1L,      //!< Low pressure
    STATE_PR_INIT     //!< Initial
} PressureState;

/**
 * User states
 */
typedef enum {
    STATE_USER_O,       //!< Normal
    STATE_USER_H        //!< Hazard lights on
} UserState;

/**
 * Events
 */
typedef enum {
    EVENT_SP_S,         //!< Speed changed to stopped
    EVENT_SP_L,         //!< Speed changed to low
    EVENT_SP_R,         //!< Speed changed to reversed
    EVENT_SP_H,         //!< Speed changed to high
    EVENT_PR_F_H,       //!< Pressure changed low, middle to high
    EVENT_PR_L1H,       //!< Pressure changed high, low to middle
    EVENT_PR_L1L,       //!< Pressure changed middle, high to low
    EVENT_USER_O,       //!< User changed to normal
    EVENT_USER_H,       //!< User changed to hazard
    EVENT_ERROR_WHEEL_SPEED,    //!< Invalid stat transition signals damaged sensor
    EVENT_ERROR_PWM_AIRGAP,     //!< Sensor indicates it is damaged
    EVENT_ERROR_PWM_EL,         //!< Sensor indicates it is needs attention
    ERROR_HIGH_SUP_PRES,    //!< Supply pressure too high
    ERROR_LOW_SUP_PRES,    //!< Supply pressure too low
} Event;

/**
 *
 */
typedef enum {
    ERROR_NONE,         //!< No error
    ERROR_WHEEL_SPEED,  //!< Lost wheel speed sensor
    ERROR_PRESSURE,     //!< Lost pressure sensor
    ERROR_PWM_AIRGAP,   //!< PWM airgap warning
    ERROR_PWM_EL        //!< PWM EL warning
} StateError;

void StateInit(void);
void SendEvent (Event ev);
void SendEventI (Event ev);
void StateMachineMailboxInit(void);
void ActionAxle(int axle, int dir);
void ActionLock(int lock);
void ActionError(StateError err);
void ActionBeep(int state);
void enableAstatus(int bank);
void initHazard(void);
void TimerFlashSample(void *arg);
void TimerFlashOff(void *arg);
void TimerFlashCancel(void *arg);

#endif /* STATEMACHINE_H_ */
