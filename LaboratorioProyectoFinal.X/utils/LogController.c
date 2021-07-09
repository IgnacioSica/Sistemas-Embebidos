#include"LogController.h"
#include "MenuController.h"

log logs[250] = {};
char logNumber = 0;
GPSPosition_t p_pos;
struct tm p_newtime;    
uint8_t bufferFrame[256];

void logTimer(void *p_param){
    while(1){
        vTaskDelay(pdMS_TO_TICKS(10000));
        logger(&logs);
    }
}

void logger(log* logs){
    uint8_t str;
    if(logNumber == 250){
        logNumber = 0;
    }
    log newLog;
    //completar newLog
    formatterPos(&p_pos, &str);
    newLog.lat_lon = str;
    logs[logNumber] = newLog;
    logNumber++;
}

void formatterPos(GPSPosition_t *position, uint8_t *str){
    sprintf(str, "latitud = %d" , 1 );
}

log* getLogs(){
    return logs;
}

void prueba(void *p_param){
    while(1){
        do{
            SIM808_getNMEA(bufferFrame);
          
        }while(SIM808_validateNMEAFrame(bufferFrame));
        
        GPS_getPosition( &p_pos,  &bufferFrame);
        GPS_getUTC( &p_newtime, &bufferFrame);
    }
}