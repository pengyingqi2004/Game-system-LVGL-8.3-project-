#include"game.h"

//函数开始调用
int game(void)
{
    static struct UI_Contral UC={NULL,NULL,NULL,NULL};
    Start_page(&UC);

    return 0;
}
//——————————————————————————————————————————————————————————————————————————————————————

//开始界面设置
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
//——————————————————————————————————————————————————————————————————————————————————————

//进入主界面按钮回调
void Enter_Main_page(lv_event_t * e)
{
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;
    UC_P->Main_page=Main_page(UC_P);
    lv_scr_load_anim(UC_P->Main_page->main_ui,LV_SCR_LOAD_ANIM_FADE_ON,500,0,true);
    free(UC_P->Start_page);
    UC_P->Start_page=NULL;
    return ;
}
//——————————————————————————————————————————————————————————————————————————————————————

//主界面基础创建
M_pg_P Main_page(struct UI_Contral * UC_P)
{
    //创建堆空间
    UC_P->Main_page = malloc(sizeof(struct Main_page));
    if(UC_P->Main_page == NULL) {
        perror("malloc main_page ");
        return NULL;
    }
    memset(UC_P->Main_page, 0, sizeof(struct Main_page));

    //创建主界面与背景
    UC_P->Main_page->main_ui=lv_obj_create(NULL);
    UC_P->Main_page->backgound=lv_img_create(UC_P->Main_page->main_ui);
    lv_img_set_src(UC_P->Main_page->backgound,"S:/1.jpg");
    lv_obj_center(UC_P->Main_page->backgound);
    
    //功能列表与游戏显示
    UC_P->Main_page->main_list=lv_list_create(UC_P->Main_page->main_ui);
    lv_obj_set_size(UC_P->Main_page->main_list,200,450);
    lv_obj_set_pos(UC_P->Main_page->main_list,10,10);
    UC_P->Main_page->main_game=lv_obj_create(UC_P->Main_page->main_ui);
    lv_obj_set_size(UC_P->Main_page->main_game,570,450);
    lv_obj_set_pos(UC_P->Main_page->main_game,220,10);

    //退出按钮
    UC_P->Main_page->exit_bottom=lv_btn_create(UC_P->Main_page->main_ui);
    lv_obj_set_size(UC_P->Main_page->exit_bottom,70,30);
    lv_obj_set_pos(UC_P->Main_page->exit_bottom,720,10);
    lv_obj_add_event_cb(UC_P->Main_page->exit_bottom, Exit_page, LV_EVENT_SHORT_CLICKED, UC_P);



    //退出文字
    UC_P->Main_page->exit_lab=lv_label_create(UC_P->Main_page->exit_bottom);
    lv_label_set_recolor(UC_P->Main_page->exit_lab,1);
    lv_obj_center(UC_P->Main_page->exit_lab);
    lv_label_set_text(UC_P->Main_page->exit_lab,"#ff0000 E##fffb00 X##00ecf0 I##d626e7 T");

    if(Mode_list(UC_P) == false)
    {
        perror("show dir list ");
    }

    return UC_P->Main_page;
}
//——————————————————————————————————————————————————————————————————————————————————————

//退出按钮回调函数
void Exit_page(lv_event_t * e)
{
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;
    UC_P->End_page=End_page(UC_P);
    lv_scr_load_anim(UC_P->End_page->end_ui,LV_SCR_LOAD_ANIM_FADE_ON,500,0,true);
    free(UC_P->Main_page);
    UC_P->Main_page=NULL;
    lv_timer_t * t = lv_timer_create(back_to_start_cb, 5000, UC_P);
    lv_timer_set_repeat_count(t, 1);  // 只执行一次
    return ;
}
//——————————————————————————————————————————————————————————————————————————————————————


//退出界面
ED_P End_page(struct UI_Contral * UC_P)
{
    UC_P->End_page = malloc(sizeof(struct End_page));
    if(UC_P->End_page == NULL) 
    {
        perror("malloc end_page ");
        return NULL;
    }
    memset(UC_P->End_page, 0, sizeof(struct End_page));
    UC_P->End_page->end_ui=lv_obj_create(NULL);
    UC_P->End_page->backgound=lv_img_create(UC_P->End_page->end_ui);
    lv_img_set_src(UC_P->End_page->backgound,"S:/2.jpg");
    lv_obj_center(UC_P->End_page->backgound);
    UC_P->End_page->end_lab=lv_label_create(UC_P->End_page->end_ui);
    lv_label_set_recolor(UC_P->End_page->end_lab,1);
    lv_obj_center(UC_P->End_page->end_lab);
    lv_label_set_text(UC_P->End_page->end_lab,"#00ffff Thank you for your use Game system\n#");
    return UC_P->End_page;

}
//——————————————————————————————————————————————————————————————————————————————————————

//返回到初始界面
void back_to_start_cb(lv_timer_t * timer)
{
    struct UI_Contral * UC_E = (struct UI_Contral *)timer->user_data;
    UC_E->Start_page = Start_page(UC_E);

    lv_scr_load_anim(UC_E->Start_page->Start_ui,
                    LV_SCR_LOAD_ANIM_FADE_ON,
                    500, 0, true);

    free(UC_E->End_page);
    UC_E->End_page = NULL;
    lv_timer_del(timer); // 删除定时器，避免重复触发
}
//——————————————————————————————————————————————————————————————————————————————————————

//功能list创建
bool Mode_list(struct UI_Contral * UC_P)
{
    GM_P game_mode= malloc(sizeof(GM));
    if(game_mode == NULL) {
        perror("malloc game_mode ");
        return false;
    }
    memset(game_mode, 0, sizeof(GM));
    game_mode->Game=lv_list_add_btn(UC_P->Main_page->main_list,NULL,"Game");
    lv_obj_add_event_cb(game_mode->Game, Game_Show_cb, LV_EVENT_SHORT_CLICKED, UC_P);




    game_mode->ranking_list=lv_list_add_btn(UC_P->Main_page->main_list,NULL,"Ranking_list");
    //lv_obj_add_event_cb(game_mode->ranking_list,ranking_list_Show,LV_EVENT_SHORT_CLICKED,UC_P);


    game_mode->User_inf=lv_list_add_btn(UC_P->Main_page->main_list,NULL,"USER");
    //lv_obj_add_event_cb(game_mode->User_inf,User_inf_Show,LV_EVENT_SHORT_CLICKED,UC_P);
    
    return true;
}
//——————————————————————————————————————————————————————————————————————————————————————

//游戏的创建
bool Game_Show(struct UI_Contral * UC_P)
{
    int btn_x=8,btn_y=8;
    lv_obj_t * game_2048=lv_btn_create(UC_P->Main_page->main_game);
    lv_obj_set_size(game_2048,game_btn_x,game_btn_x);
    lv_obj_set_pos(game_2048,btn_x,btn_y);
    lv_obj_t * game_2048_lab = lv_label_create(UC_P->Main_page->main_game);
    lv_obj_set_pos(game_2048_lab,30,110);
    lv_label_set_text(game_2048_lab,"2048");
    lv_obj_add_event_cb(game_2048, game_2048_btn_event_handler, LV_EVENT_SHORT_CLICKED, UC_P);

    btn_x+=100;
    lv_obj_t * game_memory=lv_btn_create(UC_P->Main_page->main_game);
    lv_obj_set_size(game_memory,game_btn_x,game_btn_x);
    lv_obj_set_pos(game_memory,btn_x,btn_y);
    lv_obj_t * game_memory_lab = lv_label_create(UC_P->Main_page->main_game);
    lv_obj_set_pos(game_memory_lab,105,110);
    lv_label_set_text(game_memory_lab,"memory game");
    lv_obj_add_event_cb(game_memory, game_memory_btn_event_handler, LV_EVENT_SHORT_CLICKED, UC_P);
    return true;
}
//——————————————————————————————————————————————————————————————————————————————————————

//游戏游玩界面进入
void Enter_Game_page(struct UI_Contral * UC_P)
{
    UC_P->Game_page=Game_page(UC_P);
    if(!UC_P->Game_page) return;
    lv_scr_load_anim(UC_P->Game_page->Game_ui,LV_SCR_LOAD_ANIM_FADE_ON,500,0,true);
    free(UC_P->Main_page);
    UC_P->Main_page=NULL;
    return ;
}
//——————————————————————————————————————————————————————————————————————————————————————

//游戏游玩界面创建
GP_P Game_page(struct UI_Contral * UC_P)
{
    if(!UC_P->Game_page)
    {
        UC_P->Game_page = malloc(sizeof(struct Game_page));
        if(!UC_P->Game_page)
        { 
            perror("malloc game_page "); 
            return NULL; 
        }
        memset(UC_P->Game_page, 0, sizeof(struct Game_page));
    }

    UC_P->Game_page->Game_ui=lv_obj_create(NULL);
    // UC_P->Game_page->backgound=lv_img_create(UC_P->Game_page->Game_ui);
    // lv_img_set_src(UC_P->Game_page->backgound,"S:/2.jpg");

    UC_P->Game_page->back_btn=lv_btn_create(UC_P->Game_page->Game_ui);
    lv_obj_set_size(UC_P->Game_page->back_btn,70,30);
    lv_obj_set_pos(UC_P->Game_page->back_btn,720,10);
    lv_obj_add_event_cb(UC_P->Game_page->back_btn, Return_page, LV_EVENT_SHORT_CLICKED, UC_P);

    //退出文字
    UC_P->Game_page->back_lab=lv_label_create(UC_P->Game_page->back_btn);
    lv_label_set_recolor(UC_P->Game_page->back_lab,1);
    lv_obj_center(UC_P->Game_page->back_lab);
    lv_label_set_text(UC_P->Game_page->back_lab,"#ff0000 E##fffb00 X##00ecf0 I##d626e7 T");

    return UC_P->Game_page;

}

//返回主界面
void Return_page(lv_event_t * e)
{
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;
    UC_P->Main_page=Main_page(UC_P);
    lv_scr_load_anim(UC_P->Main_page->main_ui,LV_SCR_LOAD_ANIM_FADE_ON,500,0,true);
    free(UC_P->Game_page);
    UC_P->Game_page=NULL;
    return ;
}

// 适配 LVGL 事件的回调：把 lv_event_t* 转给 Game_Show(UC_P)
static void Game_Show_cb(lv_event_t * e)
{
    // 取事件码（可选：只在点击时处理）
    lv_event_code_t code = lv_event_get_code(e);
    if(code != LV_EVENT_SHORT_CLICKED && code != LV_EVENT_CLICKED) return;

    //创建新界面
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(UC_P == NULL) return;
    // Enter_Game_page(UC_P);
    // 调用你原本的业务函数，不改它的签名
    Game_Show(UC_P);
}

//创建函数调用——清除主界面（释放空间，来保证不怎么卡），转到2048
static void game_2048_btn_event_handler(lv_event_t * e)
{
    
    if(lv_event_get_code(e) != LV_EVENT_SHORT_CLICKED && lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;
    Enter_Game_page(UC_P);
    //延迟切换
    lv_timer_t * t = lv_timer_create(start_2048_cb, 10, UC_P);
    lv_timer_set_repeat_count(t, 1);

}

/*用延迟等待 Game_ui 之后再创建 2048 */
static void start_2048_cb(lv_timer_t * timer)
{
    struct UI_Contral * UC_P = (struct UI_Contral *)timer->user_data;
    if(!UC_P || !UC_P->Game_page || !UC_P->Game_page->Game_ui) { lv_timer_del(timer); return; }
    lv_100ask_2048_simple_test();
    lv_timer_del(timer);
}

//创建函数调用——清除主界面（释放空间，来保证不怎么卡），转到记忆游戏
static void game_memory_btn_event_handler(lv_event_t * e)
{
    
    if(lv_event_get_code(e) != LV_EVENT_SHORT_CLICKED && lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;
    Enter_Game_page(UC_P);
    //延迟切换
    lv_timer_t * t = lv_timer_create(start_memory_cb, 10, UC_P);
    lv_timer_set_repeat_count(t, 1);

}

/*用延迟等待 Game_ui 之后再创建 记忆游戏 */
static void start_memory_cb(lv_timer_t * timer)
{
    struct UI_Contral * UC_P = (struct UI_Contral *)timer->user_data;
    if(!UC_P || !UC_P->Game_page || !UC_P->Game_page->Game_ui) { lv_timer_del(timer); return; }
    lv_100ask_memory_game_simple_test();
    lv_timer_del(timer);
}


//2048游戏程序--1
static void game_2048_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj_2048 = lv_event_get_target(e);
    lv_obj_t * label = lv_event_get_user_data(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) 
    {
        umask(0000);
        int sc=open("./scores.txt",O_RDWR|O_CREAT,0777);
        int scores=0;
        lseek(sc, 0, SEEK_SET);
        ssize_t rlen = read(sc, &scores, sizeof(scores));
        if (rlen != sizeof(scores)) scores = 0;   // 新文件或读失败时默认0
        if (lv_100ask_2048_get_best_tile(obj_2048) >= 2048)
        lv_label_set_text(label, "#00b329 YOU WIN! #");
        else if(lv_100ask_2048_get_status(obj_2048))
        lv_label_set_text(label, " #ff0000 GAME OVER! #");
        else
        lv_label_set_text_fmt(label, "MAX_SCORE:#16efff %d #  SCORE:#ff00ff %d #",scores, lv_100ask_2048_get_score(obj_2048));

        if(scores<lv_100ask_2048_get_score(obj_2048))
        {
            int cur = lv_100ask_2048_get_score(obj_2048);
            if (scores < cur) 
            {
                lseek(sc, 0, SEEK_SET);
                write(sc, &cur, sizeof(cur));
                fsync(sc);
            }       

        }
        close(sc);
    }
}
//2048游戏程序--2
static void new_game_btn_event_handler(lv_event_t * e)
{
    lv_obj_t * obj_2048 = lv_event_get_user_data(e);

    lv_100ask_2048_set_new_game(obj_2048);
}
//2048游戏程序--3（main)
void lv_100ask_2048_simple_test(void)
{
    /*Create 2048 game*/
    lv_obj_t * obj_2048 = lv_100ask_2048_create(lv_scr_act());
#if LV_FONT_MONTSERRAT_40    
    lv_obj_set_style_text_font(obj_2048, &lv_font_montserrat_40, 0);
#endif
    lv_obj_set_size(obj_2048, 400, 400);
    lv_obj_center(obj_2048);

    /*Information*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_recolor(label, true); 
    lv_label_set_text_fmt(label, "SCORE: #ff00ff %d #", lv_100ask_2048_get_score(obj_2048));
    lv_obj_align_to(label, obj_2048, LV_ALIGN_OUT_TOP_RIGHT, -50, -10);

    lv_obj_add_event_cb(obj_2048, game_2048_event_cb, LV_EVENT_ALL, label);

    /*New Game*/
    lv_obj_t * btn = lv_btn_create(lv_scr_act());
    lv_obj_align_to(btn, obj_2048, LV_ALIGN_OUT_TOP_LEFT, 0, -25);
    lv_obj_add_event_cb(btn, new_game_btn_event_handler, LV_EVENT_CLICKED, obj_2048);

    label = lv_label_create(btn);
    lv_label_set_text(label, "New Game");
    lv_obj_center(label);
}
//——————————————————————————————————————————————————————————————————————————————————————

//Game1----------2048

lv_obj_t * lv_100ask_2048_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_2048, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_100ask_2048_set_new_game(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS_2048);

    lv_100ask_2048_t * game_2048 = (lv_100ask_2048_t *)obj;

    game_2048->score = 0;
    game_2048->game_over = false;
    game_2048->map_count = MATRIX_SIZE * MATRIX_SIZE + MATRIX_SIZE;

    init_matrix_num(game_2048->matrix);
    update_btnm_map(game_2048->btnm_map, game_2048->matrix);
    lv_btnmatrix_set_map(game_2048->btnm, game_2048->btnm_map);

    lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
}

uint16_t lv_100ask_2048_get_score(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS_2048);

    lv_100ask_2048_t * game_2048 = (lv_100ask_2048_t *)obj;

    return game_2048->score;
}

bool lv_100ask_2048_get_status(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS_2048);

    lv_100ask_2048_t * game_2048 = (lv_100ask_2048_t *)obj;

    return game_2048->game_over;
}

uint16_t lv_100ask_2048_get_best_tile(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS_2048);

    lv_100ask_2048_t * game_2048 = (lv_100ask_2048_t *)obj;

    uint8_t x, y;
    uint16_t best_tile = 0;

	for (x = 0; x < MATRIX_SIZE; x++) {
		for (y = 0; y < MATRIX_SIZE; y++) {
			if (best_tile < game_2048->matrix[x][y])
                best_tile = game_2048->matrix[x][y];
		}
	}

    return (uint16_t)(1 << best_tile);
}

static void lv_100ask_2048_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_100ask_2048_t * game_2048 = (lv_100ask_2048_t *)obj;

    game_2048->score = 0;
    game_2048->game_over = false;
    game_2048->map_count = MATRIX_SIZE * MATRIX_SIZE + MATRIX_SIZE;

    uint16_t index;
    for (index = 0; index < game_2048->map_count; index++) {

        if (((index + 1) % 5) == 0)
        {
            game_2048->btnm_map[index] = lv_mem_alloc(2);
            if ((index+1) == game_2048->map_count)
                strcpy(game_2048->btnm_map[index], "");
            else
                strcpy(game_2048->btnm_map[index], "\n");
        }
        else
        {
            game_2048->btnm_map[index] = lv_mem_alloc(5);
            strcpy(game_2048->btnm_map[index], " ");
        }
    }

    init_matrix_num(game_2048->matrix);
    update_btnm_map(game_2048->btnm_map, game_2048->matrix);

    /*obj style init*/
    lv_theme_t * theme = lv_theme_get_from_obj(obj);
    lv_obj_set_style_outline_color(obj, theme->color_primary, LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_width(obj, lv_disp_dpx(theme->disp, 2), LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_pad(obj, lv_disp_dpx(theme->disp, 2), LV_STATE_FOCUS_KEY);
    lv_obj_set_style_outline_opa(obj, LV_OPA_50, LV_STATE_FOCUS_KEY);

    /*game_2048->btnm init*/
    game_2048->btnm = lv_btnmatrix_create(obj);
    lv_obj_set_size(game_2048->btnm, LV_PCT(100), LV_PCT(100));
    lv_obj_center(game_2048->btnm);
    lv_obj_set_style_pad_all(game_2048->btnm, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(game_2048->btnm, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_group_remove_obj(game_2048->btnm);
    lv_obj_add_flag(game_2048->btnm, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_btnmatrix_set_map(game_2048->btnm, game_2048->btnm_map);
    lv_btnmatrix_set_btn_ctrl_all(game_2048->btnm, LV_BTNMATRIX_CTRL_DISABLED);

    lv_obj_add_event_cb(game_2048->btnm, btnm_event_cb, LV_EVENT_ALL, NULL);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_100ask_2048_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_100ask_2048_t * game_2048 = (lv_100ask_2048_t *)obj;

    uint16_t index, count;
    for (index = 0; index < game_2048->map_count; index++)
    {
        lv_mem_free(game_2048->btnm_map[index]);
    }
}

static void lv_100ask_2048_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS_2048, e);
    if(res != LV_RES_OK) return;

    bool success = false;
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btnm = lv_event_get_target(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    lv_100ask_2048_t * game_2048 = (lv_100ask_2048_t *)obj;

    if (code == LV_EVENT_CLICKED)
    {
        game_2048->game_over = game_over(game_2048->matrix);
        if (!game_2048->game_over)
        {
            switch(lv_indev_get_gesture_dir(lv_indev_get_act()))
            {
                case LV_DIR_TOP:
                    success = move_left(&(game_2048->score), game_2048->matrix);
                    break;
                case LV_DIR_BOTTOM:
                    success = move_right(&(game_2048->score), game_2048->matrix);
                    break;
                case LV_DIR_LEFT:
                    success = move_up(&(game_2048->score), game_2048->matrix);
                    break;
                case LV_DIR_RIGHT:
                    success = move_down(&(game_2048->score), game_2048->matrix);
                    break;
                default: break;
            }
        }
        else
        {
            LV_LOG_USER("2048 GAME OVER!");
            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }
    }
    else if(code == LV_EVENT_KEY)
    {
        game_2048->game_over = game_over(game_2048->matrix);
        if (!game_2048->game_over)
        {
            switch(*((uint8_t *)lv_event_get_param(e)))
            {
                case LV_KEY_UP:
                    success = move_left(&(game_2048->score), game_2048->matrix);
                    break;
                case LV_KEY_DOWN:
                    success = move_right(&(game_2048->score), game_2048->matrix);
                    break;
                case LV_KEY_LEFT:
                    success = move_up(&(game_2048->score), game_2048->matrix);
                    break;
                case LV_KEY_RIGHT:
                    success = move_down(&(game_2048->score), game_2048->matrix);
                    break;
                default: break;
            }
        }
        else
        {
            LV_LOG_USER("2048 GAME OVER!");
            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }
    }

    if (success)
    {
        addRandom(game_2048->matrix);
        update_btnm_map(game_2048->btnm_map, game_2048->matrix);
        lv_btnmatrix_set_map(game_2048->btnm, game_2048->btnm_map);

        res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
        if(res != LV_RES_OK) return;
    }
}

static void btnm_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btnm = lv_event_get_target(e);
    lv_obj_t * parent = lv_obj_get_parent(btnm);

     lv_100ask_2048_t * game_2048 = (lv_100ask_2048_t *)parent;

    if(code == LV_EVENT_DRAW_PART_BEGIN) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);

        /*Change the draw descriptor the button*/
        if((dsc->id >= 0) && (dsc->label_dsc))
        {
            uint16_t x, y, num;

            x = (uint16_t)((dsc->id) / 4);
            y = (dsc->id) % 4;
            num = (uint16_t)(1 << (game_2048->matrix[x][y]));


            dsc->rect_dsc->radius = 3;
            dsc->rect_dsc->bg_color = get_num_color(num);

            if (num < 8)
                dsc->label_dsc->color = LV_100ASK_2048_TEXT_BLACK_COLOR;
            else
                dsc->label_dsc->color = LV_100ASK_2048_TEXT_WHITE_COLOR;

        }
        /*Change the draw descriptor the btnm main*/
        else if((dsc->id == 0) && !(dsc->label_dsc))
        {
            dsc->rect_dsc->radius = 5;
            dsc->rect_dsc->bg_color = LV_100ASK_2048_BG_COLOR;
            dsc->rect_dsc->border_width = 0;
        }
    }
}

static void init_matrix_num(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE])
{
	uint8_t x, y;

	for (x = 0; x < MATRIX_SIZE; x++) {
		for (y = 0; y < MATRIX_SIZE; y++) {
			matrix[x][y] = 0;
		}
	}

	/* 初始化两个随机位置的随机数 */
	addRandom(matrix);
	addRandom(matrix);
}

static void addRandom(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE])
{
	static bool initialized = false;
	uint16_t x, y;
	uint16_t r, len = 0;
	uint16_t n, list[MATRIX_SIZE * MATRIX_SIZE][2];

	if (!initialized) {
		srand(time(NULL));
		initialized = true;
	}

	for (x = 0; x < MATRIX_SIZE; x++) {
		for (y = 0; y < MATRIX_SIZE; y++) {
			if (matrix[x][y] == 0) {
				list[len][0] = x;
				list[len][1] = y;
				len++;
			}
		}
	}

	if (len > 0) {
		r = rand() % len;
		x = list[r][0];
		y = list[r][1];
		n = ((rand() % 10) / 9) + 1;
		matrix[x][y] = n;
	}
 }

static void update_btnm_map(char * btnm_map[], uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE])
{
    uint8_t x, y, index;

    index = 0;
    for (x = 0; x < MATRIX_SIZE; x++) {
		for (y = 0; y < MATRIX_SIZE; y++) {

            if (((index + 1) % 5) == 0)
                index++;

            if (matrix[x][y] != 0)
                int_to_str(btnm_map[index], (uint16_t)(1 << matrix[x][y]));
            else
            	strcpy(btnm_map[index], " ");

            index++;

        }
    }
}

static char* int_to_str(char * str, uint16_t num)
{
	uint8_t i = 0;//指示填充str
	if(num < 0)//如果num为负数，将num变正
	{
		num = -num;
		str[i++] = '-';
	}
	//转换
	do
	{
		str[i++] = num%10+48;//取num最低位 字符0~9的ASCII码是48~57；简单来说数字0+48=48，ASCII码对应字符'0'
		num /= 10;//去掉最低位
	}while(num);//num不为0继续循环

	str[i] = '\0';

	//确定开始调整的位置
	uint8_t j = 0;
	if(str[0] == '-')//如果有负号，负号不用调整
	{
		j = 1;//从第二位开始调整
		++i;//由于有负号，所以交换的对称轴也要后移1位
	}
	//对称交换
	for(; j < (i / 2); j++)
	{
		//对称交换两端的值 其实就是省下中间变量交换a+b的值：a=a+b;b=a-b;a=a-b;
		str[j] = str[j] + str[i-1-j];
		str[i-1-j] = str[j] - str[i-1-j];
		str[j] = str[j] - str[i-1-j];
	}

	return str;//返回转换后的值
}

static uint8_t find_target(uint16_t array[MATRIX_SIZE], uint8_t x, uint8_t stop) {
	uint8_t t;
	// if the position is already on the first, don't evaluate
	if (x == 0) {
		return x;
	}
	for(t = x-1; ; t--) {
		if (array[t] != 0) {
			if (array[t] != array[x]) {
				// merge is not possible, take next position
				return t + 1;
			}
			return t;
		} else {
			// we should not slide further, return this one
			if (t == stop) {
				return t;
			}
		}
	}
	// we did not find a
	return x;
}

static bool slide_array(uint16_t * score, uint16_t array[MATRIX_SIZE]) {
	bool success = false;
	uint8_t x, t, stop = 0;

	for (x = 0; x < MATRIX_SIZE; x++) {
		if (array[x] != 0) {
			t = find_target(array,x,stop);
			// if target is not original position, then move or merge
			if (t != x) {
				// if target is zero, this is a move
				if (array[t] == 0) {
					array[t] = array[x];
				} else if (array[t] == array[x]) {
					// merge (increase power of two)
					array[t]++;
					// increase score
					*score += (uint32_t)1<<array[t];
					// set stop to avoid double merge
					stop = t + 1;
				}
				array[x] = 0;
				success = true;
			}
		}
	}
	return success;
}

static void rotate_matrix(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]) {
	uint8_t i,j,n = MATRIX_SIZE;
	uint16_t tmp;

	for (i = 0; i < (n/2); i++) {
		for (j = i; j < (n-i-1); j++) {
			tmp = matrix[i][j];
			matrix[i][j] = matrix[j][n-i-1];
			matrix[j][n-i-1] = matrix[n-i-1][n-j-1];
			matrix[n-i-1][n-j-1] = matrix[n-j-1][i];
			matrix[n-j-1][i] = tmp;
		}
	}
}

static bool move_up(uint16_t * score, uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE])
{
	bool success = false;
	uint8_t x;

	for (x = 0; x < MATRIX_SIZE; x++) {
		success |= slide_array(score, matrix[x]);
	}
	return success;
}

static bool move_left(uint16_t * score, uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE])
{
	bool success;

	rotate_matrix(matrix);

    success = move_up(score, matrix);
	rotate_matrix(matrix);
	rotate_matrix(matrix);
	rotate_matrix(matrix);

    return success;
}

static bool move_down(uint16_t * score, uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE])
{
	bool success;

	rotate_matrix(matrix);
	rotate_matrix(matrix);

    success = move_up(score, matrix);
	rotate_matrix(matrix);
	rotate_matrix(matrix);

    return success;
}

static bool move_right(uint16_t * score, uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE])
{
	bool success;

	rotate_matrix(matrix);
	rotate_matrix(matrix);
	rotate_matrix(matrix);

    success = move_up(score, matrix);
	rotate_matrix(matrix);

    return success;
}

static bool find_pair_down(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]) {
	bool success = false;
	uint8_t x, y;

	for (x = 0; x < MATRIX_SIZE; x++) {
		for (y = 0; y < (MATRIX_SIZE-1); y++) {
			if (matrix[x][y] == matrix[x][y+1])
                return true;
		}
	}
	return success;
}

static uint8_t count_empty(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE]) {
	uint8_t x, y;
	uint8_t count = 0;

	for (x = 0; x < MATRIX_SIZE; x++) {
		for (y = 0; y < MATRIX_SIZE; y++) {
			if (matrix[x][y] == 0) {
				count++;
			}
		}
	}
	return count;
}

static lv_color_t get_num_color(uint16_t num)
{
    lv_color_t color;

    switch (num)
    {
        case 0:		color = LV_100ASK_2048_NUMBER_EMPTY_COLOR;  break;
        case 1:		color = LV_100ASK_2048_NUMBER_EMPTY_COLOR;  break;
        case 2:		color = LV_100ASK_2048_NUMBER_2_COLOR;	    break;
        case 4:		color = LV_100ASK_2048_NUMBER_4_COLOR;	    break;
        case 8:		color = LV_100ASK_2048_NUMBER_8_COLOR;	    break;
        case 16:	color = LV_100ASK_2048_NUMBER_16_COLOR;	    break;
        case 32:	color = LV_100ASK_2048_NUMBER_32_COLOR;	    break;
        case 64:	color = LV_100ASK_2048_NUMBER_64_COLOR;	    break;
        case 128:	color = LV_100ASK_2048_NUMBER_128_COLOR;	break;
        case 256:	color = LV_100ASK_2048_NUMBER_256_COLOR;	break;
        case 512:	color = LV_100ASK_2048_NUMBER_512_COLOR;	break;
        case 1024:	color = LV_100ASK_2048_NUMBER_1024_COLOR;   break;
        case 2048:	color = LV_100ASK_2048_NUMBER_2048_COLOR;   break;
        default:	color =  LV_100ASK_2048_NUMBER_2048_COLOR;break;
    }
    return color;
}

static bool game_over(uint16_t matrix[MATRIX_SIZE][MATRIX_SIZE])
{
	bool ended = true;

    if (count_empty(matrix) > 0) return false;
	if (find_pair_down(matrix)) return false;

	rotate_matrix(matrix);
	if (find_pair_down(matrix)) ended = false;

    rotate_matrix(matrix);
	rotate_matrix(matrix);
	rotate_matrix(matrix);

    return ended;
}

//——————————————————————————————————————————————————————————————————————————————————————

//memory游戏1
static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    lv_obj_t * memory_game = lv_event_get_user_data(e);

    lv_100ask_memory_game_set_map(memory_game, lv_slider_get_value(slider), lv_slider_get_value(slider));
    lv_label_set_text_fmt(level_label, "%d*%d", lv_100ask_memory_game_get_row(memory_game), lv_100ask_memory_game_get_row(memory_game));
}

//memory游戏2（main)
void lv_100ask_memory_game_simple_test(void)
{
	lv_obj_t * memory_game = lv_100ask_memory_game_create(lv_scr_act());
    lv_obj_set_size(memory_game, 480, 480);
    //lv_100ask_memory_set_map(memory_game, 2, 2);
    lv_obj_set_style_text_font(memory_game, &lv_font_montserrat_32, 0);
    lv_obj_center(memory_game);

    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_align_to(slider, memory_game, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_slider_set_range(slider, 2, 10);
    lv_slider_set_value(slider, lv_100ask_memory_game_get_row(memory_game), LV_ANIM_ON);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, memory_game);

    level_label = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(level_label, "%d*%d", lv_100ask_memory_game_get_row(memory_game), lv_100ask_memory_game_get_row(memory_game));

    lv_obj_align_to(level_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
}

lv_obj_t * lv_100ask_memory_game_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_memory, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}




void lv_100ask_memory_game_set_map(lv_obj_t * obj, uint16_t row, uint16_t col)
{
    LV_ASSERT_OBJ(obj, MY_CLASS_memory);

    if ((row == 0) || (col == 0))   return;
    if ((row % 2)) row++;
    if ((col % 2)) col++;
    
    lv_100ask_memory_game_t * memory_game = (lv_100ask_memory_game_t *)obj;

    int16_t count = memory_game->row * memory_game->col;
    int16_t cur_count = row * col;
    int16_t new_count = 0;

    new_count = count - cur_count;
    if(new_count > 0)
    {   
        lv_obj_t * item;
        for(int16_t i = 0; i < new_count; i++)
        {
            /*Delete the hit object first*/
            for(uint16_t i = 0; i < lv_obj_get_child_cnt(obj); i++) {
                lv_obj_t * child = lv_obj_get_child(obj, i);
                if((child) && (!lv_obj_has_flag(child, LV_OBJ_FLAG_CLICKABLE)))
                {
                    lv_obj_del(child);
                    break;
                }
                else if ((i+1) == lv_obj_get_child_cnt(obj))
                {
                    lv_obj_del(child);
                }
            }
        }
    }
    else if(new_count < 0)
    {
        lv_obj_t * item;
        lv_obj_t * label;
        for(int16_t i = 0; i > new_count; i--)
        {
            item = lv_btn_create(obj);
            lv_obj_add_style(item, &item_def_style, 0);
            lv_obj_add_style(item, &item_click_style, LV_STATE_PRESSED);

            label = lv_label_create(item);
            lv_obj_center(label);
        }
        /*Recover the hit object*/
        for(uint16_t i = 0; i < lv_obj_get_child_cnt(obj); i++) {
            lv_obj_t * child = lv_obj_get_child(obj, i);
            if((child) && (!lv_obj_has_flag(child, LV_OBJ_FLAG_CLICKABLE)))
            {
                lv_obj_remove_style(child, &item_hit_style, 0);
                lv_obj_add_flag(child, LV_OBJ_FLAG_CLICKABLE);
            }
        }
    }

    uint16_t list_number[cur_count + 1];
    list_rand_number(list_number, cur_count, (cur_count / 2)); 

    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    for(uint16_t i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        if(child)
        {
            lv_obj_set_size(child, (w / row), (h / col));
            lv_obj_add_event_cb(child, item_event_handler, LV_EVENT_CLICKED, obj);
        }

        lv_obj_t * label = lv_obj_get_child(child, 0);
        if (label && i <= cur_count)
        {
            lv_label_set_text_fmt(label, "%d", list_number[i]);
            lv_obj_center(label);
        }
    }

    memory_game->bef = NULL;
    memory_game->row = row;
    memory_game->col = col;

    lv_obj_invalidate(obj);
}



uint16_t lv_100ask_memory_game_get_row(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS_memory);
    
    lv_100ask_memory_game_t * memory_game = (lv_100ask_memory_game_t *)obj;

    return memory_game->row;
}

uint16_t lv_100ask_memory_game_get_col(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS_memory);
    
    lv_100ask_memory_game_t * memory_game = (lv_100ask_memory_game_t *)obj;

    return memory_game->col;
}



static void lv_100ask_memory_game_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_100ask_memory_game_t * memory_game = (lv_100ask_memory_game_t *)obj;

    memory_game->bef = NULL;
    memory_game->row = 0;
    memory_game->col = 0;

    lv_style_init(&cont_style);
    lv_style_set_flex_flow(&cont_style, LV_FLEX_FLOW_ROW_WRAP);
    lv_style_set_flex_main_place(&cont_style, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_style_set_layout(&cont_style, LV_LAYOUT_FLEX);
    lv_style_set_pad_all(&cont_style, 0);

	lv_style_init(&item_def_style);
	lv_style_set_radius(&item_def_style, 0);
    lv_style_set_border_width(&item_def_style, 1);
    lv_style_set_border_color(&item_def_style, lv_color_hex(0xffffff));
    lv_style_set_text_opa(&item_def_style, LV_OPA_0);
    lv_style_set_shadow_opa(&item_def_style, LV_OPA_0);

    lv_style_init(&item_hit_style);
    lv_style_set_border_width(&item_hit_style, 0);
    lv_style_set_bg_opa(&item_hit_style, LV_OPA_0);
    lv_style_set_text_opa(&item_hit_style, LV_OPA_0);
    lv_style_set_border_opa(&item_hit_style, LV_OPA_0);
    lv_style_set_shadow_opa(&item_hit_style, LV_OPA_0);

    lv_style_init(&item_click_style);
    lv_style_set_border_width(&item_click_style, 0);
    lv_style_set_text_opa(&item_click_style, LV_OPA_100);
    lv_style_set_bg_opa(&item_click_style, LV_OPA_100);
    lv_style_set_border_opa(&item_click_style, LV_OPA_100);
    lv_style_set_shadow_opa(&item_click_style, LV_OPA_100);

    lv_obj_add_style(obj, &cont_style, 0);

    lv_obj_update_layout(obj);
    lv_100ask_memory_game_set_map(obj, LV_100ASK_MEMORY_GAME_DEFAULT_ROW, LV_100ASK_MEMORY_GAME_DEFAULT_COL);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_100ask_memory_game_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
}


static void lv_100ask_memory_game_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS_memory, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_100ask_memory_game_t * memory_game = (lv_100ask_memory_game_t *)obj;

    if (code == LV_EVENT_SIZE_CHANGED)
    {
        lv_100ask_memory_game_set_map(obj, memory_game->row, memory_game->col);
    }
}



static void item_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_t * user_data = lv_event_get_user_data(e);

    lv_100ask_memory_game_t * memory_game = (lv_100ask_memory_game_t *)user_data;

    if(code == LV_EVENT_CLICKED)
    {
        if (NULL == memory_game->bef)
        {
            memory_game->bef = lv_event_get_target(e);
            return;
        }
        else
        {
            if(obj == memory_game->bef) return;
        }

        if (strcmp(\
                   lv_label_get_text(lv_obj_get_child(obj, 0)),\
                   lv_label_get_text(lv_obj_get_child(memory_game->bef, 0))) == 0)
        {
            lv_obj_add_style(obj, &item_hit_style, 0);
            lv_obj_add_style(memory_game->bef, &item_hit_style, 0);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
            lv_obj_clear_flag(memory_game->bef, LV_OBJ_FLAG_CLICKABLE);
        }

        memory_game->bef = obj;
    }
}



static void list_rand_number(uint16_t arry[], uint16_t max_count, uint16_t count)
{
	int w, t;

	srand((unsigned)time(NULL));

	for (int i = 0; i < max_count; i++)
	    arry[i] = (i % count) + 1;
	for (int i = 0; i < max_count; i++)
	{
		w = rand() % (count - (i % count)) + i;
        if (w > max_count)  w = max_count - 1;

	    t = arry[i];
	    arry[i] = arry[w];
	    arry[w] = t;
	}
}
