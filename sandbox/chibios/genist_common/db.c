/**
 * db.c
 * Database access.
 * The items here will be stored in Flash as part of the configuration. For now
 * the values are hardcoded.
 *
 *  Created on: 2013-04-30
 *      Author: jeromeg
 */
#include <string.h>
#include "db.h"
#include "stm32f37x_flash.h"
#include "cJSON.h"
#define MILLIMETERS_PER_METER   1000    /* mm to m conversion */
#define NUM_FLOATS          42
#define NUM_INTS            42
#define NUM_LDPRPOLYPT      4
typedef struct {
   //float        floatArray[NUM_FLOATS];
   int32_t      intArray[NUM_INTS];
   int32_t      Checksum;
} Database;

/* should have pressure upper lower threshold for each axle
 * for simplicity assume 2 axles
 * this can be increased in a software upgrade if necessary */
/* stationary is between the
 * low speed and reverse thresholds */
/* max 32 elements in configure array
 * append additions at the end*/

/* production should store all zeros, and force the equipment
 * manufacture to configure all values */
Database db = {
    .intArray = {
        100,        /* ST Pulses per rotation              */
        -1,          /* wheel direction  1 or -1             */
        2,          /* T number of lift axles             */
        3,          /* L Axle Position Bit Mask    */
        40000,      /* L Pressure all axle upper threshold    */ // 45psi
        34000,       /* L Pressure all axle lower threshold    */ //35psi
        41800,       /* L Pressure all -1 upper threshold      */
        34420,       /* L Pressure all -1 lower threshold      */
        57000,      /* S High speed upper threshold       */
        53000,      /* S High speed lower threshold       */
        100,       /* S Low speed distance threshold        */
        15,         /* L Solenoid bit mask                */
        -500,      /* S Reverse upper threshodld         */
        60,         /* S stop time          */
        1,          /* ST Speed sensor type index          */
        0,          /* ST speed sensor rtd sensor          */
        1150,       /* ST Wheel diameter in mm             */
        0,          /* LT swap Load Pressure sensor           */
        10000,      /* LT Supply Pressure Minimum          */
        60000,      /* LT Supply Pressure Maximum          */
        1,          /* R enable warning log               */
        55000,      /* L Lower Full Weight                */
        59000,      /* L Upper Full Weight                */
        3,          /* H ticks haz set on dectect         */
        15,          /* H ticks haz cancel on dectect      */
        3,          /* H timeout haz set off              */
        15,          /* H timeout haz cancel               */
        1,          /* R factory installed version        */
        1,          /* R current installed version        */
        2000,          /* LG L1                               */
        20,          /* LG P1                               */
        5000,          /* LG L2                               */
        30,          /* LG P2                               */
        10000,     /* LG L3                               */
        66,       /* LG P3                               */
        60000,     /* LG L4                               */
        100        /* LG P4                               */
    },
    .Checksum = 0
};




char *dbString[] = {
    "PulsesPerRotation",
    "WheelDir",
    "NumberAxles",
    "AxlePosBitMask",
    "PressureAllUpperThreshold",
    "PressureAllLowerThreshold",
    "PressureLessOneUpperThreshold",
    "PressureLessOneLowerThreshold",
    "HighSpeedUpperThreshold",
    "HighSpeedLowerThreshold",
    "LowSpeedThreshold",
    "SolenoidBitMask",
    "ReverseThreshold",
    "StopTime",
    "SpeedSensorIndex",
    "SpeedSensorRTD",
    "WheelDiameter",
    "LoadPressureSensor",
    "MinSupplyPressure",
    "MaxSupplyPressure",
    "Warnings",
    "LowerFullWeight",
    "UpperFullWeight",
    "FlashesHazDetect",
    "FlashesCancelHazDetect",
    "TimeoutHazOff",
    "TimeoutHazCancel",
    "FactoryVersion",
    "CurrentVersion",
    "L1",
    "P1",
    "L2",
    "P2",
    "L3",
    "P3",
    "L4",
    "P4",
    NULL
};

char *dbGetcmds[] = {
    "Config",
    "FaultLog",
    "FaultLogBegin",
    "FaultLogEnd",
    "WarnLog",
    "WarnLogBegin",
    "WarnLogEnd",
    NULL
};

/**
 * Find the index of a database item
 * @param string
 * @return index
 */
int16_t dbLookup(char *string)
{
    int16_t index = 0;
    while (dbString[index])
    {
        if (!strcmp(string, dbString[index]))
        {
            return index;
        }
        index++;
    }
    return -1;
}

/**
 * Get string description of database item
 * @param index
 * @return
 */
char *dbGetString(int16_t index)
{
    if (index >= 0 && index < NUM_INTS)
    {
        return dbString[index];
    }
    else
    {
        return NULL;
    }
}

/**
 * Set an integer database item
 * @param index
 * @param val
 * @return
 */
uint8_t dbSetInt(int16_t index, int32_t val)
{
    uint8_t ret = 0;
    if (index >= 0 && index < NUM_INTS)
    {
        db.intArray[index] = val;
    }
    else
    {
        ret = 1;
    }
    return ret;
}

/**
 * Get integer database item
 * @param index
 * @return
 */
int32_t dbGetInt(int16_t index)
{
    if (index >= 0 && index < NUM_INTS)
    {
        return db.intArray[index];
    }
    else
    {
        return 0;
    }
}

/**
 * wheel direction
 * @return
 */
int32_t dbGetWheelDir(void)
{
    return db.intArray[INT_WHEEL_DIR];
}

/**
 * Get SolenoidBitMask pattern
 * @return
 */
int32_t dbGetAxlePosBitMask(void)
{
    return db.intArray[INT_AXLE_POS_BIT_MASK];
}


/**
 * Calculates the number of meters traveled per pulse.
 * @return
 */
float dbGetWheelScaleFactor(void)
{
    return PI * (float)db.intArray[INT_WHEEL_DIAMETER_MILLIMETERS] /
            MILLIMETERS_PER_METER /
            db.intArray[INT_WHEEL_PULSES_PER_ROTATION];
}

/**
 * Get the threshold between high and low pressure
 * @return
 */
int32_t dbGetFullPressureUpperThreshold(void)
{
    return db.intArray[INT_PRESSURE_ALL_UPPER_THRES];
}

/**
 * Get the threshold between high and low pressure
 * @return
 */
int32_t dbGetFullPressureLowerThreshold(void)
{
    return db.intArray[INT_PRESSURE_ALL_LOWER_THRES];
}

/**
 * Get the threshold between high and low pressure
 * @return
 */
int32_t dbGetL1PressureUpperThreshold(void)
{
    return db.intArray[INT_PRESSURE_L1_UPPER_THRES];
}

/**
 * Get the threshold between high and low pressure
 * @return
 */
int32_t dbGetL1PressureLowerThreshold(void)
{
    return db.intArray[INT_PRESSURE_L1_LOWER_THRES];
}

/**
 * Get the high speed upper threshold. High Speed condition is declared when
 * the speed is above this threshold.
 * @return
 */
int32_t dbGetHighSpeedUpperThreshold(void)
{
    return db.intArray[INT_HIGH_SPEED_UPPER_THRESHOLD];
}

/**
 * Get the high speed lower threshold. High Speed condition is exited and Low
 * Speed condition is entered when the speed is below this threshold.
 * @return
 */
int32_t dbGetHighSpeedLowerThreshold(void)
{
    return db.intArray[INT_HIGH_SPEED_LOWER_THRESHOLD];
}

/**
 * Get low speed upper threshold. Low Speed condition is entered when the speed
 * above this threshold.
 * @return
 */
int32_t dbGetLowSpeedDistanceThreshold(void)
{
    return db.intArray[INT_LOW_SPEED_DISTANCE_THRESHOLD];
}

/**
 * Get SolenoidBitMask pattern
 * @return
 */
int32_t dbGetSolenoidBitMask(void)
{
    return db.intArray[INT_SOLENOID_BIT_MASK ];
}

/**
 * Get reversed upper threshold. Reversed condition is entered when the speed
 * (in reverse) is above this threshold.
 * @return
 */
int32_t dbGetReversedUpperThreshold(void)
{
    return db.intArray[INT_REVERSE_UPPER_THRESHOLD];
}

/**
 * Get reversed lower threshold. Reversed condition is exited and Stopped
 * condition is entered when the speed (in reverse) is below this threshold.
 * @return
 */
int32_t dbGetStopTime(void)
{
    return db.intArray[INT_STOP_TIME];
}

/**
 * Return which sensor has been selected for speed measurement.
 * @return
 */
int32_t dbGetSpeedSensorIndex(void)
{
    return db.intArray[INT_SPEED_SENSOR_INDEX];
}

/**
 * Return the position of the Load pressure sensor.
 * @return
 */
int32_t dbGetLoadPressureLoc(void)
{
    return db.intArray[INT_LOAD_PRESSURE];
}

/**
 * Return the minimum supply pressure.
 * @return
 */
int32_t dbGetMinSupplyPressure(void)
{
    return db.intArray[INT_PRESSURE_SUPPLY_MIN];
}

/**
 * Return the maximum supply pressure.
 * @return
 */
int32_t dbGetMaxSupplyPressure(void)
{
    return db.intArray[INT_PRESSURE_SUPPLY_MAX];
}

/**
 * Return whether warnings are set
 * @return
 */
int32_t dbGetWarnings(void)
{
    return db.intArray[INT_WARNINGS];
}

/**
 * Return lowest weight considered a full load
 * @return
 */
int32_t dbGetLowerFullWeight(void)
{
    return db.intArray[INT_LOWER_FULL_WEIGHT];
}

/**
 * Return largest weight considered a full load
 * @return
 */
int32_t dbGetUpperFullWeight(void)
{
    return db.intArray[INT_UPPER_FULL_WEIGHT];
}

/**
 * Return the number of four way flashes to detect
 * @return
 */
int32_t dbGetFlashesHazDetect(void)
{
    return db.intArray[INT_FLASHES_HAZ_DETECT];
}

/**
 * Return the number of four way flashes to cancel
 * @return
 */
int32_t dbGetFlashesCancelHazDetect(void)
{
    return db.intArray[INT_FLASHES_CANCEL_HAZ_DECTECT];
}

/**
 * Return the timeout to turn off hazard mode
 * @return
 */
int32_t dbGetTimeoutHazOff(void)
{
    return db.intArray[INT_TIMEOUT_HAZ_OFF];
}

/**
 * Return the timeout to cancel hazard mode
 * @return
 */
int32_t dbGetTimeoutHazCancel(void)
{
    return db.intArray[INT_TIMEOUT_HAZ_CANCEL];
}

/**
 * Return the factory version
 * @return
 */
int32_t dbGetFactoryVersion(void)
{
    return db.intArray[INT_FACTORY_VERSION];
}

/**
 * Return the current version
 * @return
 */
int32_t dbGetCurrentVersion(void)
{
    return db.intArray[INT_CURRENT_VERSION];
}


/**
 * Save the database to Flash
 * @return
 */
uint8_t dbSave(void)
{
    uint32_t i = 0;
    uint32_t *addr = (uint32_t *)FLASH_DB_BASE;
    uint32_t *data = (uint32_t *) &db;
    FLASH_Unlock();
    if (FLASH_ErasePage((uint32_t)addr) != FLASH_COMPLETE)
    {
        /* Error occurred while erasing Flash */
        FLASH_Lock();
        return (1);
    }

    /* Calculate checksum */
    db.Checksum = 0;
    for (i = 0; i < sizeof(Database) / sizeof(uint32_t) - 1; i++)
    {
        db.Checksum += data[i];
    }
    /* Write to Flash */
    for (i = 0; i < sizeof(Database) / sizeof(uint32_t); i++)
    {
        /* the operation will be done by word */
        if (FLASH_ProgramWord((uint32_t) addr, data[i]) == FLASH_COMPLETE)
        {
            /* Check the written value */
            if (*addr != data[i])
            {
                /* Flash content doesn't match SRAM content */
                FLASH_Lock();
                return (2);
            }
            /* Increment FLASH destination address */
            addr++;
        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            FLASH_Lock();
            return (3);
        }
    }
    FLASH_Lock();
    return (0);
}

/**
 * Restore the database from Flash
 * @return
 */
uint8_t dbRestore(void)
{
    uint32_t i = 0;
    uint32_t checksum;
    uint32_t *addr = (uint32_t *)FLASH_DB_BASE;

    /* Check checksum */
    checksum = 0;
    for (i = 0; i < sizeof(Database) / sizeof(uint32_t) - 1; i++)
    {
        checksum += addr[i];
    }
    if (checksum != addr[i])
    {
        return (1);
    }

    /* Retrieve database */
    memcpy (&db, addr, sizeof(Database) );

    return (0);
}

/**
 * Modify database using JSON string
 * @return
 */
uint8_t dbSetJSON(char *jsonString)
{
    uint8_t ret = 0;
    uint32_t i;
    cJSON *cmd;
    cJSON *dbItem;
    /* Retrieve database */
    cmd = cJSON_Parse(jsonString);
    if (cmd)
    {
        /* Verify that all fields are valid */
        dbItem = cmd->child;
        while (dbItem)
        {
            i = 0;
            while (dbString[i])
            {
                if (!strcmp(dbItem->string, dbString[i]))
                {
                    break;
                }
                i++;
            }
            if (!dbString[i])
            {
                /* Item did not match any known database item */
                ret = 1;
                break;
            }
            dbItem = dbItem->next;
        }
        if (!ret)
        {
            /* Iterate through known database items */
            i = 0;
            while (dbString[i])
            {
                /* Find database item, if present, then set value */
                dbItem = cJSON_GetObjectItem(cmd, dbString[i]);
                if (dbItem)
                {
                    db.intArray[i] = dbItem->valueint;
                }
                i++;
            }
        }
        /* Free database object */
        cJSON_Delete(cmd);
    }
    else
    {
        ret = 1;
    }
    return ret;
}

/**
 * Get database as JSON string. Caller must free() the returned string
 * after use.
 * @return JSON string
 */
cJSON *dbGetJSON(char *jsonString)
{
    uint32_t i = 0;
    cJSON *dbObject;
    cJSON *cmd;
    cJSON *dbItem;
    char *subcmd=NULL;
    dbObject = cJSON_CreateObject();
    /* get action */
    cmd = cJSON_Parse(jsonString);
    if (cmd)
    {
        dbItem = cmd->child;
        subcmd = cmd->valuestring;
        if (!strcmp(subcmd, dbGetcmds[INT_GetConfig]))
        {
        /* Iterate through known database items */
            while (dbString[i] != NULL)
            {
                /* Add database item to JSON object */
                cJSON_AddNumberToObject(dbObject,dbString[i], db.intArray[i]);
                i++;
            }
            //ret = cJSON_Print(dbObject);
            return dbObject;
        }
        else if (!strcmp(subcmd, dbGetcmds[INT_GetFaultLog]))
        {
            return dbObject;
        }
        else if (!strcmp(subcmd, dbGetcmds[INT_GetFaultBegin]))
        {
            return dbObject;
        }
        else if (!strcmp(subcmd, dbGetcmds[INT_GetFaultEnd]))
        {
            return dbObject;
        }
        else if (!strcmp(subcmd, dbGetcmds[INT_GetWarnLog]))
        {
            return dbObject;
        }
        else if (!strcmp(subcmd, dbGetcmds[INT_GetWarnBegin]))
        {
            return dbObject;
        }
        else if (!strcmp(subcmd, dbGetcmds[INT_GetWarnEnd]))
        {
            return dbObject;
        }
        else
        {
            /* unknown Get */
            cJSON_Delete(dbObject);
            return NULL;
        }
    }
    else
    {
        return dbObject;
    }
}

#if 0
uint32_t _sbrk ( int incr )
{
  return 0;
}
#endif
