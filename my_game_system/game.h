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
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>
#include <limits.h>
#include <dirent.h>

typedef struct Start_page
{
    lv_obj_t * Start_ui;
    lv_obj_t * backgound;
    lv_obj_t * Start_bottom;
    lv_obj_t * hand_lab;
    lv_obj_t * enter_lab;
}ST,*ST_P;



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
    lv_obj_t * backgound;
    lv_obj_t * end_lab;
}ED,*ED_P;


struct UI_Contral
{
    ST_P   Start_page;
    M_pg_P Main_page;
    ED_P   End_page; 

};
int game(void);
ST_P Start_page(struct UI_Contral * UC_P);
M_pg_P Main_page(struct UI_Contral * UC_P);
void Enter_Main_page(lv_event_t * e);

 #endif