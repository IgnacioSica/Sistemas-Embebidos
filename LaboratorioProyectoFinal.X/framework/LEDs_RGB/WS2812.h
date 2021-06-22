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
extern const ws2812_t BLACK;
extern const ws2812_t YELLOW;

void WS2812_send( ws2812_t *p_leds, uint8_t p_length );

#endif
