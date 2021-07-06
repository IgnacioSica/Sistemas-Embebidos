#include"LogController.h"

log logs[250] = {};
char logNumber = 0;

void logTimer(void *p_param){
    while(1){
        vTaskDelay(pdMS_TO_TICKS(10000));
        logger();
    }
}

void logger(){
    if(logNumber == 250){
        logNumber = 0;
    }
    log newLog;
    //completar newLog
    logs[logNumber] = newLog;
    logNumber++;
}