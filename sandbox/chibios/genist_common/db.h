/**
 * db.h
 *
 *  Created on: 2013-04-30
 *      Author: jeromeg
 */

#ifndef DB_H_
#define DB_H_
#include "ch.h"
#include "cJSON.h"

/* integer items */
#define INT_WHEEL_PULSES_PER_ROTATION       0
#define INT_WHEEL_DIR                       1
#define INT_NUMBER_AXLES                    2
#define INT_AXLE_POS_BIT_MASK               3
#define INT_PRESSURE_ALL_UPPER_THRES        4
#define INT_PRESSURE_ALL_LOWER_THRES        5
#define INT_PRESSURE_L1_UPPER_THRES         6
#define INT_PRESSURE_L1_LOWER_THRES         7
#define INT_HIGH_SPEED_UPPER_THRESHOLD      8
#define INT_HIGH_SPEED_LOWER_THRESHOLD      9
#define INT_LOW_SPEED_DISTANCE_THRESHOLD    10
#define INT_SOLENOID_BIT_MASK               11
#define INT_REVERSE_UPPER_THRESHOLD         12
#define INT_STOP_TIME                       13
#define INT_SPEED_SENSOR_INDEX              14
#define INT_SPEED_SENSOR_RTD                15
#define INT_WHEEL_DIAMETER_MILLIMETERS      16
#define INT_LOAD_PRESSURE                   17
#define INT_PRESSURE_SUPPLY_MIN             18
#define INT_PRESSURE_SUPPLY_MAX             19 
#define INT_WARNINGS                        20
#define INT_LOWER_FULL_WEIGHT               21
#define INT_UPPER_FULL_WEIGHT               22 
#define INT_FLASHES_HAZ_DETECT              23
#define INT_FLASHES_CANCEL_HAZ_DECTECT      24 
#define INT_TIMEOUT_HAZ_OFF                 25
#define INT_TIMEOUT_HAZ_CANCEL              26 
#define INT_FACTORY_VERSION                 27
#define INT_CURRENT_VERSION                 28 
#define INT_LOAD_INDEX                      29


/* integer items */
#define INT_GetConfig                   0
#define INT_GetFaultLog                 1
#define INT_GetFaultBegin               2
#define INT_GetFaultEnd                 3
#define INT_GetWarnLog                  4
#define INT_GetWarnBegin                5
#define INT_GetWarnEnd                  6


#define PI ((float) 3.14159)
#define SEC_PER_HOUR 3600
#define FLASH_DB_BASE      0x0803c000
uint8_t dbSetInt(int16_t index, int32_t val);
int32_t dbGetInt(int16_t index);

int32_t dbGetAxlePosBitMask(void);
int32_t dbGetSolenoidBitMask(void);

float dbGetWheelScaleFactor(void);
float dbGetLoadPressPolyPT(void);

int32_t dbGetFullPressureUpperThreshold(void);
int32_t dbGetFullPressureLowerThreshold(void);
int32_t dbGetL1PressureUpperThreshold(void);
int32_t dbGetL1PressureLowerThreshold(void);

int32_t dbGetHighSpeedUpperThreshold(void);
int32_t dbGetHighSpeedLowerThreshold(void);
int32_t dbGetLowSpeedDistanceThreshold(void);
int32_t dbGetReversedUpperThreshold(void);
int32_t dbGetStopTime(void);
int32_t dbGetSpeedSensorIndex(void);
int32_t dbGetLoadPressureLoc(void);
int32_t dbGetMinSupplyPressure(void);
int32_t dbGetMaxSupplyPressure(void);

int32_t dbGetWheelDir(void);

int32_t dbGetWarnings(void);
int32_t dbGetLowerFullWeight(void);
int32_t dbGetUpperFullWeight(void);
int32_t dbGetFlashesHazDetect(void);
int32_t dbGetFlashesCancelHazDetect(void);
int32_t dbGetTimeoutHazOff(void);
int32_t dbGetTimeoutHazCancel(void);

int32_t dbGetFactoryVersion(void);
int32_t dbGetCurrentVersion(void);


uint8_t dbSave(void);
uint8_t dbRestore(void);
int16_t dbLookup(char *string);
char *dbGetString(int16_t index);
cJSON *dbGetJSON(char *jsonString);
uint8_t dbSetJSON(char *jsonString);
#endif /* DB_H_ */
