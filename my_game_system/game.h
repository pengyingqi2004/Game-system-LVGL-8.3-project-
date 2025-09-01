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
#include <errno.h>
#include "2048.h"
#include "memory_game.h"
#include "../lvgl/src/misc/lv_anim.h"
#include "../lv_lib_100ask-release-v8.x/lv_lib_100ask.h"
#include "../lv_lib_100ask_conf.h"

extern int local_time[3];
void* Time_show(void *arg);
void* game(void *arg);
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
    // ★新增：开始页轮播所需
    lv_obj_t   *bg_img;                // 用于显示轮播图片（替代 gif）
    lv_timer_t *slide_timer;           // 轮播定时器
    char        img_dir[128];          // 图片目录（由主界面设置）
    char       *img_paths[64];         // 最多64张图片路径（S:/xxx/xx.png）
    uint16_t    img_count;             // 实际图片数量
    uint16_t    img_index;             // 当前索引
    uint16_t    slide_ms;              // 轮播间隔(ms)，例如 2000
    lv_obj_t *  time_ui;
    lv_obj_t *  time_lab;
    lv_timer_t *time_timer;

}ST,*ST_P;

typedef struct Using_page
{

    char txt[8];
    lv_obj_t * loading_btn;
    lv_obj_t * loading_lab;
    lv_obj_t * floot_list;
    lv_obj_t * floot_show_ui;
    lv_obj_t * User_ui;
    lv_obj_t * floot_ui;
    lv_obj_t *label_floor;   // 显示当前楼层
    lv_obj_t *label_door;    // 显示开门/关门状态
    lv_obj_t *gif_up;
    lv_obj_t *gif_down;
    int current_floor;
    int target_floor;
    bool busy;
    lv_obj_t *gif_door_open;
    lv_obj_t *gif_door_close;
    lv_obj_t * exit_lab;
    lv_obj_t * exit_btn;
    lv_obj_t * game_lab;
    lv_obj_t * game_btn;
    lv_obj_t * emergency_btn;
    lv_obj_t * emergency_lab;
    lv_obj_t *emergency_overlay;
    // lv_obj_t * label_dir

}FL,*FL_P;

typedef struct Floot 
{
    int floot;
    struct Floot *next;
    struct Floot *prev;
} FT, *FT_P;

typedef struct User_loading_page
{
    lv_obj_t *User_ui;     // 整个登录screen
    lv_obj_t *backgound;   // 背景图
    lv_obj_t *card;        // 中间卡片容器
    lv_obj_t *ta_user;     // 用户名输入
    lv_obj_t *ta_pass;     // 密码输入
    lv_obj_t *btn_login;   // 登录按钮
    lv_obj_t *btn_reg;     // 注册按钮
    lv_obj_t *btn_back;    // 返回按钮
    lv_obj_t *kb;          // 软键盘


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
    lv_obj_t * file_ui;
    lv_obj_t * img_ui;

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
    lv_obj_t * menu_wrap;   // ★新增：游戏大厅卡片容器
    bool       in_game;
}GP,*GP_P;


typedef struct  dir_bin_inf
{
    char dir_name[512];
    struct UI_Contral * UC_P;
    struct dir_bin_inf *next;
    struct dir_bin_inf *prev;

}DBI,*DBI_P;

typedef struct emergency
{
    lv_obj_t * emergency_ui;
    lv_obj_t * emergency_btn;
    lv_obj_t * emergency_lab;
    lv_obj_t * emergency_img;
    lv_obj_t * return_btn;
    lv_obj_t * return_lab;
}EM,*EM_P;

struct UI_Contral
{
    ST_P   Start_page;
    LD_P   loading_page;
    M_pg_P Main_page;
    ED_P   End_page; 
    GP_P   Game_page;
    FL_P   Using_page;
    DBI_P  dir_bin_inf_head;
    EM_P   emergency_page;
    char   cur_dir[512];
};

/* 原来是：const lv_obj_class_t lv_100ask_2048_class = { ... }; */
extern const lv_obj_class_t lv_100ask_2048_class;

/* 原来是：const lv_obj_class_t lv_100ask_memory_game_class = { ... }; */
extern const lv_obj_class_t lv_100ask_memory_game_class;



static lv_style_t cont_style;
static lv_style_t item_def_style;
static lv_style_t item_click_style;
static lv_style_t item_hit_style;
static lv_style_t style_floor_item, style_floor_item_pr;

//函数定义区（依据使用顺序）
ST_P Start_page(struct UI_Contral * UC_P);
void Enter_Main_page(lv_event_t * e);
M_pg_P Main_page(struct UI_Contral * UC_P);
void Exit_page(lv_event_t * e);
ED_P End_page(struct UI_Contral * UC_P);
void back_to_start_cb(lv_timer_t * timer);
bool DCM_list(struct UI_Contral * UC_P,const char * obj_dir_path);
bool Game_Show(struct UI_Contral * UC_P);
void Enter_Game_page(lv_event_t * e);
GP_P Game_page(struct UI_Contral * UC_P);
void Return_page(lv_event_t * e);
static void Game_Show_cb(lv_event_t * e);
static void game_2048_btn_event_handler(lv_event_t * e);
static void start_2048_cb(lv_timer_t * timer);
void lv_100ask_2048_simple_test(void);
static void game_memory_btn_event_handler(lv_event_t * e);
static void start_memory_cb(lv_timer_t * timer);
void lv_100ask_memory_game_simple_test(void);
static void login_btn_cb(lv_event_t *e);
static void reg_btn_cb(lv_event_t *e);
static void back_btn_cb(lv_event_t *e);
LD_P loading_page(struct UI_Contral * UC_P);
static void _anim_ready_cb(lv_anim_t *a);
static void _opa_exec_cb(void *obj, int32_t v);
static void ta_event_cb(lv_event_t * e);
void Enter_User_page(lv_event_t * e);
FL_P USING_page(struct UI_Contral * UC_P);
static void floor_btn_event_cb(lv_event_t *e);
FT_P Create_Floot(int i,FT_P first_floot);
static void door_open_done_cb(lv_timer_t * t);
static void arrive_cb(lv_timer_t * t);
static void door_close_done_cb(lv_timer_t * t);
void Enter_USING_page(lv_event_t * e);
// ★新增：设置开始页图片目录（主界面选择好文件夹后调用一次）
void Start_page_set_folder(struct UI_Contral *UC_P, const char *dir);
// 电梯楼层面板用到的样式与回调
static bool is_img_file(const char *name);
static uint16_t scan_img_dir(struct Start_page *sp);
static void slide_timer_cb(lv_timer_t *t);
void Dir_Btn_Task(lv_event_t *e );
void File_Btn_Task(lv_event_t *e);
void Close_Img_Task(lv_event_t * e);
void File_Btn_gif_Task(lv_event_t *e);
DBI_P Create_Node(void);
bool  Head_Add_Node(DBI_P head_node,DBI_P new_node);
bool  Destory_Dir_Btn_List(DBI_P head_node);
static bool has_img_ext_ci(const char *name);
static void emergency_confirm_cb(lv_event_t *e);
static void emergency_cancel_cb(lv_event_t *e);
static void emergency_big_btn_cb(lv_event_t *e);
static void emergency_return_to_using_cb(lv_event_t *e);
EM_P EMERGENCY_page(struct UI_Contral * UC_P);
void emergency_enter(lv_event_t * e);
void Enter_End_page(struct UI_Contral * UC_P);


 #endif
