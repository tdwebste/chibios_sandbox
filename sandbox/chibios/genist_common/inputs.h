/**
 * inputs.h
 *
 *  Created on: 2013-05-01
 *      Author: jeromeg
 */

#ifndef INPUTS_H_
#define INPUTS_H_

float ReadPressureTemp(SPIDriver *spip);
int32_t ReadPressure(SPIDriver *spip);
int32_t ReadPressureStat(SPIDriver *spip);
int32_t ReadSpeed(void);
char *GetSpeedSensorID(void);
float ReadSense2(void);
float ReadSense2Voltage(void);
float ReadAirTemp(void);
float ReadPWMTemp(void);
float ReadPWMVoltage(void);
float GetVoltage(uint8_t channel);

#endif /* INPUTS_H_ */
