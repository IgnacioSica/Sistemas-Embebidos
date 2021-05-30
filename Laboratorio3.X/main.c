#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include <stdbool.h>
#include "mcc_generated_files/usb/usb_device.h"
#include "mcc_generated_files/usb/usb_device_cdc.h"
#include "utils.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "platform/LEDs_RGB/WS2812.h"

enum global_state {
	menu_Principal,
	menu_FechaHora,
	menu_ColorLeds,
    menu_Consultar,
};

int main(void){
    SYSTEM_Initialize();
    
    enum global_state current_state;
	current_state = menu_Principal;
    
    LEDA_SetDigitalOutput();
    LEDA_SetLow();
    
    static bool send_menu = true;
    static bool send_echo = true;
    
    static bool send = true;
    static uint8_t numBytes;
    static uint8_t buffer[64];
    
    ut_tmrDelay_t delay;
    
    delay.startValue = TMR2_SoftwareCounterGet();
    delay.state = UT_TMR_DELAY_WAIT;
    
    uint32_t current_delay = 6000;
    
    LED_CTRL_SetDigitalOutput();
    
    //ws2812_t rgb_values = {.r = 0, .g = 255, .b = 255};
    /*ws2812_t rgb_values;
    rgb_values.r = 255;
    rgb_values.g = 0;
    rgb_values.b = 0;*/
    ws2812_t led_arr[8];
    
    int i;
    for(i = 0; i < 8; i++){
        ws2812_t rgb_values = {.r = 0, .g = 0, .b = 0};
        led_arr[i] = rgb_values;
    }
    
    /*led_arr[0].r = WHITE.r;
    led_arr[1] = RED;
    led_arr[2] = BLUE;
    led_arr[3] = GREEN;
    led_arr[4] = GREEN;
    led_arr[5] = GREEN;
    led_arr[6] = GREEN;
    led_arr[7] = BLUE;*/
        
    while (1)
    {
        WS2812_send(&led_arr, 8);
        
        CDCTxService();
        USBDeviceTasks();
        if((USBGetDeviceState() < CONFIGURED_STATE) || (USBGetSuspendState() == true)){
            continue;
        } else { 
            if (current_state == menu_Principal) {
                if(send && USBUSARTIsTxTrfReady()){
                    if(UT_delayms(&delay, current_delay)){
                        char data[] = "Menu Principal: (1) Fijar fecha y hora del sistema, (2) Encender/Apagar un led particular de un color fijo y (3) Consultar el estado y fecha y hora";
                        putsUSBUSART(data);
                        send = false;
                    }
                }
                if(!send && USBUSARTIsTxTrfReady()){
                    numBytes = getsUSBUSART(buffer,sizeof(buffer));
                    if(numBytes > 0){
                        if(buffer[0] == '1'){
                            current_state = menu_FechaHora;
                            putsUSBUSART("Fecha y Hora: ");
                        }
                        if(buffer[0] == '2'){
                            current_state = menu_ColorLeds;
                            putsUSBUSART("Color leds");
                        }
                        if(buffer[0] == '3'){
                            current_state = menu_Consultar;
                            putsUSBUSART("Consultar");
                        }
                        send = true;
                    }
                    numBytes = 0;
                }
            }
            
            if (current_state == menu_FechaHora) {
                if(send && USBUSARTIsTxTrfReady()){
                    if(UT_delayms(&delay, current_delay)){
                        char data[] = "Ingrese la fecha y hora del sistema en el siguiente formato: YYYYMMDDHHMMSSFFFFFF";
                        putsUSBUSART(data);
                        send = false;
                    }
                }
                if(!send && USBUSARTIsTxTrfReady()){
                    numBytes = getsUSBUSART(buffer,sizeof(buffer));
                    if(numBytes > 0){
                        putsUSBUSART(buffer);
                        send = true;
                    }
                    numBytes = 0;
                    
                    if(isdatetime(buffer)){
                        char data[] = " La fecha y hora se ingresaron correctamente ";
                        putsUSBUSART(data);
                    } else {
                        char data[] = " Ingrese la fecha y hora en el formato correcto ";
                        putsUSBUSART(data);
                    }
                }
            }
        }
    }
    return 1; 
}

int isdatetime(const char *datetime)
{
    // datetime format is YYYYMMDDHHMMSSFFFFFF
    struct tm   time_val;
    unsigned    microsecs;
    int         nbytes;
    const char *end = strftime(datetime, "%Y%m%d%H%M%S", &time_val);

    if (end != 0 && strlen(end) == 6 &&
        sscanf(end, "%6u%n", &microsecs, &nbytes) == 1 && nbytes == 6)
        return 1;   // Valid
    return 0;       // Invalid
}
/*
 int main() {

	while(1) {
		if (current_state == A) {
			if (E == 0) {
				// Handler estado A
			} else {
				current_state = B;
			}		
		}

		if (current_state == B) {
			if (E == 1) {
				// Handler estado B
			} else {
				current_state = C;
			}		
		}

		if (current_state == C) {
			// Handler estado C
			current_state = A;	
		}
	}
}*/

/*if(send && USBUSARTIsTxTrfReady()){
                if(UT_delayms(&delay, current_delay)){
                    char data[] = "Menu Principal: (1) Fijar fecha y hora del sistema, (2) Encender/Apagar un led particular de un color fijo y (3) Consultar el estado y fecha y hora";
                    putsUSBUSART(data);
                    send = false;
                }
            } 
            
            if(!send && USBUSARTIsTxTrfReady()){
                uint8_t numBytes;
                uint8_t buffer[1];
                numBytes = getsUSBUSART(buffer,sizeof(buffer));
                if(numBytes > 0){
                    putsUSBUSART(buffer);
                    send = true;
                }
            }
            
            if(send_echo && USBUSARTIsTxTrfReady()){
                if(UT_delayms(&delay, current_delay)){
                    uint8_t numBytes;
                    uint8_t buffer[5];
                    numBytes = getsUSBUSART(buffer,sizeof(buffer));
                    if(numBytes > 0){
                        putsUSBUSART(buffer);
                    }
                    send_echo = false;
                }
            } */

//USBDeviceTasks(); 
        //send = UT_delayms(&delay, 1000);

        /*if((USBGetDeviceState() < CONFIGURED_STATE) || (USBGetSuspendState() == true)){
            continue;
        } else {
            CDCTxService();
            if(sendonce){
                putsUSBUSART("TEST");
                delay.startValue = TMR2_SoftwareCounterGet();
                delay.state = UT_TMR_DELAY_INIT;
            }
        }*/

        /*if(UT_delayms(&delay, current_delay)){
            if(delay.state == UT_TMR_DELAY_WAIT){
                delay.state = UT_TMR_DELAY_INIT;
                current_delay = 400;
            } else {
                delay.state = UT_TMR_DELAY_WAIT;
                current_delay = 800;
            }
            delay.startValue = TMR2_SoftwareCounterGet();
            LEDA_Toggle();
        }*/