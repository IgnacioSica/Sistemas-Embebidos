#include "FreeRTOS.h"
#include "task.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "platform/LEDs_RGB/WS2812.h"
#include "mcc_generated_files/usb/usb_device.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "platform/LEDs_RGB/WS2812.h"
#include "mcc_generated_files/rtcc.h"

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
    menu_fecha,
    menu_change,
    select_led,
    select_color,
    ingresar_ano,
    ingresar_mes,
    ingresar_dia,
    ingresar_hs,
    ingresar_min,
    ingresar_sec,
} menu_state;

typedef struct{
    menu_state newState;
    char *message;
    char *buffer;
} usb_params;

typedef struct{
    uint8_t  led;
    uint8_t  color;
    uint32_t time;
}app_register_t;

menu_state current_state;
menu_state newState;
static char message[256] = "valor inicial";
static uint8_t buffer[64];
static ws2812_t led_arr[8];
static struct tm datetime;
app_register_t last_change;

void blinkLED( void *p_param );
void colorLED( void *p_param );
void setDATETIME( void *p_param );
void menu( void *p_param );
void controllerUSB( void *p_param );
void showLASTCHANGE (void *p_param);

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
    xTaskCreate( setDATETIME, "ingresar fecha y hora", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    xTaskCreate( showLASTCHANGE, "mostra ultimo cambio", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
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
                    current_state = menu_fecha;
                }else if(selected_OPTION == 2){
                    current_state = menu_leds;
                }else if(selected_OPTION == 3){
                    current_state = menu_change;
                }
            } else {
                newState = menu_principal_option;
                current_state = send;
            }
        } else {
            vTaskDelay( xDelay100ms );
        }
    }
}

void showLASTCHANGE (void *p_param){
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    while(1){
        if(current_state == menu_change){
            strncpy(message, "\r\nLed\r\n", sizeof(message));
            strcat(&message, &last_change.color);
            newState = menu_principal;
            current_state = send;
        } else {
            vTaskDelay( xDelay100ms );
        }
    }
}

void setDATETIME( void *p_param ){
    int input = 0;
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    while(1){
        input = 0;
        if(current_state == menu_fecha){
            strncpy(message, "\r\n Ingrese el ano\r\n", sizeof(message));
            newState = ingresar_ano;
            current_state = send;
        } else if(current_state == ingresar_ano){
            input = atoi(buffer);
            if(input > 0 && input < 3000){
                datetime.tm_year = input;
                strncpy(message, "\r\n Ingrese el mes del ano\r\n", sizeof(message));
                newState = ingresar_mes;
                current_state = send;
            } else {
                newState = ingresar_ano;
                current_state = send;
            }
        } else if(current_state == ingresar_mes){
            input = atoi(buffer);
            if(input > 0 && input < 13){
                datetime.tm_mon = input;
                strncpy(message, "\r\n Ingrese el dia del mes\r\n", sizeof(message));
                newState = ingresar_dia;
                current_state = send;
            } else {
                newState = ingresar_mes;
                current_state = send;
            }
        }else if(current_state == ingresar_dia){
            input = atoi(buffer);
            if(input > 0 && input < 32){
                datetime.tm_mday = input;
                strncpy(message, "\r\n Ingrese la hora del dia\r\n", sizeof(message));
                newState = ingresar_hs;
                current_state = send;
            } else {
                newState = ingresar_dia;
                current_state = send;
            }
        }else if(current_state == ingresar_hs){
            input = atoi(buffer);
            if(input > 0 && input < 61){
                datetime.tm_hour = input;
                strncpy(message, "\r\n Ingrese el minuto de la hora\r\n", sizeof(message));
                newState = ingresar_min;
                current_state = send;
            } else {
                newState = ingresar_hs;
                current_state = send;
            }
        }else if(current_state == ingresar_min){
            input = atoi(buffer);
            if(input > 0 && input < 61){
                datetime.tm_min = input;
                strncpy(message, "\r\n Ingrese el segundo de la hora\r\n", sizeof(message));
                newState = ingresar_sec;
                current_state = send;
            } else {
                newState = ingresar_min;
                current_state = send;
            }
        } else if(current_state == ingresar_sec){
            input = atoi(buffer);
            if(input > 0 && input < 61){
                datetime.tm_sec = input;
                RTCC_TimeSet(&datetime);
                current_state = menu_principal;
            } else {
                newState = ingresar_sec;
                current_state = send;
            }
        }else {
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
                last_change.led = selected_LED;
                last_change.color = selected_COLOR;
                struct tm current_time;
                RTCC_TimeGet(&current_time);
                last_change.time = mktime(&current_time);
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
                    strncpy(buffer, "", sizeof(buffer));
                    numBytes = getsUSBUSART(buffer,sizeof(buffer));
                    if(numBytes > 0){
                        putsUSBUSART(buffer);
                        current_state = newState;
                    } else {
                        vTaskDelay( xDelay100ms );
                    }
                } else if(current_state == send){
                    putsUSBUSART(message);
                    current_state = receive;
                } else {
                    vTaskDelay( xDelay100ms );
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
