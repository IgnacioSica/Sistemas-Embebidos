#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "utils/LedsController.h"
#include "framework/Accelerometer/Accelerometer.h"
#include "framework/Analog/Analog.h"

typedef enum{
    send,
    receive,
    menu_principal,
    menu_principal_option
} menu_state;

void blinkLED( void *p_param );
void getAccelerometerValues(void *p_param);
void getAnalogValues(void *p_param);
void goToMenu(void *p_param);
void menuprueba(void *p_param);

enum system_state {
    normal,
    warning,
    danger,
    debug
};

static enum system_state current_state;


int main(void)
{   
    current_state = normal;
    SYSTEM_Initialize( );
    
    while(!ACCEL_init()){}
    
    BTN1_SetDigitalInput();
    
    //xTaskCreate( blinkLED, "blink leds", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    //xTaskCreate( getAccelerometerValues, " acceletometer", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreate( ANALOG_convert, " analog converter", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    xTaskCreate( getAnalogValues, "get value from analog converter", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreate( goToMenu, "go to menu", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreate( menuprueba, "go to prueba", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    
    vTaskStartScheduler( );

    for(;;);
}

void menuprueba(void *p_param){
    while(1){
        if(current_state == debug){
            setLedColor(BLACK,8);
        }
    }
}

void goToMenu(void *p_param){
    while(1){
        if(current_state != debug && BTN1_GetValue()){
            current_state = debug;
            vTaskDelay(pdMS_TO_TICKS(1000));
            
        }else if(current_state == debug && BTN1_GetValue()){
            current_state = normal;
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        
        else{
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        
    }
}

void getAnalogValues(void *p_param){
    uint16_t value = 0;
    
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000));
        
        value = ANALOG_getResult();
        
        gradualLed(value);
        
    }
}

void getAccelerometerValues(void *p_param){
    float accel = 0;
    
    while(1){
        if(current_state != debug){
            ACCEL_Mod(&accel);  

            if(accel >= 1.5){
                current_state = danger;
            }
            else if(accel >= 1.2){
                current_state = warning;
            }
            else{
                current_state = normal;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
} 

void blinkLED( void *p_param ){
    bool blinkWarning = true;
    bool blinkDanger = true;
    
    while (1) {
        if(current_state == normal){
            setLedColor(GREEN, 8);
            blinkWarning = true;
            blinkDanger = true;
        } else if(current_state == warning){
            if(blinkWarning){
                //blinkLed(YELLOW);
                setLedColor(YELLOW, 8);
                blinkWarning = false;
            }
            blinkDanger = true;
        } else if(current_state == danger){
            if(blinkDanger){
                //blinkLed(RED);
                setLedColor(RED, 8);
                blinkDanger = false;
            }
            blinkWarning = true;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void vApplicationMallocFailedHook( void ){
    taskDISABLE_INTERRUPTS( );
    
    for(;;);
}

void vApplicationIdleHook( void ){}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName ){
    (void) pcTaskName;
    (void) pxTask;

    taskDISABLE_INTERRUPTS( );
    
    for(;;);
}

void vApplicationTickHook( void ){}

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
