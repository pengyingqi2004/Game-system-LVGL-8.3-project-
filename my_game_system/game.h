#ifndef _GAME_H_ 
#define _GAME_H_
#include <stdio.h>
#include "../lvgl/lvgl.h"
#include "../lvgl/demos/lv_demos.h"
#include "../lv_drivers/display/fbdev.h"
#include "../lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>
#include <limits.h>
#include <dirent.h>

#include "2048.h"
#include "memory_game.h"

#include "../lv_lib_100ask-release-v8.x/lv_lib_100ask.h"
#include "../lv_lib_100ask_conf.h"
#define game_btn_x 80
#define game_btn_y 80
#define MATRIX_SIZE     LV_100ASK_2048_MATRIX_SIZE

#define MY_CLASS_2048    &lv_100ask_2048_class

#define LV_100ASK_2048_BG_COLOR             lv_color_hex(0xb3a397)
#define LV_100ASK_2048_TEXT_BLACK_COLOR     lv_color_hex(0x6c635b)
#define LV_100ASK_2048_TEXT_WHITE_COLOR     lv_color_hex(0xf8f5f0)

#define LV_100ASK_2048_NUMBER_EMPTY_COLOR   lv_color_hex(0xc7b9ac)
#define LV_100ASK_2048_NUMBER_2_COLOR 		lv_color_hex(0xeee4da)
#define LV_100ASK_2048_NUMBER_4_COLOR 		lv_color_hex(0xede0c8)
#define LV_100ASK_2048_NUMBER_8_COLOR 		lv_color_hex(0xf2b179)
#define LV_100ASK_2048_NUMBER_16_COLOR 		lv_color_hex(0xf59563)
#define LV_100ASK_2048_NUMBER_32_COLOR 		lv_color_hex(0xf67c5f)
#define LV_100ASK_2048_NUMBER_64_COLOR 		lv_color_hex(0xf75f3b)
#define LV_100ASK_2048_NUMBER_128_COLOR 	lv_color_hex(0xedcf72)
#define LV_100ASK_2048_NUMBER_256_COLOR 	lv_color_hex(0xedcc61)
#define LV_100ASK_2048_NUMBER_512_COLOR 	lv_color_hex(0xedc850)
#define LV_100ASK_2048_NUMBER_1024_COLOR 	lv_color_hex(0xedc53f)
#define LV_100ASK_2048_NUMBER_2048_COLOR 	lv_color_hex(0xedc22e)
#define MY_CLASS_memory &lv_100ask_memory_game_class

static void lv_100ask_2048_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_100ask_2048_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_100ask_2048_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void btnm_event_cb(lv_event_t * e);

static void init_matrix_num(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
static void addRandom(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
static void update_btnm_map(char * lv_100ask_2048_btnm_map[], uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
static lv_color_t get_num_color(uint16_t num);
static char * int_to_str(char * str, uint16_t num);

static uint8_t count_empty(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
static uint8_t find_target(uint16_t array[MATRIX_SIZE], uint8_t x, uint8_t stop);
static void rotate_matrix(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
static bool find_pair_down(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
static bool slide_array(uint16_t * score, uint16_t array[MATRIX_SIZE]);
static bool move_up(uint16_t * score, uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
static bool move_down(uint16_t * score, uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
static bool move_left(uint16_t * score, uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
static bool move_right(uint16_t * score, uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
static bool game_over(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]);
static lv_obj_t * level_label;
static void lv_100ask_memory_game_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_100ask_memory_game_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_100ask_memory_game_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void list_rand_number(uint16_t arry[], uint16_t max_count, uint16_t count);
static void item_event_handler(lv_event_t * e);


//结构体定义区
typedef struct Game_Mode
{
    lv_obj_t * Game;
    lv_obj_t * ranking_list;
    lv_obj_t * User_inf;
}GM,*GM_P;

typedef struct Start_page
{
    lv_obj_t * Start_ui;
    lv_obj_t * backgound;
    lv_obj_t * Start_bottom;
    lv_obj_t * hand_lab;
    lv_obj_t * enter_lab;
}ST,*ST_P;

typedef struct User_loading_page
{
    lv_obj_t * User_ui;
    lv_obj_t * User_name;
    lv_obj_t * User_password;
    lv_obj_t * User_create;
}LD,*LD_P;

typedef struct User_create_page
{
    lv_obj_t * User_create_ui;
    lv_obj_t * User_create_name;
    lv_obj_t * User_create_password;
    lv_obj_t * return_btn;
}UM,*UM_P;
typedef struct Main_page
{
    lv_obj_t * main_ui;
    lv_obj_t * backgound;
    lv_obj_t * main_list;
    lv_obj_t * main_game;
    lv_obj_t * User_inf;
    lv_obj_t * exit_lab;
    lv_obj_t * exit_bottom;

}M_pg,*M_pg_P;

typedef struct End_page
{
    lv_obj_t * end_ui;
    lv_obj_t * backgound;
    lv_obj_t * end_lab;
}ED,*ED_P;

typedef struct Game_page
{
    lv_obj_t * Game_ui;
    lv_obj_t * backgound;
    lv_obj_t * back_btn;
    lv_obj_t * back_lab;
}GP,*GP_P;
struct UI_Contral
{
    ST_P   Start_page;
    LD_P   loading_page;
    UM_P   create_page;
    M_pg_P Main_page;
    ED_P   End_page; 
    GP_P   Game_page;

};


const lv_obj_class_t lv_100ask_2048_class = {
    .constructor_cb = lv_100ask_2048_constructor,
    .destructor_cb  = lv_100ask_2048_destructor,
    .event_cb       = lv_100ask_2048_event,
    .width_def      = LV_DPI_DEF * 2,
    .height_def     = LV_DPI_DEF * 2,
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .instance_size  = sizeof(lv_100ask_2048_t),
    .base_class     = &lv_obj_class
};
const lv_obj_class_t lv_100ask_memory_game_class = {
    .constructor_cb = lv_100ask_memory_game_constructor,
    .destructor_cb  = lv_100ask_memory_game_destructor,
    .event_cb       = lv_100ask_memory_game_event,
    .width_def      = LV_DPI_DEF * 2,
    .height_def     = LV_DPI_DEF * 2,
    .instance_size  = sizeof(lv_100ask_memory_game_t),
    .base_class     = &lv_obj_class
};

static lv_style_t cont_style;
static lv_style_t item_def_style;
static lv_style_t item_click_style;
static lv_style_t item_hit_style;
//函数定义区（依据使用顺序）
int game(void);
ST_P Start_page(struct UI_Contral * UC_P);
void Enter_Main_page(lv_event_t * e);
M_pg_P Main_page(struct UI_Contral * UC_P);
void Exit_page(lv_event_t * e);
ED_P End_page(struct UI_Contral * UC_P);
void back_to_start_cb(lv_timer_t * timer);
bool Mode_list(struct UI_Contral * UC_P);
bool Game_Show(struct UI_Contral * UC_P);
void Enter_Game_page(struct UI_Contral * UC_P);
GP_P Game_page(struct UI_Contral * UC_P);
void Return_page(lv_event_t * e);
static void Game_Show_cb(lv_event_t * e);
static void game_2048_btn_event_handler(lv_event_t * e);
static void start_2048_cb(lv_timer_t * timer);
void lv_100ask_2048_simple_test(void);
static void game_memory_btn_event_handler(lv_event_t * e);
static void start_memory_cb(lv_timer_t * timer);
void lv_100ask_memory_game_simple_test(void);

 #endif
