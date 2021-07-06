#include "MenuController.h"

menu_state current_state = menu_principal;
menu_state newState;

static char message[256] = "valor inicial";
static uint8_t buffer[64];
TaskHandle_t taskHandle_umbrales = NULL;

void getAnalogValues(void *p_param);

void menu( void *p_param ){
    static int selected_OPTION = 0;
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    while(1){
        if(current_state == menu_principal){
            setLedColor(WHITE, 8);
            strncpy(message, "\r\nMenu Principal: \r\n (1) Configurar umbrales\r\n (2) Encender/Apagar un led particular de un color fijo\r\n (3) Consultar el estado y fecha y hora", sizeof(message));
            newState = menu_principal_option;
            current_state = send;
        } else if(current_state == menu_principal_option){
            selected_OPTION = atoi(buffer);
            if(selected_OPTION > 0 && selected_OPTION < 4){
                if(selected_OPTION == 1){
                    current_state = umbral_warning;
                    xTaskCreate( getAnalogValues, "get analog values", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, taskHandle_umbrales);
                    
                }else if(selected_OPTION == 2){
                    //current_state = menu_leds;
                }else if(selected_OPTION == 3){
                    //current_state = menu_change;
                }
            } else {
                newState = menu_principal_option;
                current_state = send;
            }
        } else {
            vTaskDelay( xDelay100ms );
        }
    }
}

void controllerUSB( void *p_param ){
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    while(1){
        if((USBGetDeviceState() < CONFIGURED_STATE) || (USBGetSuspendState() == true)){
            vTaskDelay( xDelay100ms );
            continue;
        } else { 
            CDCTxService();
            if(USBUSARTIsTxTrfReady()){
                if(current_state == receive){
                    uint32_t numBytes;
                    strncpy(buffer, "", sizeof(buffer));
                    numBytes = getsUSBUSART(buffer,sizeof(buffer));
                    if(numBytes > 0){
                        putsUSBUSART(buffer);
                        current_state = newState;
                    } else {
                        vTaskDelay( xDelay100ms );
                    }
                } else if(current_state == send){
                    putsUSBUSART(message);
                    current_state = receive;
                } else {
                    vTaskDelay( xDelay100ms );
                }
            }
        }
    }
}

void getAnalogValues(void *p_param){
    uint16_t value = 0;
    
    while(1){
        if(current_state == umbral_warning || current_state == umbral_danger){
            vTaskDelay(pdMS_TO_TICKS(200));

            value = ANALOG_getResult();

            uint8_t valueLedsToLight = gradualLed(value);

            setLedColor(BLUE, valueLedsToLight);
            
            if(current_state == umbral_warning && BTN2_GetValue()){
                setWarningLevel(valueLedsToLight); 
                blinkLed(BLUE);
                current_state = umbral_danger;
            }else if(current_state == umbral_danger && BTN2_GetValue()){
                
                setDangerLevel(valueLedsToLight);
                blinkLed(BLUE);
                current_state = menu_principal;
                vTaskDelete(NULL);
            }
        }else{
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}