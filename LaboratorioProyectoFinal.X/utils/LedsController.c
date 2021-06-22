#include "LedsController.h"
#include "FreeRTOS.h"

void setLedColor (ws2812_t color){
    ws2812_t led_arr[8];
    int i;
    for(i = 0; i < 8; i++){
        led_arr[i] = color;
    }
    
    WS2812_send(led_arr, 8);
}

void blinkLed(ws2812_t color){
    int i;
    for(i = 0; i < 3; i++){
        setLedColor(BLACK);
        vTaskDelay(pdMS_TO_TICKS(167));
        setLedColor(color);
        vTaskDelay(pdMS_TO_TICKS(167));
    }
}