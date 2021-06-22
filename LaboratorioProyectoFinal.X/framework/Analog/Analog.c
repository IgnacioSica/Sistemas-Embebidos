#include "FreeRTOS.h"
#include "Analog.h"
#include <stdbool.h>
#include "../../mcc_generated_files/adc1.h"

static uint16_t conversionResult;

void ANALOG_convert( void *p_param )
{
    while(1)
    {
        ADC1_ChannelSelect( channel_POT );
        ADC1_SoftwareTriggerEnable();
        vTaskDelay(pdMS_TO_TICKS(5));
        ADC1_SoftwareTriggerDisable();
        while( !ADC1_IsConversionComplete() )
        {
            ADC1_Tasks( );
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        conversionResult = ADC1_ConversionResultGet();
    }
}

uint16_t ANALOG_getResult( void )
{
    return conversionResult;
}
