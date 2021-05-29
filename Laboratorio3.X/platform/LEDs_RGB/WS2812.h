#ifndef _WS2812_H
#define _WS2812_H

#include <stdint.h>

typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}ws2812_t;

extern const ws2812_t WHITE;
extern const ws2812_t RED;
extern const ws2812_t GREEN;
extern const ws2812_t BLUE;

/**
  @Function
    void WS2812_send( ws2812_t* p_led );

  @Summary
    Transmite un valor rgb a un led.

  @Description
    Transmite un valor rgb a un led.

  @Precondition
    None

  @Parameters
    @param p_led Puntero a la estrucutra que contiene los valores para configurar un led individual.


  @Returns
    void

  @Remarks
    None.
 */
void WS2812_send( ws2812_t *p_leds, uint8_t p_length );

#endif /* _WS2812_H */

/* *****************************************************************************
 End of File
 */
