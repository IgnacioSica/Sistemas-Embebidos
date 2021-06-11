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

static uint8_t numBytes;
static uint8_t buffer[64];

enum usb_state {
    send,
    receive
};

enum ledColor_state {
    hide,
    show_menu,
    select_led,
    select_color
};

typedef enum{
    
} menu_state;

typedef struct{
    char message[];
    char &buffer[];
    menu_state newState;
} usb_params;

void blinkLED( void *p_param );
void colorLED( void *p_param );
void controllerUSB( void *p_param );

TaskHandle_t TaskHandle = NULL;

enum usb_state current_stateUSB;
enum ledColor_state current_stateLED;

int main(void)
{
    SYSTEM_Initialize( );
    
    LEDA_SetDigitalOutput();
    LEDA_SetLow();
    
    CDCTxService();
    USBDeviceTasks();
    
    current_stateUSB = receive;
    current_stateLED = hide;
    
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
    
    xTaskCreate( controllerUSB, "controller usb", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, TaskHandle );
    xTaskCreate( colorLED, "color LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    
    vTaskStartScheduler( );

    for(;;);
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

void colorLED( void *p_param ){
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    static int selected_LED = 0;
    ws2812_t led_arr[8];
    while(1){
        //USBDeviceTasks();
        if((USBGetDeviceState() < CONFIGURED_STATE) || (USBGetSuspendState() == true)){
            continue;
        } else { 
            CDCTxService();
            if(USBUSARTIsTxTrfReady()){
                if(current_stateLED == hide){
                    numBytes = getsUSBUSART(buffer,sizeof(buffer));
                    if(numBytes > 0){
                        current_stateLED = show_menu;
                    } else {
                        vTaskDelay( xDelay100ms );
                    }
                } else if(current_stateLED == show_menu){
                    char data[128] = "Cambiar color de LED\r\n Ingresa el numero del led a modificar del 1 - 8\r\n";
                    putsUSBUSART(data);
                    //putsUSBUSART(buffer);
                    current_stateLED = select_led;
                } else if(current_stateLED == select_led){
                    static uint8_t local_buffer[1];
                    numBytes = getsUSBUSART(buffer,sizeof(buffer));
                    if(numBytes > 0){
                        selected_LED = atoi(local_buffer);
                        current_stateLED = select_color;
                    } else {
                        vTaskDelay( xDelay100ms );
                    }
                } else if(current_stateLED == select_color){
                    ws2812_t rgb_values = {.r = 0, .g = 255, .b = 0};
                    led_arr[selected_LED] = rgb_values;
                    WS2812_send(&led_arr, 8);
                    current_stateLED = hide;
                }
            }
        }
    }
}

void controllerUSB( void *p_param ){
    usb_params *usbParams;
    usbParams = ( usb_params * ) p_param;
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    while(1){
        if((USBGetDeviceState() < CONFIGURED_STATE) || (USBGetSuspendState() == true)){
            continue;
        } else { 
            CDCTxService();
            if(USBUSARTIsTxTrfReady()){
                if(current_stateUSB == receive){
                    numBytes = getsUSBUSART(&usbParams->buffer,sizeof(&usbParams->buffer));
                    if(numBytes > 0){
                        current_stateUSB = &usbParams->newState;
                        vTaskDelete( TaskHandle );
                    } else {
                        vTaskDelay( xDelay100ms );
                    }
                } else if(current_stateUSB == send){
                    char data[] = &usbParams->message;
                    putsUSBUSART(data);
                    current_stateUSB = receive;
                }
            }
        }
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
