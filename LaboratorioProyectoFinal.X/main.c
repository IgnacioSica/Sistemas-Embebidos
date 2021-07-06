#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "utils/LedsController.h"
#include "framework/Accelerometer/Accelerometer.h"
#include "framework/Analog/Analog.h"
#include "utils/MenuController.h"
#include "utils/AccelerometerController.h"

//void goToMenu(void *p_param);

/*enum system_state {
    normal,
    warning,
    danger,
    debug
};*/

//static TaskHandle_t taskHandle_menu = menu;


int main(void)
{   
    //current_state = normal;
    SYSTEM_Initialize( );
    
    while(!ACCEL_init()){}
    
    BTN1_SetDigitalInput();
    BTN2_SetDigitalInput();
    
    xTaskCreate( changeLedColor, "blink leds", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreate( getAccelerometerValues, " acceletometer", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreate( ANALOG_convert, " analog converter", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    xTaskCreate( goToMenu, "go to menu", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreate( controllerUSB, "controller usb", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );

    vTaskStartScheduler( );

    for(;;);
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
