#include "State.h"

static system_state current_state = normal;

system_state getCurrentState(){
    return current_state;
}

void setCurrentState(system_state state){
    current_state = state;
}

