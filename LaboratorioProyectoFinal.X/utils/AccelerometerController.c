#include "AccelerometerController.h"

uint8_t warning_level = 4;
uint8_t danger_level = 4;

uint8_t getWarningLevel(){
    return warning_level;
}
void setWarningLevel(uint8_t level){
    warning_level = level;
}

uint8_t getDangerLevel(){
    return danger_level;
}

void setDangerLevel(uint8_t level){
    danger_level = level;
}

