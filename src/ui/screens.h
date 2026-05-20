#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_START_LOGO = 1,
    SCREEN_ID_MAIN = 2,
    _SCREEN_ID_LAST = 2
};

typedef struct _objects_t {
    lv_obj_t *start_logo;
    lv_obj_t *main;
    lv_obj_t *logo;
    lv_obj_t *version;
    lv_obj_t *speedometr;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *trip_curr;
    lv_obj_t *trip_a;
    lv_obj_t *trip_b;
    lv_obj_t *odo;
    lv_obj_t *obj2;
    lv_obj_t *trip_curr_1;
    lv_obj_t *trip_a_1;
    lv_obj_t *trip_b_1;
    lv_obj_t *odo_1;
    lv_obj_t *obj3;
    lv_obj_t *speed;
    lv_obj_t *at_state;
    lv_obj_t *at_state_req;
    lv_obj_t *obj4;
    lv_obj_t *obj5;
    lv_obj_t *eng_temp;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *voltage;
    lv_obj_t *obj8;
    lv_obj_t *obj9;
    lv_obj_t *obj10;
    lv_obj_t *out_temp;
    lv_obj_t *obj11;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *atf_temp;
    lv_obj_t *obj14;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *in_temp;
    lv_obj_t *obj17;
    lv_obj_t *torq;
    lv_obj_t *obj18;
    lv_obj_t *obj19;
    lv_obj_t *obj20;
    lv_obj_t *freezer_temp;
    lv_obj_t *obj21;
    lv_obj_t *rpm;
    lv_obj_t *obj22;
    lv_obj_t *position_lamp;
    lv_obj_t *head_lamp;
    lv_obj_t *dimmer_state;
    lv_obj_t *dimmer;
} objects_t;

extern objects_t objects;

typedef struct {
    lv_meter_scale_t *scale;
    lv_meter_indicator_t *indicator;
} screen_main_state_t;

extern screen_main_state_t screen_main_state;

void create_screen_start_logo();
void tick_screen_start_logo();

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/