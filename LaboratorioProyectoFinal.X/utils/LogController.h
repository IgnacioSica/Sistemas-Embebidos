
#ifndef LOG_CONTROLLER_H    
#define LOG_CONTROLLER_H 

#include <time.h>
#include "State.h"
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    unsigned long date;
    system_state state;
    //posicion
}log;

log logs[250];

void logTimer(void *p_param);
void logger();

   
#ifdef __cplusplus
}
#endif

#endif 
