#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include<stdbool.h>
#include "mcc_generated_files/usb/usb_device.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "utils.h"
#include <time.h>

#include "platform/LEDs_RGB/WS2812.h"


int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    LEDA_SetDigitalOutput();
    LEDA_SetLow();
    
    //bool state = false;
    
    ut_tmrDelay_t delay;
    
    delay.startValue = TMR2_SoftwareCounterGet();
    delay.state = UT_TMR_DELAY_WAIT;
    
    uint32_t current_delay = 800;
    
    LED_CTRL_SetDigitalOutput();
    
    //ws2812_t rgb_values = {.r = 0, .g = 255, .b = 255};
    /*ws2812_t rgb_values;
    rgb_values.r = 255;
    rgb_values.g = 0;
    rgb_values.b = 0;*/
    ws2812_t led_arr[8];
    
    int i;
    for(i = 0; i < 8; i++){
        ws2812_t rgb_values = {.r = 0, .g = 0, .b = 0};
        led_arr[i] = rgb_values;
    }
    
    /*led_arr[0].r = WHITE.r;
    led_arr[1] = RED;
    led_arr[2] = BLUE;
    led_arr[3] = GREEN;
    led_arr[4] = GREEN;
    led_arr[5] = GREEN;
    led_arr[6] = GREEN;
    led_arr[7] = BLUE;*/
        
    while (1)
    {
        WS2812_send(&led_arr, 8);
        CDCTxService();
        USBDeviceTasks();
        if((USBGetDeviceState() < CONFIGURED_STATE) || (USBGetSuspendState() == true)){
            continue;
        } else {
                    if(USBUSARTIsTxTrfReady())
        {
            char data[] = "Hello World";
            putsUSBUSART(data);
        }
            

                
            
        }
        
       
        //USBDeviceTasks(); 
        //send = UT_delayms(&delay, 1000);

        /*if((USBGetDeviceState() < CONFIGURED_STATE) || (USBGetSuspendState() == true)){
            continue;
        } else {
            CDCTxService();
            if(sendonce){
                putsUSBUSART("TEST");
                delay.startValue = TMR2_SoftwareCounterGet();
                delay.state = UT_TMR_DELAY_INIT;
            }
        }*/

        if(UT_delayms(&delay, current_delay)){
            if(delay.state == UT_TMR_DELAY_WAIT){
                delay.state = UT_TMR_DELAY_INIT;
                current_delay = 400;
            } else {
                delay.state = UT_TMR_DELAY_WAIT;
                current_delay = 800;
            }
            delay.startValue = TMR2_SoftwareCounterGet();
            LEDA_Toggle();
        }
    }
    return 1; 
}
