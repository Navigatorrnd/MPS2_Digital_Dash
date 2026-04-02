
#include "display_handler.h"
//U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 21, /* dc=*/ 17, /* reset=*/ 16);
using namespace esp_panel::drivers;
using namespace esp_panel::board;


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
};



  					