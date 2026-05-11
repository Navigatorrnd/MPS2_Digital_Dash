#ifndef _mps_params_h
#define _mps_params_h

#include <Arduino.h>

#define ID_AT_REQ       0x7e1
#define ID_AT_ANS       0x7e9
#define ID_DID_REQ      0x7e0
#define ID_DID_ANS      0x7e8
#define ID_CLIM_REQ     0x7a2
#define ID_CLIM_ANS     0x7a3
#define ID_MPS_SPEED    0x215
#define ID_MPS_AT       0x218
#define ID_MPS_STEER    0x236
#define ID_MPS_RPM      0x308
#define ID_MPS_TORQ     0x312
#define ID_MPS_DID_TEMP 0x608
#define ID_MPS_TORQ     0x312

#define ID_MPS_ETACS    0x424
#define ID_MPS_AUTOAC   0x445

#define PID_CLIMAT_TEMP     0x10
#define PID_AT_INFO         0x02
#define PID_DID_INFO        0x02
#define PID_ODO_INFO        0x03
#define PID_ECU_VOLTAGE     0x01    // Control Module Voltage 

#define CAN_ID_PID          0x7DF

#define TRIP_COEFF          25700.0f

typedef struct {
    int t_engine;                 // Температура двигателя (°C).
    int t_akpp;                   // Температура коробки передач (°C, специфично для NMPS/Pajero4).
    int t_ext;                    // Внешняя температура (°C).
    int t_int;                    // Внешняя температура (°C).
    int rpm;                      // Обороты двигателя (об/м).
    int torque;
    int speed;  
    double avrg_speed;     
    int p_intake;                 // Давление во впускном коллекторе (кПа).
    int p_fuel;                   // Давление топлива в рейке (МПа).
    float v_ecu;                  // Напряжение бортовой сети (Вольт).
    int t_engine2;                // Температура двигателя (°C) от блока климата.
    int t_airflow;                // Температура воздуха в печке (°C).
    uint8_t at_drive;
    uint8_t at_drive_current;
    uint32_t raw_fuel;
    float fuel_in_H;
    float fuel_in_100;
    float total_fuel;
    uint32_t odometer;
    double trip;
    double trip_over_cnt;
    uint32_t trip_cnt_incr;
} mps_general_params_t;

typedef struct {
    bool head_lamp_lo;
    bool left_turn;
    bool right_turn;
    bool position_lamp;
    bool head_lamp_hi;
    bool fog_lamp_front;
    bool fog_lamp_rear;
    bool driver_door;
    bool other_door; 
} mps_etacs_params_t;

typedef struct {
    uint8_t setted_temp;
    uint8_t fan_speed;
    bool flow_wind;
    bool flow_down;
    bool flow_up;
    bool outside_air;
    bool inside_air;
    bool ac_on;
    bool mirror_heat;
} mps_autoac_params_t;

typedef struct {
    uint32_t odometer;
    double trip_a;
    double trip_b;
    double trip_curr;
} mps_odom_params_t;
#endif