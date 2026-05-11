#ifndef _display_handler_h
#define _display_handler_h

#include <Arduino.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include "lvgl_v8_port.h"

#include "./ui/ui.h"

#include "mps_params.h"

class Display_Handler
{
    
public:
    Display_Handler();
    void DisplayInit(String ve);
    void DisplayTickDiD(mps_general_params_t params_did );
    void DisplayTickODO(mps_odom_params_t params_odo, int currTrip, mps_etacs_params_t etacs_params, int dimmer);
    void DisplayTickSpeed(mps_general_params_t params_did);
    void setActiveTrip(int curr);
private:
    //U8G2 u8g2;//(&u8g2_cb_r0, /* cs=*/ 21, /* dc=*/ 17, /* reset=*/ 16);

    uint8_t active_check_cnc;
    int old_speed;
    bool tripAisActive;
    bool tripBisActive;
};

#endif