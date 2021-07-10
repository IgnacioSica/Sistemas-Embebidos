#include "MenuController.h"
#include "State.h"

menu_state current_state = menu_principal;
menu_state newState;

static TaskHandle_t taskHandle_menu = NULL;
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
            strncpy(message, "\r\nMenu Principal: \r\n (1) Configurar umbrales\r\n (2) Ver lista de logs\r\n", sizeof(message));
            newState = menu_principal_option;
            current_state = send;
        } else if(current_state == menu_principal_option){
            selected_OPTION = atoi(buffer);
            if(selected_OPTION > 0 && selected_OPTION < 4){
                if(selected_OPTION == 1){
                    current_state = umbral_warning;
                    xTaskCreate( getAnalogValues, "get analog values", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &taskHandle_umbrales);
                    
                }else if(selected_OPTION == 2){
                    sprintf(message, "%d", 8);
                    /*uint8_t i;
                    for(i=0; i < 250 ;i++){
                        while(current_state == only_send){
                        }
                        //sprintf(message, "%d", 8);
                        //sprintf(message, "%s, %s, %s, %d", logs[i].log_number, logs[i].lat_lon, logs[i].date, logs[i].state);
                        current_state = only_send;*/
                    //}
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


void showControllerUSB( void *p_param ){
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    while(1){
        if((USBGetDeviceState() < CONFIGURED_STATE) || (USBGetSuspendState() == true)){
            vTaskDelay( xDelay100ms );
            continue;
        } else { 
            CDCTxService();
            if(USBUSARTIsTxTrfReady()){
                if(current_state == only_send){
                    putsUSBUSART(message);
                    current_state = menu_principal;
                } else {
                    vTaskDelay( xDelay100ms );
                }
            }
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

void goToMenu(void *p_param){
    while(1){
        if(getCurrentState() != debug && BTN1_GetValue()){
            setCurrentState(debug);
            xTaskCreate( menu, "go to menu controller", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &taskHandle_menu );
            vTaskDelay(pdMS_TO_TICKS(1000));
            
        }else if(getCurrentState() == debug && BTN1_GetValue()){
            setCurrentState(normal);
            vTaskDelay(pdMS_TO_TICKS(1000));
            vTaskDelete(taskHandle_menu);
        }
        else{
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}