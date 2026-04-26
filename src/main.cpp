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
#include "EncButton.h"
#include "can_handler.h"
#include "display_handler.h"

// #include "nvs_flash.h"
// #include "nvs.h"

#include <Preferences.h>

#define BEEPER_PIN 11
#define BUTTON_PIN 6
#define IGN_PIN 9
#define POWERKEY_PIN 10
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
GTimer displayOdoTimer(MS);
GTimer canboxTimer(MS);
GTimer hudTimer(MS);
GTimer busActiveCheckTimer(MS);
GTimer ignCheckTimer(MS);
GTimer poweroffTimer(MS);
GTimer saveTimer(MS);
GTimer resetActiveTripTimer(MS);

GTimer twaiCheckTimer(MS);   

Can_Handler can_handler;
Display_Handler display;  
Button btn(BUTTON_PIN);
int active_trip;
Preferences saved_params;

void setup()
{
    //delay(2000);
    
    Serial.begin(115200);
    
    Serial.println("Start");
    //delay(2000);

    display.DisplayInit(VERSION);
    delay(5000);
    can_handler = Can_Handler();    
    Serial.println("setup Can_Handler");
    can_handler.CanHandlerInit();  
    Serial.println("setup CanHandlerInit");  
    reqTimer.setInterval(200);
    calculateTimer.setInterval(1000);
    displayTimer.setInterval(200);  
    displayOdoTimer.setInterval(500); 
    ignCheckTimer.setTimeout(15000);


    Serial.println("loadScreen MAIN");
    lvgl_port_lock(-1);
    loadScreen(SCREEN_ID_MAIN);
    lvgl_port_unlock();

    Serial.println("Setup complete");
    timing = millis();
    twaiCheckTimer.setTimeout(10000);

    btn.setBtnLevel(LOW);
    btn.setClickTimeout(300);
    btn.setDebTimeout(50);
    btn.setHoldTimeout(500);
    btn.setStepTimeout(200);
    btn.setTimeout(1000);

    active_trip = 0;
    pinMode(IGN_PIN, INPUT_PULLDOWN);
    pinMode(POWERKEY_PIN, OUTPUT);
    digitalWrite(POWERKEY_PIN, 1);

    saved_params.begin("params", false);
    can_handler.set_tripA(saved_params.getDouble("tripA", 0) );
    can_handler.set_tripB(saved_params.getDouble("tripB", 0) );
  
    
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
    btn.tick();
    if (btn.click())
    {
        Serial.println("click");
        if(active_trip == 1)active_trip = 2;
        else active_trip = 1;
        //display.setActiveTrip(current_trip);
        Serial.print("Active trip ");
        Serial.println(active_trip);
        resetActiveTripTimer.setTimeout(20000);
    }
    
    if (btn.hold()) 
    {
        Serial.println("hold");   
        if(active_trip == 1) can_handler.reset_tripA();
        if(active_trip == 2) can_handler.reset_tripB();
    }
    
    if(reqTimer.isReady())
    {        
        //can_handler.taskCanSend();
        
    }      
    if(calculateTimer.isReady())
    {
        can_handler.calculate();
        //taskPrintParams();
    }     
    if(displayTimer.isReady())
    {
        display.DisplayTickDiD(can_handler.get_params());
        display.DisplayTickSpeed(can_handler.get_params());
        //display.u8g2display(can_handler.get_params(), can_handler.get_wheel_angle()); 
    } 
    if(displayOdoTimer.isReady())
    {
        display.DisplayTickODO(can_handler.get_odom_params(), active_trip);   
    }

    if(can_handler.get_bus_active())
    {
        twaiCheckTimer.setTimeout(1000);  
        can_handler.reset_bus_active();
    }
    if(twaiCheckTimer.isReady())
    {
        Serial.println("twaiCheckTimer; restart"); 
        reqTimer.stop();
        can_handler.CanHandlerTwaiRestart();
        
    }
    if(ignCheckTimer.isReady())
    {
        if(digitalRead(IGN_PIN))
        {
            ignCheckTimer.setTimeout(5000);  
            //Serial.println("IGN active");
        }
        else
        {
            saveTimer.setTimeout(5000);
            Serial.println("saveTimer activate");
            //saving trips

        }
    }
    if(saveTimer.isReady())
    {
        if(!digitalRead(IGN_PIN))
        {
            Serial.println("IGN off");
            mps_odom_params_t params_odo = can_handler.get_odom_params();
            saved_params.putDouble("tripA", params_odo.trip_a); 
            saved_params.putDouble("tripB", params_odo.trip_b); 
            saved_params.end();
            Serial.println("params saved");
            poweroffTimer.setTimeout(60000);
            Serial.println("poweroffTimer activate");
        }
        else
        {
            ignCheckTimer.setTimeout(5000);  
        }    
    }
    if(poweroffTimer.isReady())
    {
        if(!digitalRead(IGN_PIN))
        {
            Serial.println("poweroff");
            digitalWrite(POWERKEY_PIN, 0);
        }
        else
        {
            ignCheckTimer.setTimeout(5000);  
            digitalWrite(POWERKEY_PIN, 1);
        }    
    }

    if(resetActiveTripTimer.isReady())
    {
        active_trip = 0;
        Serial.println("resetActiveTripTimer");
    }
}
