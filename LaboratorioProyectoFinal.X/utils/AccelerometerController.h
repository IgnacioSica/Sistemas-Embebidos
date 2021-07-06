
#ifndef ACCELEROMETER_CONTROLLER_H    
#define ACCELEROMETER_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "State.h"
#include "LedsController.h"

#ifdef __cplusplus

#endif

uint8_t warning_level;
uint8_t danger_level;

uint8_t getWarningLevel();
void setWarningLevel(uint8_t level);

uint8_t getDangerLevel();
void setDangerLevel(uint8_t level);

void getAccelerometerValues(void *p_param);
void changeLedColor( void *p_param );
    
#ifdef __cplusplus

#endif

#endif 
