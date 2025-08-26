#include"game.h"
int game(void)
{
    static struct UI_Contral UC={NULL,NULL,NULL};
    Start_page(&UC);

    return 0;
}

ST_P Start_page(struct UI_Contral * UC_P)
{
    UC_P->Start_page = malloc(sizeof(struct Start_page));
    if(UC_P->Start_page==NULL)
    {
        perror("malloc start_page ");
    }
    memset(UC_P->Start_page, 0, sizeof(struct Start_page));

    UC_P->Start_page->Start_ui=lv_obj_create(NULL);
    UC_P->Start_page->backgound=lv_gif_create(UC_P->Start_page->Start_ui);
    lv_obj_center(UC_P->Start_page->backgound);
    lv_gif_set_src(UC_P->Start_page->backgound,"S:/2.gif");
    UC_P->Start_page->Start_bottom=lv_btn_create(UC_P->Start_page->Start_ui);
    lv_obj_set_size(UC_P->Start_page->Start_bottom,150,40);
    lv_obj_center(UC_P->Start_page->Start_bottom);


    lv_obj_add_event_cb(UC_P->Start_page->Start_bottom,Enter_Main_page,LV_EVENT_SHORT_CLICKED,UC_P);

    UC_P->Start_page->enter_lab=lv_label_create(UC_P->Start_page->Start_ui);
    UC_P->Start_page->hand_lab=lv_label_create(UC_P->Start_page->Start_ui);
    lv_obj_set_width(UC_P->Start_page->hand_lab,200);
    lv_label_set_long_mode(UC_P->Start_page->hand_lab,LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_recolor(UC_P->Start_page->hand_lab,1);
    lv_label_set_recolor(UC_P->Start_page->enter_lab,1);
    lv_label_set_text(UC_P->Start_page->hand_lab,"#ff375c Welcome to use game system#");
    lv_label_set_text(UC_P->Start_page->enter_lab,"#ff375c Enter ##88ff00 to ##00d5ff play#");
    lv_obj_set_pos(UC_P->Start_page->hand_lab,300,10);
    lv_obj_set_pos(UC_P->Start_page->enter_lab,350,232);
    lv_scr_load_anim(UC_P->Start_page->Start_ui,
                LV_SCR_LOAD_ANIM_FADE_ON,
                500, 0, true);
   return UC_P->Start_page;
}

void Enter_Main_page(lv_event_t * e)
{
    struct UI_Contral * UC_P=(struct UI_Contral *)e->user_data;
    UC_P->Main_page=Main_page(UC_P);
    lv_scr_load_anim(UC_P->Main_page->main_ui,LV_SCR_LOAD_ANIM_FADE_ON,500,0,true);
    free(UC_P->Start_page);
    UC_P->Start_page=NULL;
    return ;
}

M_pg_P Main_page(struct UI_Contral * UC_P)
{
    UC_P->Main_page = malloc(sizeof(struct Main_page));
    if(UC_P->Main_page == NULL) {
        perror("malloc main_page ");
        return NULL;
    }
    memset(UC_P->Main_page, 0, sizeof(struct Main_page));
    UC_P->Main_page->main_ui=lv_obj_create(NULL);
    UC_P->Main_page->backgound=lv_img_create(UC_P->Main_page->main_ui);
    lv_img_set_src(UC_P->Main_page->backgound,"S:/1.jpg");
    lv_obj_center(UC_P->Main_page->backgound);
    
    //游戏列表与游戏显示
    UC_P->Main_page->main_list=lv_list_create(UC_P->Main_page->main_ui);
    UC_P->Main_page->main_game=lv_obj_create(UC_P->Main_page->main_ui);
    //-----------------0

    UC_P->Main_page->exit_bottom=lv_btn_create(UC_P->Main_page->main_ui);
    lv_obj_set_size(UC_P->Main_page->exit_bottom,70,30);
    lv_obj_set_pos(UC_P->Main_page->exit_bottom,720,10);
    UC_P->Main_page->exit_lab=lv_label_create(UC_P->Main_page->exit_bottom);
    lv_label_set_recolor(UC_P->Main_page->exit_lab,1);
    lv_obj_center(UC_P->Main_page->exit_lab);
    lv_label_set_text(UC_P->Main_page->exit_lab,"#ff0000 E##fffb00 X##00ecf0 I##d626e7 T");

    return UC_P->Main_page;
}
