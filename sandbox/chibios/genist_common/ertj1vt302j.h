/**
 * ertj1vt302j.h
 *
 * This module provides a conversion from a voltage reading to temperature
 * based on the ERTJ1VT302J thermistor specification.
 *
 *  On the custom board there is a resistance divider:
 *
 *
 *                    Vref (3.3V)
 *                    |
 *                    |
 *                  Rref (3K)
 *                    |
 *        Vtemp1 ---- +
 *                    |
 *                  Rtemp
 *                    |
 *                    |
 *                   GND
 *
 * Rtemp = (1 - Vtemp1 / Vref) * Rref
 *
 *  Created on: 2013-05-02
 *      Author: jeromeg
 */

#ifndef ERTJ1VT302J_H_
#define ERTJ1VT302J_H_

/* Temperature sensor points */
#define ERT_R_M40         189900.00f    /* Resistance at -40C           */
#define ERT_R_0            11900.00f    /* Resistance at 0C             */
#define ERT_R_25            3000.00f    /* Resistance at 25C            */
#define ERT_R_100            147.10f    /* Resistance at 100C           */
#define ERT_R_125             70.54f    /* Resistance at 125C           */
#define ERT_R_REF          10000.00f    /* Reference resistor           */
#define ERT_V_REF              3.30f    /* Reference voltage            */

/**
 * Convert voltage to temperature using linear approximation based on nominal
 * thermistor characteristics.
 */
static inline float ERTVoltageToTemperature(float vtemp)
{
    //float rtemp = (vtemp * ERT_R_REF) / (ERT_V_REF - vtemp);
    float rtemp = (vtemp * ERT_R_REF)  / (ERT_V_REF - vtemp);
    float ret = 0.0f;
    if (rtemp >= ERT_R_0)
    {
        ret = - (rtemp - ERT_R_0) * 40.0f / (ERT_R_M40 - ERT_R_0);
    }
    else if (rtemp >= ERT_R_25)
    {
        ret = 25.0f - ( rtemp - ERT_R_25) * 25.0f / (ERT_R_0 - ERT_R_25);
    }
    else if (rtemp >= ERT_R_100)
    {
        ret = 100.0f - (rtemp - ERT_R_100) * 75.0f / (ERT_R_25 - ERT_R_100);
    }
    else
    {
        ret = 125.0f - (rtemp - ERT_R_125) * 25.0f / (ERT_R_100 - ERT_R_125);
    }
    return ret;
}
#endif /* ERTJ1VT302J_H_ */
