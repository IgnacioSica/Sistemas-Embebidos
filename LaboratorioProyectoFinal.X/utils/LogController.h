
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
#include "AccelerometerController.h"


#ifdef __cplusplus
extern "C" {
#endif

/*typedef struct{
    uint8_t log_number;
    system_state state;
    uint8_t lat_lon;
    uint8_t date;
}log;*/

//log* getLogs();
//extern log logs[250];
extern uint8_t *logs[250];
void logTimer(void *p_param);
void logger(void *p_param);
void pruebaGPS(void *p_param);

   
#ifdef __cplusplus
}
#endif

#endif 
