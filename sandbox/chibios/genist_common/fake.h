/**
 * fake.h
 *
 *  Created on: 2013-04-30
 *      Author: jeromeg
 */

#ifndef FAKE_H_
#define FAKE_H_

#define PWM_STATE_NORMAL            0
#define PWM_STATE_AIRGAP_WARNING    1
#define PWM_STATE_EL_WARNING        2

void startFakeSpeed(void);
void setFakeSpeed(int speed);
void startFakePwm(void);
void setFakePwm(int speed, int mode);

#endif /* FAKE_H_ */
