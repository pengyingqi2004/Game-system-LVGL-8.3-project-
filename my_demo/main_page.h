#ifndef _MAIN_PAGE_H_ 
#define _MAIN_PAGE_H_
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
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>
#include <limits.h>
#include <dirent.h>
#define  DEFAULT_SHOW_DIR "/"

int main_page(void);
typedef struct Statr_UI
{
    lv_obj_t * start_ui;
    lv_obj_t * background;
    lv_obj_t * tittle_lab;
    lv_obj_t * enter_btn;
    lv_obj_t * enter_lab;
}Start_UI,*Start_UI_P;

typedef struct Dir_UI
{
    lv_obj_t * dir_ui;
    lv_obj_t * dir_list;
    lv_obj_t * file_ui;
    lv_obj_t * exit_btn;
    lv_obj_t * picture;
    lv_obj_t * exit_lab;
    lv_obj_t * img_ui;
}Dir_UI,*Dir_UI_P;

typedef struct END_UI
{
    lv_obj_t * end_ui;
    lv_obj_t * end_lab;
    lv_obj_t * end_background;
    
}END_UI,*END_UI_P;

typedef struct  dir_bin_inf
{
    char dir_name[512];
    struct All_UI * UC_P;
    struct dir_bin_inf *next;
    struct dir_bin_inf *prev;

}DBI,*DBI_P;

struct All_UI
{
    Start_UI_P start_ui_p;
    Dir_UI_P   dir_ui_p;
    END_UI_P   end_ui_p;
    DBI_P      dir_bin_inf_head;
};


int main_page(void);
Start_UI_P Show_Start_UI(struct All_UI * UC_P);
Dir_UI_P   Show_Dir_UI(struct All_UI * UC_P);
END_UI_P   Show_End_UI(struct All_UI * UC_P);
void Enter_Btn(lv_event_t *e);
void Enter_Exit(lv_event_t *e);
void file_click_cb(lv_event_t * e);
void back_to_start_cb(lv_timer_t * timer);
bool Show_Dir_List(const char * obj_dir_path,struct All_UI * UC_P);
void Dir_Btn_Task(lv_event_t * e);
bool  Destory_Dir_Btn_List(DBI_P head_node);
DBI_P Create_Node(void);
bool  Head_Add_Node(DBI_P head_node,DBI_P new_node);
// Dir_UI_P   Show_Dir_UI(void);
// void Enter_Btn(lv_event_t *e);
void File_Btn_Task(lv_event_t *e);
void Close_Img_Task(lv_event_t * e);
void File_Btn_gif_Task(lv_event_t *e);

#endif