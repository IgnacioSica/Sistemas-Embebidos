#include "buttons.h"
#include "../mcc_generated_files/pin_manager.h"
#include "../utils/utils.h"

bool BTN1_Value = false;
bool BTN2_Value = false;

void BTN1_SetState(){
    LEDA_SetValue(true);
    flagA = true;
    offsetA = TMR2_SoftwareCounterGet();
}

bool BTN1_GetState(){
    return BTN1_Value;
}

void BTN1_Reset(){
    BTN1_Value = false;
}

void BTN2_SetState(){
    LEDB_SetValue(true);
    flagB = true;
    offsetB = TMR2_SoftwareCounterGet();
}

bool BTN2_GetState(){
    return BTN2_Value;
}

void BTN2_Reset(){
    BTN2_Value = false;
}
