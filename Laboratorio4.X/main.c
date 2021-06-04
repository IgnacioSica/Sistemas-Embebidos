#include "FreeRTOS.h"
#include "task.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "platform/LEDs_RGB/WS2812.h"
#include "mcc_generated_files/usb/usb_device.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "platform/LEDs_RGB/WS2812.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

void blinkLED( void *p_param );
void controllerUSB( void *p_param );

static uint8_t numBytes;
static uint8_t buffer[64];

enum usb_state {
    send,
    receive
};

enum usb_state current_stateUSB;

int main(void)
{
    SYSTEM_Initialize( );
    
    LEDA_SetDigitalOutput();
    LEDA_SetLow();
    
    CDCTxService();
    USBDeviceTasks();
    
    current_stateUSB = receive;
    
    ws2812_t led_arr[8];
    time_t t;
    srand((unsigned) time(&t));
    
    int i;
    for(i = 0; i < 8; i++){
        //ws2812_t rgb_values = {.r = rand() % 255, .g = rand() % 255, .b = rand() % 255};
        ws2812_t rgb_values = {.r = 0, .g = 0, .b = 0};
        led_arr[i] = rgb_values;
    }
    
    WS2812_send(&led_arr, 8);

    xTaskCreate( blinkLED, "task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    
    xTaskCreate( controllerUSB, "controller usb", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    
    vTaskStartScheduler( );

    for(;;);
}

void controllerUSB( void *p_param ){
    while(1){
        if((USBGetDeviceState() < CONFIGURED_STATE) || (USBGetSuspendState() == true)){
            continue;
        } else { 
            if(USBUSARTIsTxTrfReady()){
                if(current_stateUSB == receive){
                    numBytes = getsUSBUSART(buffer,sizeof(buffer));
                    current_stateUSB = send;
                } else if(current_stateUSB == send){
                    char data[64] = "Hello World";
                    putsUSBUSART(data);
                    current_stateUSB = receive;
                }
            }
        }
    }
}

void blinkLED( void *p_param ){
    while (1) {
        if (LEDA_GetValue() > 0) {
            // Encendido
            vTaskDelay(pdMS_TO_TICKS(400));
        } else {
            // Apagado
            vTaskDelay(pdMS_TO_TICKS(800));
        }
        LEDA_Toggle();
    }
}

void vApplicationMallocFailedHook( void ){
    taskDISABLE_INTERRUPTS( );
    for(;;);
}

void vApplicationIdleHook( void ){

}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ){
    (void) pcTaskName;
    (void) pxTask;
    
    taskDISABLE_INTERRUPTS( );
    for(;;);
}

void vApplicationTickHook( void ){
    
}

void vAssertCalled( const char * pcFile, unsigned long ulLine ){
    volatile unsigned long ul=0;

    (void) pcFile;
    (void) ulLine;

    __asm volatile( "di");
    {
        while(ul==0){
            portNOP( );
        }
    }
    __asm volatile( "ei");
}
