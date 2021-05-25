#include "utils.h"
#include "mcc_generated_files/rtcc.h"
#include "mcc_generated_files/pin_manager.h"

#include <stdbool.h>
#include <time.h>

uint16_t counter;
bool statusTimer1;

bool UT_delayms(ut_tmrDelay_t* p_timer, uint32_t p_ms){
    
    if(TMR2_SoftwareCounterGet() >= (p_timer->startValue + p_ms)){
        return true;
    }
    
    return false;     
}
