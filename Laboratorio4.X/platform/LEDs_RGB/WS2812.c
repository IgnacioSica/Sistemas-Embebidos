#include "WS2812.h"
#include "../../mcc_generated_files/pin_manager.h"
#include "../../mcc_generated_files/interrupt_manager.h"

const ws2812_t WHITE    = {255, 255, 255};
const ws2812_t RED      = {255, 0, 0};
const ws2812_t GREEN    = {0, 255, 0};
const ws2812_t BLUE     = {0, 0, 255};

static uint32_t WS2812_bitFlip( uint8_t p_b )
{
    p_b = (p_b & 0xF0) >> 4 | (p_b & 0x0F) << 4;
    p_b = (p_b & 0xCC) >> 2 | (p_b & 0x33) << 2;
    p_b = (p_b & 0xAA) >> 1 | (p_b & 0x55) << 1;
    return (uint32_t)p_b;
}

static void WS2812_sendSingle( ws2812_t* p_led )
{
    uint8_t j;
    uint32_t val;

    val = (WS2812_bitFlip(p_led->b) << 16) + (WS2812_bitFlip(p_led->r) << 8) + (WS2812_bitFlip(p_led->g));

    for( j = 0; j < 24; j++ )
    {
        
        if(val&1){
            LED_CTRL_SetHigh( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            LED_CTRL_SetLow( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
        }else{
            LED_CTRL_SetHigh( ); 
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            LED_CTRL_SetLow( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
            _nop( );
        }
        val=val>>1;
    }
}

void WS2812_send( ws2812_t *p_leds, uint8_t p_length )
{
    uint8_t i;
    
    INTERRUPT_GlobalDisable();
    
    for( i=0; i<p_length; i++ )
    {
        WS2812_sendSingle( (p_leds+i) );
    }
    
    INTERRUPT_GlobalEnable();
}
