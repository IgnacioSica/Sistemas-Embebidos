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
        xTaskCreate( logger, "logger", configMINIMAL_STACK_SIZE+100, NULL, tskIDLE_PRIORITY+1, NULL );
       
    }
}

void logger(void *p_param){
    uint8_t str[250];
    if(logNumber == 250){
        logNumber = 0;
    }
    log newLog;
    //formatterPos(&p_pos, &str);
    //newLog.lat_lon = str;
    
    sprintf(str, "latitud = %d, longitud = %d", p_pos.latitude, p_pos.longitude);
    strcpy(newLog.lat_lon, str);
    
    //formatterTime(&p_newtime, &str);
    //newLog.date;
    sprintf(str, "[%d/%d/%d - %d:%d:%d]", p_newtime.tm_mday, p_newtime.tm_mon, p_newtime.tm_year, p_newtime.tm_hour, p_newtime.tm_min, p_newtime.tm_sec);
    strcpy(newLog.date, str);
   
    newLog.state = getCurrentState();
    newLog.log_number = logNumber;
    logs[logNumber] = newLog;
    logNumber++;
    vTaskDelete(NULL);
}

void formatterPos(GPSPosition_t *position, uint8_t *str){
    sprintf(str, "latitud = %d, longitud = %d", position->latitude, position->longitude);
}

//"[%d/%d/%d - %d:%d:%d]"
//[DD/MM/YYYY - HH:MM:SS]
void formatterTime(struct tm *p_newtime, uint8_t *str){
    sprintf(str, "[%d/%d/%d - %d:%d:%d]", p_newtime->tm_mday, p_newtime->tm_mon, p_newtime->tm_year, p_newtime->tm_hour, p_newtime->tm_min, p_newtime->tm_sec);
}

log* getLogs(){
    return logs;
}

void pruebaGPS(void *p_param){
    uint8_t bufferFrame[256];
    GPSPosition_t p_pos;
    struct tm p_newtime;
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        do{
            SIM808_getNMEA(bufferFrame);
          
        }while(SIM808_validateNMEAFrame(bufferFrame));
        
        GPS_getPosition( &p_pos,  &bufferFrame);
        
        GPS_getUTC( &p_newtime, &bufferFrame);
        
    }
}