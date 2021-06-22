#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "utils/LedsController.h"

void blinkLED( void *p_param );

enum system_state {
    normal,
    warning,
    danger,
    debug
};

static enum system_state current_state;

int main(void)
{   
    current_state = danger;
    SYSTEM_Initialize( );
    
    while(!ACCEL_init()){}
    
    xTaskCreate( blinkLED, "blink leds", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    vTaskStartScheduler( );

    for(;;);
}

void blinkLED( void *p_param ){
    bool blinkWarning = true;
    bool blinkDanger = true;
    
    while (1) {
        if(current_state == normal){
            setLedColor(GREEN);
            blinkWarning = true;
            blinkDanger = true;
        } else if(current_state == warning){
            if(blinkWarning){
                blinkLed(YELLOW);
                blinkWarning = false;
            }
            blinkDanger = true;
        } else if(current_state == danger){
            if(blinkDanger){
                blinkLed(RED);
                blinkDanger = false;
            }
            blinkWarning = true;
        }
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
