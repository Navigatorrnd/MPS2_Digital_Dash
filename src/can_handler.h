#ifndef _can_handler_h
#define _can_handler_h


#include <esp32_can.h>
#include <esp_task_wdt.h>
#include "mps_params.h"

#define CAN_PIN_RX  GPIO_NUM_5      // Rx pin for TJA1051T/3
#define CAN_PIN_TX  GPIO_NUM_4      // Tx pin for TJA1051T/3




//#define USE_MULTIK            // отключаем запросы, когда работает мультитроникс, смотрим, что отвечают ему

#define FUEL_READ_CNT 10


class Can_Handler
{
public:
    Can_Handler();
    
    void CanHandlerInit();
    void CanHandlerDeInit();
    void taskCanSend();
    void calculate();
    mps_general_params_t get_params();
    mps_autoac_params_t get_ac_params();
    mps_etacs_params_t get_etacs_params();
    int get_wheel_angle();
    bool get_bus_active(){ return is_bus_active; }
    bool get_did_active(){ return is_did_active; }
    void reset_bus_active(){ is_bus_active = false; }
    void reset_did_active(){ is_did_active = false; }

private:
    static bool req_more;
    //static int fuel_tmp_buf[100];
    static int fuel_tmp_total;
    static int fuel_tmp_last;
    static int fuel_tmp_num;
    //static int speed_tmp_buf[100];
    static int speed_tmp_total;
    static int speed_tmp_num;
    static mps_general_params_t current_params;
    static mps_etacs_params_t current_etacs_params;
    static mps_autoac_params_t current_autoac_params;
    static int wheel_angle;
    static bool is_bus_active;
    static bool is_did_active;
    /*bool req_more;
    int fuel_tmp_buf[10];
    int fuel_tmp_total;
    int fuel_tmp_last;
    int fuel_tmp_num;
    mps_params_t current_params;*/

    void set_mask_filt();
    void sendPid(unsigned long can_id, unsigned char __pid);
    static  void HandleRxEvent(CAN_FRAME* rxFrame);
    int pid_iterator;

};



#endif