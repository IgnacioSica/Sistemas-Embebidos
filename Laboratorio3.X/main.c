#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include<stdbool.h>
#include "mcc_generated_files/usb/usb_device.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "utils.h"
#include <time.h>


int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
    
    LEDA_SetDigitalOutput();
    
    LEDA_SetLow();
    
    bool state = false;
    ut_tmrDelay_t delay;
    
    delay.startValue = TMR2_SoftwareCounterGet();
    delay.state = UT_TMR_DELAY_INIT;
    
    while (1)
    {
        //USBDeviceTasks(); 
        //send = UT_delayms(&delay, 1000);

        /*if((USBGetDeviceState() < CONFIGURED_STATE) || (USBGetSuspendState() == true)){
            continue;
        } else {
            CDCTxService();
            if(send){
                putsUSBUSART("TEST");
                delay.startValue = TMR2_SoftwareCounterGet();
                delay.state = UT_TMR_DELAY_INIT;
            }
        }*/

        if(UT_delayms(&delay, 1000)){
            delay.startValue = TMR2_SoftwareCounterGet();
            LEDA_Toggle();
        }
    }
    return 1; 
}
