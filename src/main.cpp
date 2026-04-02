/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#define VERSION "0.0.1a" 
#include <Arduino.h>
//#include "esp_panel_board_custom_conf.h"
//#include "esp_panel_drivers_conf.h"


#include "GyverTimer.h"
#include "can_handler.h"
#include "display_handler.h"


/**
/* To use the built-in examples and demos of LVGL uncomment the includes below respectively.
 * You also need to copy `lvgl/examples` to `lvgl/src/examples`. Similarly for the demos `lvgl/demos` to `lvgl/src/demos`.
 */
// #include <demos/lv_demos.h>
// #include <examples/lv_examples.h>


int timing;
int speed = 0;
int torq = 330;
int rpm = 700;
bool speed_up = true;

GTimer reqTimer(MS);  
GTimer calculateTimer(MS);
GTimer displayTimer(MS);
GTimer canboxTimer(MS);
GTimer hudTimer(MS);
GTimer busActiveCheckTimer(MS);  

Can_Handler can_handler;
Display_Handler display;  

void setup()
{
    //delay(2000);
    
    Serial.begin(115200);
    
    Serial.println("Start");
    //delay(2000);


    can_handler = Can_Handler();    
    Serial.println("setup Can_Handler");
    can_handler.CanHandlerInit();  
    Serial.println("setup CanHandlerInit");  
    reqTimer.setInterval(200);
    calculateTimer.setInterval(1000);
    displayTimer.setInterval(200);  

    display.DisplayInit(VERSION);
    //delay(5000);
    //Serial.println("loadScreen MAIN");
    //lvgl_port_lock(-1);
    //loadScreen(SCREEN_ID_MAIN);
    //lvgl_port_unlock();

    Serial.println("Setup complete");
    timing = millis();

}

void loop()
{
    //return;
    //Serial.println("IDLE loop");
    //ui_tick();
#ifdef DEMO
    if(millis() - timing > 100)
    {
        lvgl_port_lock(-1);
        lv_meter_set_indicator_value(objects.speedometr, screen_main_state.indicator, speed);
        lv_label_set_text(objects.speed, String(speed).c_str());
        lv_label_set_text(objects.rpm, String(rpm).c_str());
        lv_label_set_text(objects.torq, String(torq).c_str());
        lvgl_port_unlock();
        if(speed_up)
        {
            speed++;
            if(speed>150) 
            {
                speed_up=false;
                torq = -5;
                Serial.println("speed_up==false;");
            }
            rpm +=26;
            if(rpm > 2500) rpm = 1200;
        }
        else{
            speed--;
            if(speed<0){
                speed_up=true;
                speed++;
                torq = 305;
                Serial.println("speed_up==true;");
            } 
            rpm -= 26;
            if(rpm < 1200) rpm = 2500;
        } 
        timing = millis();
    }
#endif
    if(reqTimer.isReady())
    {        
        can_handler.taskCanSend();
        
    }      
    if(calculateTimer.isReady())
    {
        can_handler.calculate();
        //taskPrintParams();
    }     
    if(displayTimer.isReady())
    {
     
        //display.u8g2display(can_handler.get_params(), can_handler.get_wheel_angle()); 
    } 
}
