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

typedef enum{
    send,
    receive,
    menu_principal,
    menu_principal_option,
    menu_leds,
    select_led,
    select_color
} menu_state;

typedef struct{
    menu_state newState;
    char *message;
    char *buffer;
} usb_params;

menu_state current_state;
menu_state newState;
static char message[256] = "valor inicial";
static uint8_t buffer[64];
static ws2812_t led_arr[8];

void blinkLED( void *p_param );
void colorLED( void *p_param );
void menu( void *p_param );
void controllerUSB( void *p_param );

TaskHandle_t TaskHandle = NULL;

usb_params globalParams;

int main(void)
{
    SYSTEM_Initialize( );
    
    LEDA_SetDigitalOutput();
    LEDA_SetLow();
    
    current_state = send;
    newState = menu_principal;
    strncpy(message, "Menu Principal:\r\n (1) Fijar fecha y hora del sistema\r\n (2) Encender/Apagar un led particular de un color fijo\r\n (3) Consultar el estado y fecha y hora", sizeof(message));

    xTaskCreate( blinkLED, "task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreate( controllerUSB, "controller usb", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    xTaskCreate( colorLED, "color LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    xTaskCreate( menu, "menu", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    
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

void menu( void *p_param ){
    static int selected_OPTION = 0;
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    while(1){
        if(current_state == menu_principal){
            strncpy(message, "\r\nMenu Principal: \r\n (1) Fijar fecha y hora del sistema\r\n (2) Encender/Apagar un led particular de un color fijo\r\n (3) Consultar el estado y fecha y hora", sizeof(message));
            newState = menu_principal_option;
            current_state = send;
        } else if(current_state == menu_principal_option){
            selected_OPTION = atoi(buffer);
            if(selected_OPTION > 0 && selected_OPTION < 4){
                if(selected_OPTION == 1){
                    current_state = send;
                }else if(selected_OPTION == 2){
                    current_state = menu_leds;
                }else if(selected_OPTION == 3){
                    current_state = send;
                }
            } else {
                newState = select_led;
                current_state = send;
            }
        } else {
            vTaskDelay( xDelay100ms );
        }
    }
}

void colorLED( void *p_param ){
    static int selected_LED = 0;
    static int selected_COLOR = 0;
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    while(1){
        if(current_state == menu_leds){
            strncpy(message, "\r\nColor leds\r\n Ingrese el numero de led a modificar del 1 al 8\r\n", sizeof(message));
            newState = select_led;
            current_state = send;
        } else if(current_state == select_led){
            selected_LED = atoi(buffer);
            if(selected_LED > 0 && selected_LED < 9){
                strncpy(message, "\r\n Ingrese el codigo de color del 1 al 5\r\n", sizeof(message));
                newState = select_color;
                current_state = send;
            } else {
                newState = select_led;
                current_state = send;
            }
        } else if (current_state == select_color){
            selected_COLOR = atoi(buffer);
            if(selected_COLOR > 0 && selected_COLOR < 6){
                if(selected_COLOR == 1){
                    led_arr[selected_LED] = WHITE;
                }else if(selected_COLOR == 2){
                    led_arr[selected_LED] = RED;
                }else if(selected_COLOR == 3){
                    led_arr[selected_LED] = GREEN;
                }else if(selected_COLOR == 4){
                    led_arr[selected_LED] = BLUE;
                }else if(selected_COLOR == 5){
                    ws2812_t rgb_values = {.r = 0, .g = 0, .b = 0};
                    led_arr[selected_LED] = rgb_values;
                }
                WS2812_send(&led_arr, 8);
                current_state = menu_principal;
            } else {
                newState = select_color;
                current_state = send;
            }
        } else {
            vTaskDelay( xDelay100ms );
        }
    }
}

void controllerUSB( void *p_param ){
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    while(1){
        if((USBGetDeviceState() < CONFIGURED_STATE) || (USBGetSuspendState() == true)){
            vTaskDelay( xDelay100ms );
            continue;
        } else { 
            CDCTxService();
            if(USBUSARTIsTxTrfReady()){
                if(current_state == receive){
                    uint32_t numBytes;
                    numBytes = getsUSBUSART(buffer,sizeof(buffer));
                    if(numBytes > 0){
                        current_state = newState;
                    } else {
                        vTaskDelay( xDelay100ms );
                    }
                } else if(current_state == send){
                    putsUSBUSART(message);
                    current_state = receive;
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
