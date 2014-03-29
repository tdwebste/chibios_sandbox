/**
 * pwm_rheintacho.h
 *
 *  Created on: 2013-05-02
 *      Author: jeromeg
 */

#ifndef PWM_RHEINTACHO_H_
#define PWM_RHEINTACHO_H_
#include <math.h>

/* PWM values when sensor is detecting a signal above 1 Hz              */
//#define PWM_DIR_TOL         10      /* Airgap tol                       */
#define PWM_DIR_LR_US       5      /* Airgap warning range             */
#define PWM_DIR_DRL_US      10     /* Left rotation                    */
#define PWM_DIR_DRR_US      19     /* Right rotation                   */
#define PWM_DIR_DRL_EL_US   37     /* Left rotation with EL warning    */
#define PWM_DIR_DRR_EL_US   73     /* Right rotation with EL warning   */
#define PWM_TOL             1     /* tollerance on width */

/* PWM values when sensor is detecting a signal below 1 Hz or no signal */
#define PWM_DIR_STOP_US     144    /* Stopped state                    */
#define PWM_DIR_STOP_EL_US  145    /* Stopped state with EL             */
#define PWM_STOP_PERIOD     1440000 /* Stopped period                   */

/* Temperature sensor coefficients from data sheet */
#define PWM_A           -412.600f   /* Resistance at -40C               */
#define PWM_B            140.410f   /* Resistance at 0C                 */
#define PWM_C            0.00764f   /* Resistance at 25C                */
#define PWM_D           -6.25e-17f  /* Resistance at 100C               */
#define PWM_E           -1.25e-24f  /* Resistance at 125C               */
#define PWM_R_REF        1000.000f  /* Reference resistor               */
#define PWM_R_BIAS       2600.0f    /* Bias resistor                    */
#define PWM_V_REF        2.76f      /* Reference voltage across bridge  */

#define PWM_I_REF       (PWM_V_REF / PWM_R_BIAS)
/****************************************************************

On the custom board there is a Wheatstone bridge fed by a constant current
source:


                      I (constant current source)
                      |
               +------+-----+
               |            |
               R            R
               |            |
               +---  Vo ----+
               |            |
               R            Rx
               |            |
               +------+-----+
                      |
                     GND


I = I1 + I2

current = current in one branch plus current on other branch.

total equivalent resistance is R where

R =              1
     ------------------------------
           1           1
       --------  +  ----------------
          2R        (R + Rx)

Vo =  I * R       (Rx - R)
      -----  *  ---------------
        4             (Rx - R)
                 R + ----------
                         4


Rx =  4 * Vo * R
      -------------  +  R
      (I * R - Vo)


******************************************************************/
static inline float PWMVoltageToTemperature(float vtemp)
{
    float ret;
    vtemp = -vtemp;
    float rtemp = (4.0f * vtemp * PWM_R_REF) / (PWM_I_REF * PWM_R_REF - vtemp) +
            PWM_R_REF;
    ret = PWM_A + PWM_B * sqrtf(1 + PWM_C * rtemp) + PWM_D * powf(rtemp, 5)
           + PWM_E * powf(rtemp, 7);
    return ret;
}
#endif /* PWM_RHEINTACHO_H_ */
