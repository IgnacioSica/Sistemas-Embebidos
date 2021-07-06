
#ifndef STATE_H  
#define STATE_H

#ifdef __cplusplus
#endif

typedef enum {
    normal,
    warning,
    danger,
    debug
}system_state;

system_state getCurrentState();
void setCurrentState(system_state state);

#ifdef __cplusplus
 
#endif

#endif 