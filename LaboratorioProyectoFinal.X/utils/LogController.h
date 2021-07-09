
#ifndef LOG_CONTROLLER_H    
#define LOG_CONTROLLER_H 

#include <time.h>
#include <stdint.h>
#include <string.h>

#include "State.h"
#include "FreeRTOS.h"
#include "task.h"
#include "../framework/GPS/GPS.h"
#include "../framework/SIM808/SIM808.h"
#include "MenuController.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    //unsigned long date;
    system_state state;
    uint8_t lat_lon;
    uint8_t date;
}log;



void logTimer(void *p_param);
void logger(log* logs);
log* getLogs();
void prueba(void *p_param);

   
#ifdef __cplusplus
}
#endif

#endif 
