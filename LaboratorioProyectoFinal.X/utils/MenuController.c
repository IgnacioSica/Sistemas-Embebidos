
typedef enum{
    send,
    receive,
    menu_principal,
    menu_principal_option,
    menu_leds,
    menu_fecha,
    menu_change,
    select_led,
    select_color,
    ingresar_ano,
    ingresar_mes,
    ingresar_dia,
    ingresar_hs,
    ingresar_min,
    ingresar_sec,
} menu_state;

menu_state current_state;
menu_state newState;

void menu( void *p_param );
void controllerUSB( void *p_param );

usb_params globalParams;

int main(void)
{
    SYSTEM_Initialize( );
    
    LEDA_SetDigitalOutput();
    LEDA_SetLow();
    
    current_state = send;
    newState = menu_principal;
    strncpy(message, "Menu Principal:\r\n (1) Fijar fecha y hora del sistema\r\n (2) Encender/Apagar un led particular de un color fijo\r\n (3) Consultar el estado y fecha y hora", sizeof(message));

    xTaskCreate( blinkLED, "task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL );
    xTaskCreate( controllerUSB, "controller usb", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    xTaskCreate( colorLED, "color LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    xTaskCreate( setDATETIME, "ingresar fecha y hora", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    xTaskCreate( showLASTCHANGE, "mostra ultimo cambio", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    xTaskCreate( menu, "menu", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
    
    vTaskStartScheduler( );

    for(;;);
}

void menu( void *p_param ){
    static int selected_OPTION = 0;
    const TickType_t xDelay100ms = pdMS_TO_TICKS( 100UL );
    while(1){
        if(current_state == menu_principal){
            strncpy(message, "\r\nMenu Principal: \r\n (1) Fijar fecha y hora del sistema\r\n (2) Encender/Apagar un led particular de un color fijo\r\n (3) Consultar el estado y fecha y hora", sizeof(message));
            newState = menu_principal_option;
            current_state = send;
        } else if(current_state == menu_principal_option){
            selected_OPTION = atoi(buffer);
            if(selected_OPTION > 0 && selected_OPTION < 4){
                if(selected_OPTION == 1){
                    current_state = menu_fecha;
                }else if(selected_OPTION == 2){
                    current_state = menu_leds;
                }else if(selected_OPTION == 3){
                    current_state = menu_change;
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