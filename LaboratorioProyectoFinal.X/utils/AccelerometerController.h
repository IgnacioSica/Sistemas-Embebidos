
#ifndef ACCELEROMETER_CONTROLLER_H    
#define ACCELEROMETER_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

uint8_t warning_level;
uint8_t danger_level;

uint8_t getWarningLevel();
void setWarningLevel(uint8_t level);

uint8_t getDangerLevel();
void setDangerLevel(uint8_t level);
    
#ifdef __cplusplus
}
#endif

#endif 
