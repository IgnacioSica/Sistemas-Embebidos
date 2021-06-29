#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "utils/LedsController.h"
#include "framework/Accelerometer/Accelerometer.h"


void blinkLED( void *p_param );
void getAccelerometerValues(void *p_param);

enum system_state {
    normal,
    warning,
    danger,
    debug
};

static enum system_state current_state;
static Accel_t accel;

int main(void)
{   
    current_state = normal;
    SYSTEM_Initialize( );
    
    while(!ACCEL_init()){}
    
    xTaskCreate( blinkLED, "blink leds", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreate( getAccelerometerValues, " acceletometer", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    vTaskStartScheduler( );

    for(;;);
}

void getAccelerometerValues(void *p_param){
    
    while(1){
        ACCEL_GetAccel (&accel);
        
        if(accel.Accel_Y < 0.8 || accel.Accel_Y > -0.8){
            if((accel.Accel_Z > 0.8 || accel.Accel_Z < -0.8) && (accel.Accel_X > 0.8 || accel.Accel_X < -0.8)){
                current_state = danger;
            }
            else if((accel.Accel_Z > 0.5 || accel.Accel_Z < -0.5) && (accel.Accel_X > 0.8 || accel.Accel_X < -0.8)){
                current_state = warning;
            }else{
                current_state = normal;
            }
        }else{
            current_state = danger;
        }
        //else if(accel.Accel_Z > 0.5 || accel.Accel_Z < -0.5){
        //    current_state = warning;
        //}
        //else{
        //    current_state = normal;
        //}
        vTaskDelay(pdMS_TO_TICKS(10));
    }
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
                //blinkLed(YELLOW);
                setLedColor(YELLOW);
                blinkWarning = false;
            }
            blinkDanger = true;
        } else if(current_state == danger){
            if(blinkDanger){
                //blinkLed(RED);
                setLedColor(RED);
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
