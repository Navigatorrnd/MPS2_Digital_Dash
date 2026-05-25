
#include "display_handler.h"
//U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 21, /* dc=*/ 17, /* reset=*/ 16);
using namespace esp_panel::drivers;
using namespace esp_panel::board;

uint32_t tick_cnt = 0;
Display_Handler::Display_Handler()
{    
    //u8g2 = U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI(U8G2_R0, /* cs=*/ 21, /* dc=*/ 17, /* reset=*/ 16);

};

void Display_Handler::DisplayInit(String ver)
{    
    Serial.println("Initializing board");
    Board *board = new Board();
    board->init();
#if LVGL_PORT_AVOID_TEARING_MODE
    auto lcd = board->getLCD();
 
    // When avoid tearing function is enabled, the frame buffer number should be set in the board driver
    lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
#if ESP_PANEL_DRIVERS_BUS_ENABLE_RGB && CONFIG_IDF_TARGET_ESP32S3
    auto lcd_bus = lcd->getBus();
    /**
     * As the anti-tearing feature typically consumes more PSRAM bandwidth, for the ESP32-S3, we need to utilize the
     * "bounce buffer" functionality to enhance the RGB data bandwidth.
     * This feature will consume `bounce_buffer_size * bytes_per_pixel * 2` of SRAM memory.
     */
    if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
        static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
    }
#endif
#endif
    assert(board->begin());
    Serial.println("Initializing LVGL");
    lvgl_port_init(board->getLCD(), board->getTouch());
    //lv_disp_set_bg_color(NULL, lv_color_hex(0x0000ffff));
    
    Serial.println("Creating UI");
    lvgl_port_lock(-1);
    ui_init() ;
    lvgl_port_unlock();
    Serial.println("loadScreen LOGO");
    //loadScreen(SCREEN_ID_MAIN);
    lvgl_port_lock(-1);
    loadScreen(SCREEN_ID_START_LOGO);
    lv_label_set_text(objects.version, String(ver).c_str());
    lvgl_port_unlock();

    old_speed = 0;
};

String AtState_to_str(uint8_t at_drive);
void Display_Handler::DisplayTickDiD(mps_general_params_t params_did)
{
    //Serial.print("DisplayTickDiD ");
    //Serial.println(tick_cnt);
    
    //tick_cnt++;
    lvgl_port_lock(-1);
    //lv_meter_set_indicator_value(objects.speedometr, screen_main_state.indicator, params_did.speed);
    lv_label_set_text(objects.speed, String(params_did.speed).c_str());
    lv_label_set_text(objects.rpm, String(params_did.rpm).c_str());
    lv_label_set_text(objects.torq, String(params_did.torque).c_str());

    lv_label_set_text(objects.at_state, String(AtState_to_str(params_did.at_drive)).c_str());
    lv_label_set_text(objects.at_state_req, String(AtState_to_str(params_did.at_drive_current)).c_str());
    lv_label_set_text(objects.eng_temp, String(params_did.t_engine).c_str());
    lv_label_set_text(objects.eng_temp, String(params_did.t_engine).c_str());
    lv_label_set_text(objects.atf_temp, String(params_did.t_akpp).c_str());
    lv_label_set_text(objects.voltage, String(params_did.v_ecu, 1).c_str());
    lv_label_set_text(objects.in_temp, String(params_did.t_int).c_str());
    lv_label_set_text(objects.out_temp, String(params_did.t_ext).c_str());
    lv_label_set_text(objects.freezer_temp, String(params_did.t_airflow).c_str());

    lvgl_port_unlock();
};



void Display_Handler::DisplayTickODO(mps_odom_params_t params_odo, int currTrip,  mps_etacs_params_t etacs_params)
{
    lvgl_port_lock(-1);
    lv_label_set_text(objects.odo, String(params_odo.odometer).c_str());
    lv_label_set_text(objects.trip_a, String(params_odo.trip_a).c_str());
    lv_label_set_text(objects.trip_b, String(params_odo.trip_b).c_str());
    lv_label_set_text(objects.trip_curr, String(params_odo.trip_curr).c_str());
    //lv_label_set_text(objects.trip_curr, String(tick_cnt).c_str());

    if(currTrip == 1)
    {
        lv_obj_set_style_bg_color(objects.trip_a, lv_color_hex(0xff444444), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.trip_b, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else if(currTrip == 2)
    {
        lv_obj_set_style_bg_color(objects.trip_b, lv_color_hex(0xff444444), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.trip_a, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else 
    {
        lv_obj_set_style_bg_color(objects.trip_b, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.trip_a, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);       
    }
    
    if(etacs_params.position_lamp)
    {   
        lv_led_set_brightness(objects.position_lamp, 255);
        lv_obj_set_style_bg_opa(objects.dimmer, dimmer, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else 
    {
        lv_led_set_brightness(objects.position_lamp, 2);
        lv_obj_set_style_bg_opa(objects.dimmer, dimmer, LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    if(etacs_params.head_lamp_lo)
    {
        lv_led_set_brightness(objects.head_lamp, 255);
        
    }
    else lv_led_set_brightness(objects.head_lamp, 2);
    lv_label_set_text(objects.dimmer_state, String(dimmer).c_str());
    //Serial.println(etacs_params.head_lamp_lo);
    lvgl_port_unlock();
};

void set_speedometr_value(void * indicator, int32_t v) {
    // indicator — это указатель на вашу стрелку (lv_meter_indicator_t)
    // v — текущее значение анимации
    lvgl_port_lock(-1);
    lv_meter_set_indicator_value(objects.speedometr, (lv_meter_indicator_t*)indicator, v);
    lvgl_port_unlock();
}

void Display_Handler::DisplayTickSpeed(mps_general_params_t params_did)
{
    lvgl_port_lock(-1);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, screen_main_state.indicator);        // Объект, который передастся в колбэк
    lv_anim_set_values(&a, old_speed, params_did.speed); // Откуда и до скольки
    lv_anim_set_time(&a, 200);                // Длительность в мс (напр. 500мс)
    lv_anim_set_exec_cb(&a, set_speedometr_value); // Наша функция выше
    lv_anim_set_path_cb(&a, lv_anim_path_linear); 
    lv_anim_start(&a);
    old_speed = params_did.speed;
    lvgl_port_unlock();
}  	

String AtState_to_str(uint8_t at_drive)
{   String result = "";
    if(at_drive<0xff)
    {
        switch (at_drive) 
        {
            case 0x00:
                result = "N";
                break;
            case 0x01:
                result = "1";
                break;
            case 0x02:
                result = "2";
                break;
            case 0x03:
                result = "3";
                break;
            case 0x04:
                result = "4";
                break;
            case 0x05:
                result = "5";
                break;
            case 0x0d:
                result = "P";
                break;
            case 0x0b:
                result = "R";
                break;
            default:
                result = "E";
                break;
        }
    } 
    return result;
}


void Display_Handler::setActiveTrip(int curr)
{
    if(curr == 1)
    {
        tripAisActive = true;
        tripBisActive = false;
    }
    if(curr == 2)
    {
        tripBisActive = true;
        tripAisActive = false;
    }
}

void Display_Handler::setEngtempAlarm(bool alarm)
{
    if(alarm)
    {
        lv_obj_set_style_bg_opa(objects.eng_temp, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.eng_temp, lv_color_hex(0xffb00000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else
        lv_obj_set_style_bg_color(objects.eng_temp, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT); 
}

void Display_Handler::setATFTempAlarm(bool alarm)
{
    if(alarm)
    {
        lv_obj_set_style_bg_opa(objects.atf_temp, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(objects.atf_temp, lv_color_hex(0xffb00000), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    else
        lv_obj_set_style_bg_color(objects.atf_temp, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT); 
}
