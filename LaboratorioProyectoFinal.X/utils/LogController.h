
#ifndef LOG_CONTROLLER_H    
#define LOG_CONTROLLER_H 

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    tm date;
    current_state state;
    //posicion
}log;

log logs[250];

void logTimer(void *p_param);
void logger(void *p_param);

   
#ifdef __cplusplus
}
#endif

#endif 
