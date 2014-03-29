/*
    Copyright (C) 2013 Genist
*/

#ifndef _RTD_H_
#define _RTD_H_

/****************************************************************

On the custom board there is a resistance bridge :


                      I (constant current source)
                      |
               +------+-----+
               |            |
               R1           R3
               |            |
       Vrtd--- +            |
               |            |
               R2           Rpt1000
               |            |
               +------+-----+
                      |
                     GND
      

I = I1 + I2

current = current in one branch plus current on other branch.

total equivalent resistance is R where

R =         1
     ------------------------------
         1            1
       --------  + ----------------
       (R1 + R2)    (R3 + Rpt1000)

Vrtd =        R2
        V * -----------
             (R1 + R2)


     = I * R *     R2
                -----------
                 (R1 + R2)

solve for Rpt1000 : courtesy of Wolfram Alpha

******************************************************************/
extern void testRTD(void);
extern msg_t printADCOutputThread(void *arg);

#endif /* _RTD_H_ */
