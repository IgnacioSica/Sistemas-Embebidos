#include "buttons.h"
#include "../mcc_generated_files/pin_manager.h"

bool BTN1_Value = false;
bool BTN2_Value = false;

void BTN1_SetState(){
    BTN1_Value = !BTN1_Value;
}

bool BTN1_GetState(){
    return BTN1_Value;
}

void BTN1_Reset(){
    BTN1_Value = false;
}

void BTN2_SetState(){
    BTN2_Value = !BTN2_Value;
}

bool BTN2_GetState(){
    return BTN2_Value;
}

void BTN2_Reset(){
    BTN2_Value = false;
}
