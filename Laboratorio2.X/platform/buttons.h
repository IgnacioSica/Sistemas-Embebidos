#include <stdbool.h>

#ifndef _BUTTONS_H 
#define _BUTTONS_H

#ifdef __cplusplus
extern "C" {
#endif

    void BTN1_SetState();
    bool BTN1_GetState();
    void BTN1_Reset();
    void BTN2_SetState();
    bool BTN2_GetState();
    void BTN2_Reset();

#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */
