#include "AccelerometerController.h"

uint8_t warning_level = 4;
uint8_t danger_level = 4;
bool isTaskRunning = false;

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

void getAccelerometerValues(void *p_param){
    float accel = 0;
    
    while(1){
        if(getCurrentState() != debug){
            ACCEL_Mod(&accel);  

            if(accel >= ((getWarningLevel() + getDangerLevel()) * 0.5)){
                setCurrentState(danger);
                /*if(!isTaskRunning){
                    isTaskRunning = true;
                    xTaskCreate( logger, "logger", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
                }*/
            }
            else if(accel >= (0.5 * getWarningLevel())){
                setCurrentState(warning);
                /*if(!isTaskRunning){
                    xTaskCreate( logger, "logger", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
                    isTaskRunning = true;
                }*/
            }
            else{
                setCurrentState(normal);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
} 

void changeLedColor( void *p_param ){
    bool blinkWarning = true;
    bool blinkDanger = true;
    
    while (1) {
        if(getCurrentState() == normal){
            setLedColor(GREEN, 8);
            blinkWarning = true;
            blinkDanger = true;
        } else if(getCurrentState() == warning){
            if(blinkWarning){
                blinkLed(YELLOW);
                blinkWarning = false;
            }
            blinkDanger = true;
        } else if(getCurrentState() == danger){
            if(blinkDanger){
                blinkLed(RED);
                blinkDanger = false;
                vTaskDelay(pdMS_TO_TICKS(3000));
            }
            blinkWarning = true;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

