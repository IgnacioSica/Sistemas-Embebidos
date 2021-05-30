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
    ingresar_ano,
    ingresar_mes,
    ingresar_dia,
    ingresar_hs,
    ingresar_min,
    ingresar_sec,
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
    static struct tm datetime;
    
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
                        //char data[] = " Ingrese la fecha y hora del sistema ";
                        //putsUSBUSART(data);
                        current_state = ingresar_ano;
                        send = true;
                    }
                }
            }
            
            if (current_state == ingresar_ano) {
                if(send && USBUSARTIsTxTrfReady()){
                    if(UT_delayms(&delay, current_delay)){
                        char data[] = "Ingrese el ano";
                        putsUSBUSART(data);
                        send = false;
                    }
                }
                if(!send && USBUSARTIsTxTrfReady()){
                    uint8_t buffer_local[6];
                    numBytes = getsUSBUSART(buffer_local, sizeof(buffer_local));
                    if(numBytes > 0){
                        int i_buffer = atoi(buffer_local);
                        if(i_buffer > 0 && i_buffer < 3000){
                            datetime.tm_year = i_buffer;
                            current_state = ingresar_mes;
                            send = true;
                        } else {
                            send = true;
                        }
                    }
                    numBytes = 0;
                }
            }
            
            if (current_state == ingresar_mes) {
                if(send && USBUSARTIsTxTrfReady()){
                    if(UT_delayms(&delay, current_delay)){
                        char data[] = " Ingrese el mes ";
                        putsUSBUSART(data);
                        send = false;
                    }
                }
                if(!send && USBUSARTIsTxTrfReady()){
                    uint8_t buffer_local[6];
                    numBytes = getsUSBUSART(buffer_local, sizeof(buffer_local));
                    if(numBytes > 0){
                        int i_buffer = atoi(buffer_local);
                        if(i_buffer > 0 && i_buffer < 13){
                            datetime.tm_mon = buffer_local;
                            current_state = ingresar_dia;
                            send = true;
                        } else {
                            send = true;
                        }
                    }
                    numBytes = 0;
                }
            }
            
            if (current_state == ingresar_dia) {
                if(send && USBUSARTIsTxTrfReady()){
                    if(UT_delayms(&delay, current_delay)){
                        char data[] = " Ingrese el dia ";
                        putsUSBUSART(data);
                        send = false;
                    }
                }
                if(!send && USBUSARTIsTxTrfReady()){
                    uint8_t buffer_local[2];
                    numBytes = getsUSBUSART(buffer_local, sizeof(buffer_local));
                    if(numBytes > 0){
                        int i_buffer = atoi(buffer_local);
                        if(i_buffer > 0 && i_buffer < 32){
                            datetime.tm_mday = buffer_local;
                            current_state = ingresar_hs;
                            send = true;
                        } else {
                            send = true;
                        }
                    }
                    numBytes = 0;
                }
            }
            
            if (current_state == ingresar_hs) {
                if(send && USBUSARTIsTxTrfReady()){
                    if(UT_delayms(&delay, current_delay)){
                        char data[] = " Ingrese la hora ";
                        putsUSBUSART(data);
                        send = false;
                    }
                }
                if(!send && USBUSARTIsTxTrfReady()){
                    uint8_t buffer_local[6];
                    numBytes = getsUSBUSART(buffer_local, sizeof(buffer_local));
                    if(numBytes > 0){
                        int i_buffer = atoi(buffer_local);
                        if(i_buffer > 0 && i_buffer < 24){
                            datetime.tm_hour = buffer_local;
                            current_state = ingresar_min;
                            send = true;
                        } else {
                            send = true;
                        }
                    }
                    numBytes = 0;
                }
            }
            
            if (current_state == ingresar_min) {
                if(send && USBUSARTIsTxTrfReady()){
                    if(UT_delayms(&delay, current_delay)){
                        char data[] = " Ingrese los minutos ";
                        putsUSBUSART(data);
                        send = false;
                    }
                }
                if(!send && USBUSARTIsTxTrfReady()){
                    uint8_t buffer_local[6];
                    numBytes = getsUSBUSART(buffer_local, sizeof(buffer_local));
                    if(numBytes > 0){
                        int i_buffer = atoi(buffer_local);
                        if(i_buffer > 0 && i_buffer < 60){
                            datetime.tm_min = buffer_local;
                            current_state = menu_Principal;
                            send = true;
                        } else {
                            send = true;
                        }
                    }
                    numBytes = 0;
                }
            }
        }
    }
    return 1; 
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