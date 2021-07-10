#include "State.h"

static system_state current_state = normal;

system_state getCurrentState(){
    return current_state;
}

void setCurrentState(system_state state){
    current_state = state;
}

const char* getStateName(system_state state) 
{
   switch (state) 
   {
      case normal: return "normal";
      case warning: return "warning";
      case danger: return "danger";
      case debug: return "debug";
   }
}
