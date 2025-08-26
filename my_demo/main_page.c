#include"main_page.h"
#include <dirent.h>
int main_page(void)
{
    static struct All_UI UC = {NULL,NULL,NULL,NULL};

    UC.start_ui_p = Show_Start_UI(&UC);
    if (UC.start_ui_p == NULL)
    {
        printf("显示界面初始化失败\n");
        return -1;
    }
    return 0;
}
void back_to_start_cb(lv_timer_t * timer)
{
    struct All_UI * UC_E = (struct All_UI *)timer->user_data;
    UC_E->start_ui_p = Show_Start_UI(UC_E);

    lv_scr_load_anim(UC_E->start_ui_p->start_ui,
                    LV_SCR_LOAD_ANIM_FADE_ON,
                    500, 0, true);

    free(UC_E->end_ui_p);
    UC_E->end_ui_p = NULL;
    lv_timer_del(timer); // 删除定时器，避免重复触发
}

void Enter_Btn(lv_event_t *e)
{
    struct All_UI * UC_P = (struct All_UI *)e->user_data;
    // lv_obj_del(UC_P->start_ui_p);
    // UC_P->dir_ui_p =Show_Dir_UI();
    UC_P->dir_ui_p = Show_Dir_UI(UC_P);

    // 用动画切换屏幕（淡入淡出）
    lv_scr_load_anim(UC_P->dir_ui_p->dir_ui,
                     LV_SCR_LOAD_ANIM_FADE_ON, // 动画类型：淡入淡出
                     500,                      // 动画时长（ms）
                     0,                        // 延迟（ms）
                     true);                    // 自动删除旧 screen
    free(UC_P->start_ui_p);
    UC_P->start_ui_p = NULL;
 
    return ;
}


void Enter_Exit(lv_event_t *e)
{
    struct All_UI * UC_E = (struct All_UI *)e->user_data;
    UC_E->end_ui_p = Show_End_UI(UC_E);

    // 用动画切换屏幕（淡入淡出）
    lv_scr_load_anim(UC_E->end_ui_p->end_ui,
                     LV_SCR_LOAD_ANIM_FADE_ON, // 动画类型：淡入淡出
                     500,                      // 动画时长（ms）
                     0,                        // 延迟（ms）
                     true);                    // 自动删除旧 screen
    free(UC_E->dir_ui_p);
    UC_E->dir_ui_p = NULL;

    if (UC_E->dir_bin_inf_head) {
        Destory_Dir_Btn_List(UC_E->dir_bin_inf_head);
        free(UC_E->dir_bin_inf_head);
        UC_E->dir_bin_inf_head = NULL;
    }

    lv_timer_t * t = lv_timer_create(back_to_start_cb, 5000, UC_E);
    lv_timer_set_repeat_count(t, 1);  // 只执行一次
    
    return ;
}

Start_UI_P Show_Start_UI(struct All_UI * UC_P)
{
    UC_P->start_ui_p = (Start_UI_P)malloc(sizeof(Start_UI));
    if(UC_P->start_ui_p == (Start_UI_P)NULL)
    {
        perror("malloc start ui p ...");
        return (Start_UI_P)NULL;
    }

    memset(UC_P->start_ui_p,0,sizeof(Start_UI));


    UC_P->start_ui_p->start_ui=lv_obj_create(NULL);
    UC_P->start_ui_p->background= lv_img_create(UC_P->start_ui_p->start_ui);
    lv_img_set_src(UC_P->start_ui_p->background,"S:/3.jpg");
    lv_obj_center(UC_P->start_ui_p->background);


    UC_P->start_ui_p->tittle_lab=lv_label_create(UC_P->start_ui_p->start_ui);
    lv_label_set_long_mode(UC_P->start_ui_p->tittle_lab,LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_recolor(UC_P->start_ui_p->tittle_lab,1);
    lv_label_set_text(UC_P->start_ui_p->tittle_lab,"#ff00f7 ————欢迎使用文件控制系统————#");

    LV_FONT_DECLARE(songti_Chinese);
    lv_obj_set_style_text_font(UC_P->start_ui_p->tittle_lab,&songti_Chinese,LV_STATE_DEFAULT);


    lv_obj_set_width(UC_P->start_ui_p->tittle_lab,200);
    lv_obj_set_pos(UC_P->start_ui_p->tittle_lab,300,10);


    UC_P->start_ui_p->enter_btn=lv_btn_create(UC_P->start_ui_p->start_ui);
    // 普通状态
    lv_obj_set_style_bg_color(UC_P->start_ui_p->enter_btn, lv_color_hex(0xffff43), LV_PART_MAIN | LV_STATE_DEFAULT);

    // 按下状态
    lv_obj_set_style_bg_color(UC_P->start_ui_p->enter_btn, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_PRESSED);


    lv_obj_set_size(UC_P->start_ui_p->enter_btn,160,80);
    lv_obj_center(UC_P->start_ui_p->enter_btn);

    UC_P->start_ui_p->enter_lab=lv_label_create(UC_P->start_ui_p->start_ui);
    lv_obj_set_size(UC_P->start_ui_p->enter_lab,140,80);
    lv_obj_set_pos(UC_P->start_ui_p->enter_lab,355,230);
    lv_label_set_recolor(UC_P->start_ui_p->enter_lab,1);
    lv_label_set_text(UC_P->start_ui_p->enter_lab,"#00ff2a Enter##ff1486 System#");
    lv_obj_add_event_cb(UC_P->start_ui_p->enter_btn,Enter_Btn,LV_EVENT_SHORT_CLICKED,UC_P);

    lv_scr_load_anim(UC_P->start_ui_p->start_ui,
                 LV_SCR_LOAD_ANIM_FADE_ON,
                 500, 0, true);



    return UC_P->start_ui_p;

}

Dir_UI_P Show_Dir_UI(struct All_UI * UC_P)
{
    Dir_UI_P dir_ui_p = (Dir_UI_P)malloc(sizeof(Dir_UI));
    if(dir_ui_p == (Dir_UI_P)NULL)
    {
        perror("malloc start ui p ...");
        return (Dir_UI_P)NULL;
    }

    memset(dir_ui_p,0,sizeof(Dir_UI));
    dir_ui_p->dir_ui=lv_obj_create(NULL);

    UC_P->dir_ui_p = dir_ui_p;

    if (UC_P->dir_bin_inf_head == NULL) {
        UC_P->dir_bin_inf_head = Create_Node();
        if (UC_P->dir_bin_inf_head == (DBI_P)-1) {
            printf("创建头结点失败！\n");
            return dir_ui_p; // 或者直接 return NULL
        }
    }

    dir_ui_p->picture=lv_img_create(dir_ui_p->dir_ui);
    lv_img_set_src(dir_ui_p->picture,"S:/1.jpg");
    lv_obj_center(dir_ui_p->picture);



    dir_ui_p->dir_list= lv_list_create(dir_ui_p->dir_ui);

    lv_obj_set_size(dir_ui_p->dir_list,200,450);

    lv_obj_set_pos(dir_ui_p->dir_list,10,10);

    dir_ui_p->file_ui=lv_obj_create(dir_ui_p->dir_ui);

    lv_obj_set_size(dir_ui_p->file_ui,570,450);;

    lv_obj_set_pos(dir_ui_p->file_ui,220,10);

    //work1 --添加退出按钮


    dir_ui_p->exit_btn=lv_btn_create(dir_ui_p->dir_ui);
    dir_ui_p->exit_lab=lv_label_create(dir_ui_p->dir_ui);
    
    // 普通状态
    lv_obj_set_style_bg_color(dir_ui_p->exit_btn, lv_color_hex(0x68fcfa), LV_PART_MAIN | LV_STATE_DEFAULT);

    // 按下状态
    lv_obj_set_style_bg_color(dir_ui_p->exit_btn, lv_color_hex(0xff0000), LV_PART_MAIN | LV_STATE_PRESSED);

    lv_obj_set_size(dir_ui_p->exit_btn,70,30);
    lv_label_set_recolor(dir_ui_p->exit_lab,1);
    lv_label_set_text(dir_ui_p->exit_lab,"#f02af0 EXIT#");

    lv_obj_set_pos(dir_ui_p->exit_btn,720,10);
    lv_obj_set_pos(dir_ui_p->exit_lab,741,17);



    //work2 --设置退出界面
    lv_obj_add_event_cb(dir_ui_p->exit_btn,Enter_Exit,LV_EVENT_SHORT_CLICKED,UC_P);



    //work end
    if(Show_Dir_List(DEFAULT_SHOW_DIR,UC_P) == false)
    {
        perror("show dir list ");
    }


    //lv_scr_load(dir_ui_p->dir_ui);

    return dir_ui_p;
}


END_UI_P Show_End_UI(struct All_UI * UC_P)
{
    END_UI_P end_ui_p = (END_UI_P)malloc(sizeof(END_UI));
    if(end_ui_p == (END_UI_P)NULL)
    {
        perror("malloc start ui p ...");
        return (END_UI_P)NULL;
    }

    memset(end_ui_p,0,sizeof(END_UI));
    end_ui_p->end_ui=lv_obj_create(NULL);
    end_ui_p->end_background=lv_img_create(end_ui_p->end_ui);
    lv_img_set_src(end_ui_p->end_background,"S:/1.jpg");
    end_ui_p->end_lab=lv_label_create(end_ui_p->end_ui);
    lv_label_set_recolor(end_ui_p->end_lab,1);
    lv_label_set_text(end_ui_p->end_lab,"#dd4561 Thank you to use my system#");
    lv_obj_center(end_ui_p->end_lab);
    return end_ui_p;
}


bool Show_Dir_List(const char * obj_dir_path,struct All_UI * UC_P)
{
    DIR * dq = opendir(obj_dir_path);
    if(dq==(DIR*)NULL)
    {
        perror("opendir ");
        return false;
    }
    int file_btn_x=8,file_btn_y=8;
    while (1)
    {
        struct dirent *eq = readdir(dq);
        if(eq == (struct dirent *)NULL)
        {
            break;
        }
        if(strcmp(eq->d_name,".")==0) continue;
        if(eq->d_type==DT_DIR)
        {
            DBI_P btn_inf = Create_Node();
            if(btn_inf == (DBI_P)-1)
            {
                printf("创建目录按钮节点失败！\n");
                return false;
            }
            btn_inf->UC_P = UC_P;
            if(strcmp(eq->d_name,"..") == 0)  
            {
                
                char * addr = strrchr(obj_dir_path,'/');//获取最后一个/的地址
                
                if(strcmp(obj_dir_path,"/") == 0 ||  addr == &obj_dir_path[0]) //如果最后一个/是第一个位置那么就是二级目录
                {
                    
                    strcpy(btn_inf->dir_name,DEFAULT_SHOW_DIR);
                }
                else
                {
                    
                    //把最后一个/变成/0
                    strcpy(btn_inf->dir_name,obj_dir_path);
                    char * addr = strrchr(btn_inf->dir_name,'/');//获取最后一个/的地址
                    *addr = '\0';

                }
            }
            else
            {
                if(obj_dir_path[strlen(obj_dir_path)-1]=='/')
                {
                    sprintf(btn_inf->dir_name,"%s%s",obj_dir_path,eq->d_name);
                }
                else
                {
                    sprintf(btn_inf->dir_name,"%s/%s",obj_dir_path,eq->d_name);
                }
            }
            // btn_inf->UC_P = UC_P; 
            Head_Add_Node(UC_P->dir_bin_inf_head,btn_inf);

            lv_obj_t * dir_btn=lv_list_add_btn(UC_P->dir_ui_p->dir_list,LV_SYMBOL_DIRECTORY,eq->d_name);

            lv_obj_add_event_cb(dir_btn, Dir_Btn_Task, LV_EVENT_SHORT_CLICKED, btn_inf);
        }
        else
        {
            DBI_P btn_inf =Create_Node();
            if(btn_inf==(DBI_P)-1)
            {
                printf("创建头节点失败\n");
                return false;
            }
            Head_Add_Node(UC_P->dir_bin_inf_head,btn_inf);
            btn_inf->UC_P=UC_P;
            if(obj_dir_path[strlen(obj_dir_path)-1]=='/')
            {
                sprintf(btn_inf->dir_name,"%s%s",obj_dir_path,eq->d_name);
            }
            else
            {
                sprintf(btn_inf->dir_name,"%s/%s",obj_dir_path,eq->d_name);
            }


            lv_obj_t * file_btn=lv_btn_create(UC_P->dir_ui_p->file_ui);
            lv_obj_set_size(file_btn,80,80);
            lv_obj_set_pos(file_btn,file_btn_x,file_btn_y);
            lv_obj_t * btn_lab = lv_label_create(file_btn);
            lv_label_set_text(btn_lab,eq->d_name);
            lv_label_set_long_mode(btn_lab,LV_LABEL_LONG_SCROLL_CIRCULAR);
            lv_obj_set_width(btn_lab,80);

            file_btn_x+=90;
            if(file_btn_x>360)
            {
                file_btn_x=8;
                file_btn_y+=90;
            }
            char * obj_p = strrchr(eq->d_name,'.');
            if(obj_p == NULL) continue;

            if((strcmp(obj_p,".jpg") == 0) || (strcmp(obj_p,".png") == 0))
            {
                lv_obj_add_event_cb(file_btn,File_Btn_Task,LV_EVENT_SHORT_CLICKED,btn_inf);
            }
            else if((strcmp(obj_p,".gif") == 0))
            {
                lv_obj_add_event_cb(file_btn,File_Btn_gif_Task,LV_EVENT_SHORT_CLICKED,btn_inf);
            }
        }

    }
    if(closedir(dq)==-1)
    {
        perror("closedir ");
        return false;
    }
    return true;
}


void Dir_Btn_Task(lv_event_t *e )
{
    DBI_P btn_inf =(DBI_P)e->user_data;
    lv_obj_clean(btn_inf->UC_P->dir_ui_p->dir_list);
    char new_dir_name[256*2] = {0};
    strcpy(new_dir_name,btn_inf->dir_name);
    struct All_UI *UC_P = btn_inf->UC_P;


    //顺便把旧按钮对应的堆空间free调用 摧毁链表 剩下 头结点
    if(Destory_Dir_Btn_List(btn_inf->UC_P->dir_bin_inf_head) == false)
    {
        printf("摧毁链表失败！\n");
        return ;
    }
    btn_inf = NULL;  // 防止后续误用已释放的节点
    //再添加新按钮
    Show_Dir_List(new_dir_name, UC_P);
    //Show_Dir_List(btn_inf->dir_name,btn_inf->UC_P);
    return ;
}

DBI_P Create_Node()
{
    DBI_P new_node =(DBI_P)malloc(sizeof(DBI));
    if(new_node==(DBI_P)NULL)
    {
        perror("malloc new node ");
        return (DBI_P)-1;
    }
    memset(new_node,0,sizeof(DBI));

    new_node->next=new_node;
    new_node->prev=new_node;

    return new_node;
}

bool  Head_Add_Node(DBI_P head_node,DBI_P new_node)
{
    if(head_node == (DBI_P)NULL)
    {
        printf("头结点异常，无法添加！\n");
        return false;
    }

    new_node->next        = head_node->next;
    head_node->next->prev = new_node;
    new_node->prev        = head_node;
    head_node->next       = new_node;

    return true;
}

bool  Destory_Dir_Btn_List(DBI_P head_node)
{
    if(head_node == (DBI_P)NULL)
    {
        printf("头结点异常，无法摧毁！\n");
        return false;
    }

    
    while(head_node->next != head_node)
    {
        DBI_P free_node = head_node->next;

        free_node->next->prev = free_node->prev;
        free_node->prev->next = free_node->next;

        free_node->next = (DBI_P)NULL;
        free_node->prev = (DBI_P)NULL;

        free(free_node);
    }

    return true;
}

void File_Btn_Task(lv_event_t *e)
{
    DBI_P btn_inf =(DBI_P)e->user_data;

    lv_obj_add_flag(btn_inf->UC_P->dir_ui_p->dir_ui,LV_OBJ_FLAG_HIDDEN);

    btn_inf->UC_P->dir_ui_p->img_ui = lv_obj_create(NULL);
    lv_obj_t * img_win= lv_win_create(btn_inf->UC_P->dir_ui_p->img_ui,30);
    lv_obj_set_size(img_win,800,480);

    lv_obj_t * close_btn=lv_win_add_btn(img_win,LV_SYMBOL_CLOSE,30);
    lv_obj_add_event_cb(close_btn, Close_Img_Task, LV_EVENT_CLICKED, btn_inf->UC_P);

    // 获取窗口的内容容器
    lv_obj_t * content = lv_win_get_content(img_win);

    // 创建一个 img 控件，父对象是 content
    lv_obj_t * gif = lv_img_create(content);

    char img_file_path[256*2] = "\0"; //存放有卷标的路径
    sprintf(img_file_path,"S:%s",btn_inf->dir_name);

    // 设置图片路径
    lv_img_set_src(gif, img_file_path);

    // 在窗口内容区居中
    lv_obj_center(gif);

    lv_scr_load(btn_inf->UC_P->dir_ui_p->img_ui);
    return ;
}

void Close_Img_Task(lv_event_t * e)
{
    // 通过 user_data 直接拿 UC_P（因为我们在绑定时传的是 UC_P）
    struct All_UI * UC_P = (struct All_UI *)lv_event_get_user_data(e);
    if (UC_P == NULL || UC_P->dir_ui_p == NULL) return;

    // 1) 先恢复目录界面
    if (UC_P->dir_ui_p->dir_ui) {
        lv_obj_clear_flag(UC_P->dir_ui_p->dir_ui, LV_OBJ_FLAG_HIDDEN);
        lv_scr_load(UC_P->dir_ui_p->dir_ui);
    }

    // 2) 再“异步删除”图片界面，避免在事件处理栈里直接删父对象导致崩溃
    if (UC_P->dir_ui_p->img_ui) {
        lv_obj_del_async(UC_P->dir_ui_p->img_ui);
        UC_P->dir_ui_p->img_ui = NULL;
    }

}

void File_Btn_gif_Task(lv_event_t *e)
{
    DBI_P btn_inf = (DBI_P)e->user_data;

    // 隐藏目录界面
    lv_obj_add_flag(btn_inf->UC_P->dir_ui_p->dir_ui, LV_OBJ_FLAG_HIDDEN);

    // 新建一个屏幕承载窗口
    btn_inf->UC_P->dir_ui_p->img_ui = lv_obj_create(NULL);

    // 创建带标题栏的窗口
    lv_obj_t *img_win = lv_win_create(btn_inf->UC_P->dir_ui_p->img_ui, 30);
    lv_obj_set_size(img_win, 800, 480);

    // 关闭按钮
    lv_obj_t *close_btn = lv_win_add_btn(img_win, LV_SYMBOL_CLOSE, 30);
    lv_obj_add_event_cb(close_btn, Close_Img_Task, LV_EVENT_CLICKED, btn_inf->UC_P);

    // 关键：在窗口内容区创建 GIF，而不是直接挂在窗口本身
    lv_obj_t *content = lv_win_get_content(img_win);

    // 正确的 GIF 控件与 API
    lv_obj_t *gif = lv_gif_create(content);

    char img_file_path[256*2] = "";
    sprintf(img_file_path, "S:%s", btn_inf->dir_name);

    // 设置 GIF 源
    lv_gif_set_src(gif, img_file_path);

    // 在内容区居中
    lv_obj_center(gif);

    // 切到图片屏幕
    lv_scr_load(btn_inf->UC_P->dir_ui_p->img_ui);
}
