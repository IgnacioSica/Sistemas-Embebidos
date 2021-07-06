
#ifndef MENU_CONTROLLER_H 
#define MENU_CONTROLLER_H

#include "FreeRTOS.h"
#include "task.h"
#include "../mcc_generated_files/system.h"
#include "../mcc_generated_files/pin_manager.h"
#include "../framework/LEDs_RGB/WS2812.h"
#include "../mcc_generated_files/usb/usb_device.h"
#include "../mcc_generated_files/usb/usb_device_cdc.h"
#include "../mcc_generated_files/rtcc.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "LedsController.h"

#ifdef __cplusplus

#endif

typedef enum{
    send,
    receive,
    menu_principal,
    menu_principal_option,
    umbral_warning,
    umbral_danger
} menu_state;

typedef struct{
    menu_state newState;
    char *message;
    char *buffer;
} usb_params;

void menu( void *p_param );
void controllerUSB( void *p_param );
void goToMenu(void *p_param);

#ifdef __cplusplus

#endif

#endif 
