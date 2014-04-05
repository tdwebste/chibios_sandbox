////////////////////////////////////////////////////
// File Name : lsm303main.c 
// 
// Objective :
//      
//   10,Oct,2013 : Tim Taisik Ham
//        - To control lsm303dlhc accelerometer and magnetic sensor

#include <string.h>
#include "ch.h"
#include "hal.h"
#include "db.h"
#include "genist_debug.h"
#include "threads.h"
#include "statemachine.h"

////////////////////////////////////////////////////////////////
// Global Variable
//

I2CDriver       gI2CDriver;      
I2CConfig       gI2CConfig;

////////////////////////////////////////////////////////////////
// External Variable
//
extern  void LSM303DLHC_AccInit();
extern  void LSM303DLHC_MagInit();

//DECLARE_THREAD(Threadlsm303dlhc, NORMALPRIOR, 1024)
//static WORKING_AREA(i2c_thread_wa, 1024);
//static msg_t Threadlsm303dlhc(void *arg)
void i2c_thread(void *arg)
{
   (void) arg;

   msg_t     msg;
   uint8_t   data;

   chRegSetThreadName("Accelerometer LSM303DLHC");

   LSM303DLHC_AccInit();
   LSM303DLHC_MagInit();

   while (TRUE) 
   {
        data = LSM303DLHC_AccGetDataStatus(); 
	data = LSM303DLHC_MagGetDataStatus();

        chThdSleepMilliseconds(30);
   }  
   return 0;
}   
   
}
