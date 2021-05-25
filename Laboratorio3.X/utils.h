#ifndef _UTILS_H    
#define _UTILS_H

#ifdef __cplusplus
#endif


#define UT_DELAY_CICLES 500

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
#endif

#endif

typedef enum
{
    UT_TMR_DELAY_INIT,
    UT_TMR_DELAY_WAIT
}UT_TMR_DELAY_STATE;

typedef struct
{
    uint32_t startValue;
    UT_TMR_DELAY_STATE state;
}ut_tmrDelay_t;

bool UT_delayms(ut_tmrDelay_t* p_timer, uint32_t p_ms);
