#ifndef LV_MEMORY_GAME_H
#define LV_MEMORY_GAME_H
#ifdef __cplusplus
extern "C" {
#endif
#include "../lv_lib_100ask-release-v8.x/lv_lib_100ask.h"

typedef struct {
    lv_obj_t obj;
    lv_obj_t * bef;
    uint16_t row;
    uint16_t col;
} lv_100ask_memory_game_t;

lv_obj_t * lv_100ask_memory_game_create(lv_obj_t * parent);

void lv_100ask_memory_game_set_map(lv_obj_t * obj, uint16_t row, uint16_t col);

uint16_t lv_100ask_memory_game_get_row(lv_obj_t * obj);

uint16_t lv_100ask_memory_game_get_col(lv_obj_t * obj);

#endif  /*LV_100ASK_MEMORY_GAME*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

