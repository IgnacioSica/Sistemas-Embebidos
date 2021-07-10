#include"LogController.h"
#include "MenuController.h"

uint8_t *logs[250];
char logNumber = 0;
GPSPosition_t p_pos;
struct tm p_newtime;    
uint8_t bufferFrame[256];


void logTimer(void *p_param){
    while(1){
        vTaskDelay(pdMS_TO_TICKS(10000));
        xTaskCreate( logger, "logger", configMINIMAL_STACK_SIZE+100, NULL, tskIDLE_PRIORITY+1, NULL );
        
        /*while(!isTaskRunning){
            vTaskDelay(pdMS_TO_TICKS(10000));
        }
        isTaskRunning = true;
        xTaskCreate( logger, "logger", configMINIMAL_STACK_SIZE+100, NULL, tskIDLE_PRIORITY+1, NULL );*/
    }
}

void logger(void *p_param){
    uint8_t str_pos[50];
    uint8_t str_date[25];
    
    if(logNumber == 250){
        logNumber = 0;
    }
    uint8_t newLog[100];
    sprintf(str_pos, "latitud = %d, longitud = %d", p_pos.latitude, p_pos.longitude);
    
    sprintf(str_date, "[%d/%d/%d - %d:%d:%d]", p_newtime.tm_mday, p_newtime.tm_mon, p_newtime.tm_year, p_newtime.tm_hour, p_newtime.tm_min, p_newtime.tm_sec);
  
    sprintf(newLog, "\r\n %d [%s] %s\t %s", logNumber, getStateName(getCurrentState()) ,str_date, str_pos);
    logs[logNumber] = newLog;
    logNumber++;
    isTaskRunning = false;
    vTaskDelete(NULL);
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