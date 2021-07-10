#include <math.h>
#include "LedsController.h"
#include "FreeRTOS.h"

void setLedColor (ws2812_t color, uint8_t ledsToLight){
    ws2812_t led_arr[8];
    int i;
    for(i =0; i <= ledsToLight; i++){
        led_arr[i] = color;
    }
    for(i = ledsToLight; i <= 8; i++){
        led_arr[i] = BLACK;
    }
    
    WS2812_send(led_arr, 8);
}

void blinkLed(ws2812_t color){
    int i;
    for(i = 0; i < 3; i++){
        setLedColor(BLACK, 8);
        vTaskDelay(pdMS_TO_TICKS(167));
        setLedColor(color, 8);
        vTaskDelay(pdMS_TO_TICKS(167));
    }
}

uint8_t gradualLed(int ADCValue) {
    float ADCLevel = ADCValue / 128.0;
    
    if (ADCLevel == 0) {
        ADCLevel = 1;
    }
    
    ADCLevel = ceil(ADCLevel);
    uint8_t ledsToLight = ADCLevel;
    return ledsToLight;
}