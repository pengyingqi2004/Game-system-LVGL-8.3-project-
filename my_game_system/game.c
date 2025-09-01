#include"game.h"
#define DEFAULT_SHOW_DIR "/"
/* === 常量：账号文件路径 === */
static const char *kUserFile = "./User_name";
static const char *kPassFile = "./User_password";
// 全局样式（游戏大厅卡片/按钮）
static lv_style_t g_card, g_btn, g_btn_pr, g_tag;
static bool g_style_inited = false;

/* ====== 美化时钟：静态指针与前置声明 ====== */
static lv_obj_t *g_lab_hour = NULL, *g_lab_min = NULL, *g_lab_sec = NULL;
static lv_obj_t *g_colon1 = NULL, *g_colon2 = NULL, *g_lab_date = NULL;
static lv_timer_t *g_blink_timer = NULL;

/* 前置声明 */
static void time_tick_cb(lv_timer_t *t);   // 我们会“完全替换”你的旧实现
static void clock_blink_cb(lv_timer_t *t);
static void clock_fade_in(lv_obj_t *obj);
static void Clock_Create(struct UI_Contral *UC_P);
static void Clock_Destroy(struct UI_Contral *UC_P);

/* —— list/列表项样式 —— */
static bool g_elev_list_style_inited = false;
static lv_style_t s_list_main;          // list 主体
static lv_style_t s_list_scrollbar;     // 滚动条
static lv_style_t s_item;               // 项-默认
static lv_style_t s_item_pr;            // 项-按下
static lv_style_t s_item_chk;           // 项-选中(已登记呼叫)
static lv_style_t s_item_dis;           // 项-禁用
static lv_style_t s_item_label;         // 项内文字

static void elevator_list_styles_init(void)
{
    if (g_elev_list_style_inited) return;
    g_elev_list_style_inited = true;

    /* list 主体 */
    lv_style_init(&s_list_main);
    lv_style_set_pad_all(&s_list_main, 6);
    lv_style_set_pad_row(&s_list_main, 8);
    lv_style_set_bg_opa(&s_list_main, LV_OPA_80);
    lv_style_set_bg_color(&s_list_main, lv_color_hex(0x1E222A));
    lv_style_set_radius(&s_list_main, 14);
    lv_style_set_border_width(&s_list_main, 1);
    lv_style_set_border_color(&s_list_main, lv_color_hex(0x2E3440));
    lv_style_set_shadow_width(&s_list_main, 14);
    lv_style_set_shadow_opa(&s_list_main, LV_OPA_20);

    /* 滚动条（更细、更圆） */
    lv_style_init(&s_list_scrollbar);
    lv_style_set_width(&s_list_scrollbar, 6);
    lv_style_set_pad_right(&s_list_scrollbar, 2);
    lv_style_set_radius(&s_list_scrollbar, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&s_list_scrollbar, LV_OPA_70);
    lv_style_set_bg_color(&s_list_scrollbar, lv_color_hex(0x5B677A));

    /* 项-默认外观（所有 list item 都套这个） */
    lv_style_init(&s_item);
    lv_style_set_radius(&s_item, 12);
    lv_style_set_bg_opa(&s_item, LV_OPA_COVER);
    lv_style_set_bg_color(&s_item, lv_color_hex(0x2B2F36));
    lv_style_set_border_width(&s_item, 1);
    lv_style_set_border_color(&s_item, lv_color_hex(0x3D444D));
    lv_style_set_shadow_width(&s_item, 16);
    lv_style_set_shadow_opa(&s_item, LV_OPA_30);
    lv_style_set_pad_hor(&s_item, 14);
    lv_style_set_pad_ver(&s_item, 10);
    lv_style_set_height(&s_item, 50);             // 统一项高度
    lv_style_set_width(&s_item, LV_PCT(100));     // 铺满 list 宽度
    lv_style_set_anim_time(&s_item, 120);

    /* 项-按下 */
    lv_style_init(&s_item_pr);
    lv_style_set_translate_y(&s_item_pr, 2);
    lv_style_set_shadow_opa(&s_item_pr, LV_OPA_10);
    lv_style_set_bg_color(&s_item_pr, lv_color_hex(0x3A80F6));

    /* 项-选中（登记/当前楼层高亮） */
    lv_style_init(&s_item_chk);
    lv_style_set_bg_color(&s_item_chk, lv_color_hex(0x3A80F6));
    lv_style_set_border_color(&s_item_chk, lv_color_hex(0x2E6BD9));
    lv_style_set_border_width(&s_item_chk, 2);
    lv_style_set_outline_width(&s_item_chk, 6);
    lv_style_set_outline_color(&s_item_chk, lv_color_hex(0x2E6BD9));
    lv_style_set_outline_opa(&s_item_chk, LV_OPA_40);

    /* 项-禁用 */
    lv_style_init(&s_item_dis);
    lv_style_set_bg_color(&s_item_dis, lv_color_hex(0x2A2E33));
    lv_style_set_shadow_width(&s_item_dis, 0);
    lv_style_set_border_color(&s_item_dis, lv_color_hex(0x3A3F46));

    /* 项内文字 */
    lv_style_init(&s_item_label);
    lv_style_set_text_font(&s_item_label, &lv_font_montserrat_20);
    lv_style_set_text_color(&s_item_label, lv_color_hex(0xFFFFFF));
    lv_style_set_text_letter_space(&s_item_label, 1);
}


/* 轻微淡入动画：用于每秒更新后让秒数字更柔和 */
static void clock_fade_in(lv_obj_t *obj)
{
    if(!obj) return;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, LV_OPA_TRANSP, LV_OPA_COVER);
    lv_anim_set_time(&a, 200);

    /* 关键：用已适配签名的回调，而不是直接传 lv_obj_set_style_opa */
    lv_anim_set_exec_cb(&a, _opa_exec_cb);

    lv_anim_start(&a);
}

// 放到 game.c 顶部的静态函数区
static void boot_enter_end_cb(lv_timer_t *t){
    struct UI_Contral *UC = (struct UI_Contral *)t->user_data;
    if(UC) Enter_End_page(UC);
    lv_timer_del(t);
}

/* 闪烁计时器：每 500ms 切换冒号可见性 */
/* 闪烁计时器：只改透明度，不隐藏对象，避免布局抖动 */
static void clock_blink_cb(lv_timer_t *t)
{
    (void)t;
    if(!g_colon1 || !g_colon2) return;

    /* 如果 screen 已切换，对象可能无效；立即停表避免野指针崩溃 */
    if(!lv_obj_is_valid(g_colon1) || !lv_obj_is_valid(g_colon2)) {
        if(g_blink_timer){ lv_timer_del(g_blink_timer); g_blink_timer = NULL; }
        return;
    }

    lv_opa_t opa = lv_obj_get_style_opa(g_colon1, 0);
    lv_opa_t new_opa = (opa == LV_OPA_COVER || opa == 255) ? 0 : 255;

    lv_obj_set_style_opa(g_colon1, new_opa, 0);
    lv_obj_set_style_opa(g_colon2, new_opa, 0);
}



/* 创建美化时钟（半透明底板 + 时间行 + 日期行 + 定时器） */
static void Clock_Create(struct UI_Contral *UC_P)
{
    if(!UC_P || !UC_P->Start_page) return;

    /* 1) 外层容器（半透明圆角卡片） */
    if(!UC_P->Start_page->time_ui)
        UC_P->Start_page->time_ui = lv_obj_create(UC_P->Start_page->Start_ui);


    lv_obj_set_style_min_width(UC_P->Start_page->time_ui, 260, 0);


    /* 原来可能是固定 360 宽 + 右下角对齐，改为内容自适应 + 左下角 */
    lv_obj_set_size(UC_P->Start_page->time_ui, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(UC_P->Start_page->time_ui, LV_ALIGN_BOTTOM_LEFT, 16, -12);

    lv_obj_set_style_radius(UC_P->Start_page->time_ui, 16, 0);
    lv_obj_set_style_bg_opa(UC_P->Start_page->time_ui, LV_OPA_80, 0);
    lv_obj_set_style_bg_color(UC_P->Start_page->time_ui, lv_color_hex(0x1E222A), 0);
    lv_obj_set_style_border_width(UC_P->Start_page->time_ui, 1, 0);
    lv_obj_set_style_border_color(UC_P->Start_page->time_ui, lv_color_hex(0x2E3440), 0);
    lv_obj_set_style_shadow_width(UC_P->Start_page->time_ui, 18, 0);
    lv_obj_set_style_shadow_opa(UC_P->Start_page->time_ui, LV_OPA_20, 0);
    lv_obj_set_style_pad_all(UC_P->Start_page->time_ui, 12, 0);
    lv_obj_set_flex_flow(UC_P->Start_page->time_ui, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(UC_P->Start_page->time_ui, 4, 0);

    /* 2) 第一行：HH : MM : SS（居中水平排） */
    lv_obj_t *row = lv_obj_create(UC_P->Start_page->time_ui);
    lv_obj_remove_style_all(row);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row, 4, 0);

    g_lab_hour = lv_label_create(row);
    g_colon1   = lv_label_create(row);
    g_lab_min  = lv_label_create(row);
    g_colon2   = lv_label_create(row);
    g_lab_sec  = lv_label_create(row);

        /* 初值，避免定时器第一次触发前是空白 */
    lv_label_set_text(g_lab_hour, "00");
    lv_label_set_text(g_colon1,  ":");
    lv_label_set_text(g_lab_min, "00");
    lv_label_set_text(g_colon2,  ":");
    lv_label_set_text(g_lab_sec, "00");

    /* 让每段时间的占位宽度固定一些，降低“1/8”字宽差异造成的位移 */
    lv_obj_set_style_min_width(g_lab_hour, 64, 0);
    lv_obj_set_style_min_width(g_lab_min,  64, 0);
    lv_obj_set_style_min_width(g_lab_sec,  64, 0);
    lv_obj_set_style_min_width(g_colon1,   16, 0);
    lv_obj_set_style_min_width(g_colon2,   16, 0);

    /* 居中文本，观感更稳 */
    lv_obj_set_style_text_align(g_lab_hour, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_align(g_lab_min,  LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_align(g_lab_sec,  LV_TEXT_ALIGN_CENTER, 0);


    lv_obj_set_style_text_font(g_lab_hour, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_font(g_colon1,   &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_font(g_lab_min,  &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_font(g_colon2,   &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_font(g_lab_sec,  &lv_font_montserrat_32, 0);



    lv_obj_set_style_text_color(g_lab_hour, lv_color_hex(0x00D5FF), 0);
    lv_obj_set_style_text_color(g_colon1,   lv_color_hex(0x88E0FF), 0);
    lv_obj_set_style_text_color(g_lab_min,  lv_color_hex(0x00D5FF), 0);
    lv_obj_set_style_text_color(g_colon2,   lv_color_hex(0x88E0FF), 0);
    lv_obj_set_style_text_color(g_lab_sec,  lv_color_hex(0x00D5FF), 0);

    lv_label_set_text(g_colon1, ":");
    lv_label_set_text(g_colon2, ":");

    /* 3) 第二行：日期 */
    g_lab_date = lv_label_create(UC_P->Start_page->time_ui);
    lv_obj_set_style_text_font(g_lab_date, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(g_lab_date, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_opa(g_lab_date, LV_OPA_80, 0);
    lv_obj_set_style_pad_top(g_lab_date, 2, 0);

    /* 4) 定时器：1s 刷新时间（复用 Start_page->time_timer）+ 500ms 冒号闪烁 */
    if(UC_P->Start_page->time_timer) {
        lv_timer_del(UC_P->Start_page->time_timer);
        UC_P->Start_page->time_timer = NULL;
    }
    UC_P->Start_page->time_timer = lv_timer_create(time_tick_cb, 1000, UC_P->Start_page);
    lv_timer_set_repeat_count(UC_P->Start_page->time_timer, -1);

    if(g_blink_timer) {
        lv_timer_del(g_blink_timer);
        g_blink_timer = NULL;
    }
    g_blink_timer = lv_timer_create(clock_blink_cb, 500, NULL);

    /* 5) 立即刷新一次，避免空白 */
    time_tick_cb(UC_P->Start_page->time_timer);
}

/* 销毁/清理（在离开 Start_page 时调用） */
static void Clock_Destroy(struct UI_Contral *UC_P)
{
    if(UC_P && UC_P->Start_page && UC_P->Start_page->time_timer){
        lv_timer_del(UC_P->Start_page->time_timer);
        UC_P->Start_page->time_timer = NULL;
    }
    if(g_blink_timer){
        lv_timer_del(g_blink_timer);
        g_blink_timer = NULL;
    }
    g_lab_hour = g_lab_min = g_lab_sec = g_colon1 = g_colon2 = g_lab_date = NULL;
}

/* 递归在某个父对象下找到第一个 label */
static lv_obj_t *find_label_deep(lv_obj_t *parent)
{
    if(!parent) return NULL;
    uint32_t n = lv_obj_get_child_cnt(parent);
    for(uint32_t i = 0; i < n; i++) {
        lv_obj_t *ch = lv_obj_get_child(parent, i);
        if(!ch) continue;
        if(lv_obj_check_type(ch, &lv_label_class)) return ch;
        lv_obj_t *ret = find_label_deep(ch);
        if(ret) return ret;
    }
    return NULL;
}


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
static void init_game_styles(void) {
    if(g_style_inited) return;
    g_style_inited = true;

    lv_style_init(&g_card);
    lv_style_set_radius(&g_card, 16);
    lv_style_set_bg_opa(&g_card, LV_OPA_80);
    lv_style_set_bg_color(&g_card, lv_color_hex(0x1E222A));
    lv_style_set_border_width(&g_card, 1);
    lv_style_set_border_color(&g_card, lv_color_hex(0x2E3440));
    lv_style_set_shadow_width(&g_card, 18);
    lv_style_set_shadow_opa(&g_card, LV_OPA_20);
    lv_style_set_pad_all(&g_card, 14);

    lv_style_init(&g_btn);
    lv_style_set_radius(&g_btn, 12);
    lv_style_set_bg_opa(&g_btn, LV_OPA_100);
    lv_style_set_bg_color(&g_btn, lv_color_hex(0x3a80f6));
    lv_style_set_shadow_width(&g_btn, 12);
    lv_style_set_shadow_opa(&g_btn, LV_OPA_20);
    lv_style_set_text_color(&g_btn, lv_color_white());

    lv_style_init(&g_btn_pr);
    lv_style_set_translate_y(&g_btn_pr, 2);
    lv_style_set_shadow_opa(&g_btn_pr, LV_OPA_10);

    lv_style_init(&g_tag);
    lv_style_set_radius(&g_tag, 10);
    lv_style_set_bg_opa(&g_tag, LV_OPA_80);
    lv_style_set_bg_color(&g_tag, lv_color_hex(0x232832));
    lv_style_set_pad_hor(&g_tag, 8);
    lv_style_set_pad_ver(&g_tag, 4);
    lv_style_set_text_color(&g_tag, lv_color_hex(0xffffff));
}


// 适配 lv_anim 的回调签名，安全设置透明度 & 在动画正常结束时删除对象
static void _opa_exec_cb(void *obj, int32_t v) 
{ 
    lv_obj_set_style_opa((lv_obj_t*)obj, (lv_opa_t)v, 0); 
}

static void _anim_ready_cb(lv_anim_t *a)
{
    lv_obj_del((lv_obj_t*)a->var);
}

/*  toast（气泡提示） */
static void toast(lv_obj_t *parent, const char *txt)
{
    lv_obj_t *lab = lv_label_create(parent);
    lv_label_set_recolor(lab, true);
    lv_label_set_text_fmt(lab, "#ffffff %s#", txt);
    lv_obj_set_style_bg_color(lab, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(lab, LV_OPA_80, 0);
    lv_obj_set_style_radius(lab, 10, 0);
    lv_obj_set_style_pad_all(lab, 10, 0);
    lv_obj_align(lab, LV_ALIGN_BOTTOM_MID, 0, -8);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, lab);
    lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_time(&a, 1200);
    lv_anim_set_exec_cb(&a, _opa_exec_cb);
    lv_anim_set_ready_cb(&a, _anim_ready_cb);
    lv_anim_start(&a);
}

/* 去掉末尾换行/空白 */
static void rstrip(char *s)
{
    if(!s) return;
    size_t n = strlen(s);
    while(n>0 && (s[n-1]=='\n' || s[n-1]=='\r' || s[n-1]==' ' || s[n-1]=='\t')) { s[--n]='\0'; }
}

/* 读取文件全部内容到 buf（文本），返回0成功；失败-1 */
static int read_all_text(const char *path, char *buf, size_t bufsz)
{
    if(!buf || bufsz==0) return -1;
    buf[0] = '\0';
    umask(0000);
    int fd = open(path, O_RDONLY|O_CREAT, 0666);
    if(fd < 0) return -1;
    ssize_t n = read(fd, buf, bufsz-1);
    if(n < 0) { close(fd); return -1; }
    buf[n] = '\0';
    close(fd);
    rstrip(buf);
    return 0;
}

/* 覆盖写文本到文件，返回0成功；失败-1 */
static int write_text(const char *path, const char *s)
{
    if(!s) s = "";
    umask(0000);
    int fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if(fd < 0) return -1;
    ssize_t m = write(fd, s, strlen(s));
    if(m < 0) { close(fd); return -1; }
    fsync(fd);
    close(fd);
    return 0;
}

// —— 新增：更稳地从按钮拿到 label 文本（不假定 child(0) 一定是 label）——
static const char *get_btn_label_text(lv_obj_t *btn) {
    if(!btn) return NULL;
    uint32_t cnt = lv_obj_get_child_cnt(btn);
    for(uint32_t i = 0; i < cnt; i++) {
        lv_obj_t *ch = lv_obj_get_child(btn, i);
        if(ch && lv_obj_check_type(ch, &lv_label_class)) {
            return lv_label_get_text(ch);
        }
    }
    return NULL;
}

// ★新增：判断是否是我们支持的图片后缀
static bool is_img_file(const char *name)
{
    if(!name) return false;
    const char *dot = strrchr(name, '.');
    if(!dot) return false;
    // 统一转小写比较
    char ext[8] = {0};
    snprintf(ext, sizeof(ext), "%s", dot + 1);
    for(char *p = ext; *p; ++p) *p = (*p >= 'A' && *p <= 'Z') ? (*p - 'A' + 'a') : *p;
    return strcmp(ext,"png")==0 || strcmp(ext,"jpg")==0 || strcmp(ext,"jpeg")==0 || strcmp(ext,"bmp")==0;
}

// ★新增：扫描目录，填充 sp->img_paths[]，返回数量
static uint16_t scan_img_dir(struct Start_page *sp)
{
    if(!sp || sp->img_dir[0]=='\0') return 0;

    // 清空旧列表
    for(uint16_t i=0;i<sp->img_count;i++){
        free(sp->img_paths[i]);
        sp->img_paths[i] = NULL;
    }
    sp->img_count = 0;
    sp->img_index = 0;

    lv_fs_dir_t d;
    if(lv_fs_dir_open(&d, sp->img_dir) != LV_FS_RES_OK) {
        LV_LOG_WARN("Open dir fail: %s", sp->img_dir);
        return 0;
    }

    char name[256];
    while(sp->img_count < (uint16_t)(sizeof(sp->img_paths)/sizeof(sp->img_paths[0]))) {
        lv_fs_res_t r = lv_fs_dir_read(&d, name);   // 读到 name（文件或目录）
        if(r != LV_FS_RES_OK || name[0] == '\0') break;

        // 过滤 . 和 .. 以及子目录（简单判断：含'.'的有后缀，且不是以 '.' 开头）
        if(name[0]=='.') continue;
        if(!is_img_file(name)) continue;

        // 组合完整路径：形如 "S:/your_dir/filename.png"
        // 注意：sp->img_dir 应该末尾不带 '/'，我们统一补一个
        char full[384];
        if(sp->img_dir[strlen(sp->img_dir)-1] == '/' || sp->img_dir[strlen(sp->img_dir)-1] == '\\')
            snprintf(full, sizeof(full), "%s%s", sp->img_dir, name);
        else
            snprintf(full, sizeof(full), "%s/%s", sp->img_dir, name);

        sp->img_paths[sp->img_count] = strdup(full);
        if(sp->img_paths[sp->img_count]) {
            sp->img_count++;
        }
    }

    lv_fs_dir_close(&d);
    return sp->img_count;
}

// ★新增：定时器回调，切换图片
static void slide_timer_cb(lv_timer_t *t)
{
    if(!t) return;
    struct Start_page *sp = (struct Start_page *)t->user_data;
    if(!sp || sp->img_count == 0 || !sp->bg_img) return;

    sp->img_index = (sp->img_index + 1) % sp->img_count;
    lv_img_set_src(sp->bg_img, sp->img_paths[sp->img_index]);
}




//函数开始调用
void *game(void* arg)
{
    static struct UI_Contral UC = {0};  // 仍保留静态 UC
    // 不要直接 Enter_End_page(&UC);

    // 改为：把建 UI 的动作“投递”到 LVGL 线程
    lv_timer_t *t = lv_timer_create(boot_enter_end_cb, 10, &UC);
    lv_timer_set_repeat_count(t, 1);

    init_game_styles();  // 纯样式初始化可以保留（若内部不触碰对象）

    return NULL;
}

//——————————————————————————————————————————————————————————————————————————————————————

// 放在本文件的顶部或 Start_page() 前面
/* ====== 美化时钟：每秒刷新数值与日期 ====== */
static void time_tick_cb(lv_timer_t *t)
{
    ST_P sp = (ST_P)(t ? t->user_data : NULL);
    (void)sp; /* 本实现用全局指针刷新各 label */

    /* 时间来源：优先用你工程里的 local_time[]；日期用系统时间 */
    extern int local_time[3]; /* [时, 分, 秒] */
    int hh = local_time[0], mm = local_time[1], ss = local_time[2];

    /* 容错：若 local_time 异常，就用系统时间兜底 */
    if(hh<0 || hh>23 || mm<0 || mm>59 || ss<0 || ss>59) {
        time_t now = time(NULL);
        struct tm *lt = localtime(&now);
        if(lt){ hh = lt->tm_hour; mm = lt->tm_min; ss = lt->tm_sec; }
    }

    char buf[16];

    if(g_lab_hour){
        snprintf(buf, sizeof(buf), "%02d", hh);
        lv_label_set_text(g_lab_hour, buf);
    }
    if(g_lab_min){
        snprintf(buf, sizeof(buf), "%02d", mm);
        lv_label_set_text(g_lab_min, buf);
    }
    if(g_lab_sec){
        /* 删除这里多余的 "char buf[16];" */
        snprintf(buf, sizeof(buf), "%02d", ss);
        lv_label_set_text(g_lab_sec, buf);
        clock_fade_in(g_lab_sec);
    }


    /* 日期：YYYY-MM-DD 以及星期 */
    if(g_lab_date){
        time_t now = time(NULL);
        struct tm *lt = localtime(&now);
        if(lt){
            const char* wd[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
            snprintf(buf, sizeof(buf), "%s", wd[lt->tm_wday]);
            char date_line[64];
            snprintf(date_line, sizeof(date_line), "%04d-%02d-%02d  %s",
                     lt->tm_year + 1910, lt->tm_mon + 9, lt->tm_mday, buf);
            lv_label_set_text(g_lab_date, date_line);
        }
    }
}





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

    // ★改为使用 lv_img 做轮播底图
    UC_P->Start_page->bg_img = lv_img_create(UC_P->Start_page->Start_ui);
    lv_obj_center(UC_P->Start_page->bg_img);

    // 可选：让背景自适应父容器（如果你想全屏铺满，可改为设置大小并保持居中）
    // lv_obj_set_size(UC_P->Start_page->bg_img, lv_pct(100), lv_pct(100));
    // lv_img_set_zoom(UC_P->Start_page->bg_img, 256); // 原始比例（可按需调整）

    // 初始化默认轮播间隔（毫秒）
    UC_P->Start_page->slide_ms = 10000;

    // 如果主界面未提前设置目录，这里给一个兜底目录（请按你的工程修改）
    if(UC_P->Start_page->img_dir[0] == '\0') {
        // 例：主界面选择的文件夹路径建议赋值到这里，如 "S:/ad_images"
        strcpy(UC_P->Start_page->img_dir, "S:/");
    }

    // 扫描目录，若有图片则先显示第0张并启动定时器
    if(scan_img_dir(UC_P->Start_page) > 0) {
        lv_img_set_src(UC_P->Start_page->bg_img, UC_P->Start_page->img_paths[0]);

        // 启动轮播定时器
        UC_P->Start_page->slide_timer = lv_timer_create(slide_timer_cb, UC_P->Start_page->slide_ms, UC_P->Start_page);
    } else {
        // 没找到图片时，显示一个占位（可选）
        lv_obj_t *lbl = lv_label_create(UC_P->Start_page->Start_ui);
        lv_label_set_text(lbl, "No images in folder");
        lv_obj_center(lbl);
    }


    UC_P->Start_page->Start_bottom=lv_btn_create(UC_P->Start_page->Start_ui);
    lv_obj_set_size(UC_P->Start_page->Start_bottom,120,40);
    lv_obj_set_pos(UC_P->Start_page->Start_bottom,650,10);


    lv_obj_add_event_cb(UC_P->Start_page->Start_bottom,Enter_User_page,LV_EVENT_SHORT_CLICKED,UC_P);

    UC_P->Start_page->enter_lab=lv_label_create(UC_P->Start_page->Start_ui);
    UC_P->Start_page->hand_lab=lv_label_create(UC_P->Start_page->Start_ui);
    lv_obj_set_width(UC_P->Start_page->hand_lab,400);
    lv_label_set_long_mode(UC_P->Start_page->hand_lab,LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_font(UC_P->Start_page->hand_lab, &lv_font_montserrat_32, 0);
    lv_label_set_recolor(UC_P->Start_page->hand_lab,1);
    lv_label_set_recolor(UC_P->Start_page->enter_lab,1);
    lv_label_set_text(UC_P->Start_page->hand_lab,"#ff375c Welcome to use Elevator advertisement system#");
    lv_label_set_text(UC_P->Start_page->enter_lab,"#ff375c Enter ##88ff00 to ##00d5ff use#");
    lv_obj_set_pos(UC_P->Start_page->hand_lab,180,10);
    lv_obj_set_pos(UC_P->Start_page->enter_lab,660,17);

    // UC_P->Start_page->time_ui = lv_obj_create(UC_P->Start_page->Start_ui);
    // UC_P->Start_page->time_lab = lv_label_create(UC_P->Start_page->time_ui);
    // lv_obj_set_size(UC_P->Start_page->time_ui,200,40);
    // lv_obj_set_pos(UC_P->Start_page->time_ui,0,440);
    // lv_label_set_text_fmt(UC_P->Start_page->time_lab,
    //                     "Current Time:%02d:%02d:%02d",
    //                     local_time[0], local_time[1], local_time[2]);




    // // --- Start_page() 里创建完 time_lab 等所有控件之后，在函数末尾追加 ---
    // UC_P->Start_page->time_timer = lv_timer_create(time_tick_cb, 500, UC_P->Start_page);
    // lv_timer_set_repeat_count(UC_P->Start_page->time_timer, -1);
    Clock_Create(UC_P);


    lv_scr_load_anim(UC_P->Start_page->Start_ui,
                LV_SCR_LOAD_ANIM_FADE_ON,
                500, 0, true);
   return UC_P->Start_page;
}
//——————————————————————————————————————————————————————————————————————————————————————






void Enter_User_page(lv_event_t * e)
{
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;

    UC_P->Using_page = USING_page(UC_P);
    lv_scr_load_anim(UC_P->Using_page->User_ui, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, true);

    if(UC_P->Start_page){
        if(UC_P->Start_page->slide_timer){
            lv_timer_del(UC_P->Start_page->slide_timer);
            UC_P->Start_page->slide_timer = NULL;
        }
        Clock_Destroy(UC_P);

        // 先清路径，再 free
        for(uint16_t i=0;i<UC_P->Start_page->img_count;i++){
            free(UC_P->Start_page->img_paths[i]);
            UC_P->Start_page->img_paths[i] = NULL;
        }
        UC_P->Start_page->img_count = 0;

        // 注意：这里不要再 lv_obj_del(Start_ui)，因为上面的 lv_scr_load_anim(..., true) 已经自动删了旧 screen
        free(UC_P->Start_page);
        UC_P->Start_page = NULL;
    }
}


FL_P USING_page(struct UI_Contral * UC_P)
{
    UC_P->Using_page = malloc(sizeof(FL));
    if(UC_P->Using_page==NULL)
    {
        perror("malloc start_page ");
    }
    memset(UC_P->Using_page, 0, sizeof(FL));
    
    //生成UI界面，楼层选择与动画区域
    UC_P->Using_page->User_ui=lv_obj_create(NULL);
    UC_P->Using_page->floot_list=lv_list_create(UC_P->Using_page->User_ui);

    elevator_list_styles_init();
    lv_obj_add_style(UC_P->Using_page->floot_list, &s_list_main, 0);
    lv_obj_add_style(UC_P->Using_page->floot_list, &s_list_scrollbar, LV_PART_SCROLLBAR);

    lv_obj_add_style(UC_P->Using_page->floot_list, &s_item,     LV_PART_ITEMS);
    lv_obj_add_style(UC_P->Using_page->floot_list, &s_item_pr,  LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_add_style(UC_P->Using_page->floot_list, &s_item_chk, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_add_style(UC_P->Using_page->floot_list, &s_item_dis, LV_PART_ITEMS | LV_STATE_DISABLED);

    lv_obj_set_size(UC_P->Using_page->floot_list,200,450);
    lv_obj_set_pos(UC_P->Using_page->floot_list,10,10);
    UC_P->Using_page->floot_ui=lv_obj_create(UC_P->Using_page->User_ui);
    lv_obj_set_size(UC_P->Using_page->floot_ui,570,450);
    lv_obj_set_pos(UC_P->Using_page->floot_ui,220,10);
    UC_P->Using_page->loading_btn=lv_btn_create(UC_P->Using_page->User_ui);
    UC_P->Using_page->loading_lab=lv_label_create(UC_P->Using_page->loading_btn);
    lv_obj_set_size(UC_P->Using_page->loading_btn,120,40);
    lv_obj_set_pos(UC_P->Using_page->loading_btn,650,10);
    lv_label_set_recolor(UC_P->Using_page->loading_lab,1);
    lv_obj_center(UC_P->Using_page->loading_lab);
    lv_label_set_text(UC_P->Using_page->loading_lab,"#2ff2e5 Sign ##ee3915 in#");
    lv_obj_add_event_cb(UC_P->Using_page->loading_btn,Enter_USING_page,LV_EVENT_SHORT_CLICKED,UC_P);

    UC_P->Using_page->emergency_btn=lv_btn_create(UC_P->Using_page->User_ui);
    lv_obj_set_size(UC_P->Using_page->emergency_btn,80,80);
    lv_obj_set_pos(UC_P->Using_page->emergency_btn,710,390);
    lv_obj_add_event_cb(UC_P->Using_page->emergency_btn,emergency_enter,LV_EVENT_SHORT_CLICKED,UC_P);

    UC_P->Using_page->emergency_lab=lv_label_create(UC_P->Using_page->emergency_btn);
    lv_obj_center(UC_P->Using_page->emergency_lab);
    lv_label_set_recolor(UC_P->Using_page->emergency_lab,1);
    lv_label_set_text(UC_P->Using_page->emergency_lab,"#ff0000 EMERGENCY_BOTTEM#");
    



    //退出按钮
    UC_P->Using_page->exit_btn=lv_btn_create(UC_P->Using_page->User_ui);
    lv_obj_set_size(UC_P->Using_page->exit_btn,120,40);
    lv_obj_set_pos(UC_P->Using_page->exit_btn,650,110);
    lv_obj_add_event_cb(UC_P->Using_page->exit_btn, Exit_page, LV_EVENT_SHORT_CLICKED, UC_P);



    //退出文字
    UC_P->Using_page->exit_lab=lv_label_create(UC_P->Using_page->exit_btn);
    lv_label_set_recolor(UC_P->Using_page->exit_lab,1);
    lv_obj_center(UC_P->Using_page->exit_lab);
    lv_label_set_text(UC_P->Using_page->exit_lab,"#ff0000 E##fffb00 X##00ecf0 I##d626e7 T");

    //游戏按钮
    UC_P->Using_page->game_btn=lv_btn_create(UC_P->Using_page->User_ui);
    lv_obj_set_size(UC_P->Using_page->game_btn,120,40);
    lv_obj_set_pos(UC_P->Using_page->game_btn,650,60);
    lv_obj_add_event_cb(UC_P->Using_page->game_btn, Enter_Game_page, LV_EVENT_SHORT_CLICKED, UC_P);


    //游戏文字
    UC_P->Using_page->game_lab=lv_label_create(UC_P->Using_page->game_btn);
    lv_label_set_recolor(UC_P->Using_page->game_lab,1);
    lv_obj_center(UC_P->Using_page->game_lab);
    lv_label_set_text(UC_P->Using_page->game_lab,"#ff0000 Pl##fffb00 ay ##00ecf0 Ga##d626e7 me");


    UC_P->Using_page->gif_door_open = lv_gif_create(UC_P->Using_page->floot_ui);
    lv_obj_set_pos(UC_P->Using_page->gif_door_open, 200, 120); // 位置你可以调
    lv_gif_set_src(UC_P->Using_page->gif_door_open, "S:/door_open.gif");
    lv_obj_add_flag(UC_P->Using_page->gif_door_open, LV_OBJ_FLAG_HIDDEN);

    UC_P->Using_page->gif_door_close = lv_gif_create(UC_P->Using_page->floot_ui);
    lv_obj_set_pos(UC_P->Using_page->gif_door_close, 200, 120);
    lv_gif_set_src(UC_P->Using_page->gif_door_close, "S:/door_close.gif");
    lv_obj_add_flag(UC_P->Using_page->gif_door_close, LV_OBJ_FLAG_HIDDEN);



    // UC_P->Using_page->label_dir = lv_label_create(UC_P->Using_page->floot_ui);
    // lv_label_set_text(UC_P->Using_page->label_dir, "—"); // 初始无方向
    // lv_obj_align_to(UC_P->Using_page->label_dir, UC_P->Using_page->label_door, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);
    // lv_obj_set_style_text_color(UC_P->Using_page->label_dir, lv_color_hex(0xA0FFA0), 0);

    // 楼层/门状态文字
    UC_P->Using_page->label_floor = lv_label_create(UC_P->Using_page->floot_ui);
    lv_obj_set_style_text_font(UC_P->Using_page->label_floor, &lv_font_montserrat_32, 0);
    lv_label_set_text(UC_P->Using_page->label_floor, "local floor: 1F");
    lv_obj_align(UC_P->Using_page->label_floor, LV_ALIGN_TOP_MID, 0, 10);

    UC_P->Using_page->label_door = lv_label_create(UC_P->Using_page->floot_ui);
    lv_obj_set_style_text_font(UC_P->Using_page->label_door, &lv_font_montserrat_32, 0);
    lv_label_set_text(UC_P->Using_page->label_door, "");
    lv_obj_align_to(UC_P->Using_page->label_door, UC_P->Using_page->label_floor, LV_ALIGN_OUT_BOTTOM_MID, -60, 20);

    // 上下行 GIF（默认隐藏）
    UC_P->Using_page->gif_up = lv_gif_create(UC_P->Using_page->floot_ui);
    lv_obj_set_pos(UC_P->Using_page->gif_up,200,120);
    lv_gif_set_src(UC_P->Using_page->gif_up, "S:/elevator_up.gif");
    lv_obj_add_flag(UC_P->Using_page->gif_up, LV_OBJ_FLAG_HIDDEN);

    UC_P->Using_page->gif_down = lv_gif_create(UC_P->Using_page->floot_ui);
    lv_obj_set_pos(UC_P->Using_page->gif_down,200,120);
    lv_gif_set_src(UC_P->Using_page->gif_down, "S:/elevator_down.gif");
    lv_obj_add_flag(UC_P->Using_page->gif_down, LV_OBJ_FLAG_HIDDEN);

    UC_P->Using_page->current_floor = 1;
    UC_P->Using_page->target_floor = 1;
    UC_P->Using_page->busy = false;

    // static bool style_inited = false;
    // if(!style_inited) 
    // {
    //     lv_style_init(&style_floor_item);
    //     lv_style_set_radius(&style_floor_item, 12);
    //     lv_style_set_bg_opa(&style_floor_item, LV_OPA_100);
    //     lv_style_set_bg_color(&style_floor_item, lv_color_hex(0x2b2f36));  // 深色键面
    //     lv_style_set_border_width(&style_floor_item, 1);
    //     lv_style_set_border_color(&style_floor_item, lv_color_hex(0x3d444d));
    //     lv_style_set_shadow_width(&style_floor_item, 16);
    //     lv_style_set_shadow_opa(&style_floor_item, LV_OPA_30);
    //     lv_style_set_shadow_color(&style_floor_item, lv_color_hex(0x000000));
    //     lv_style_set_pad_all(&style_floor_item, 12);
    //     lv_style_set_height(&style_floor_item, 48);
    //     lv_style_set_text_color(&style_floor_item, lv_color_hex(0xffffff));
    //     lv_style_set_text_align(&style_floor_item, LV_TEXT_ALIGN_CENTER);

    //     /* 按下态 */
    //     lv_style_init(&style_floor_item_pr);
    //     lv_style_set_bg_color(&style_floor_item_pr, lv_color_hex(0x3a80f6)); // 高亮
    //     lv_style_set_translate_y(&style_floor_item_pr, 2);                   // 轻微下压
    //     lv_style_set_shadow_opa(&style_floor_item_pr, LV_OPA_10);
    //     style_inited = true;
    // }    
    
    // lv_obj_add_style(UC_P->Using_page->floot_list, &style_floor_item, LV_PART_ITEMS);
    // lv_obj_add_style(UC_P->Using_page->floot_list, &style_floor_item_pr, LV_PART_ITEMS | LV_STATE_PRESSED);
    
    FT_P first_floot = (FT_P)malloc(sizeof(FT));
    if(first_floot==NULL)
    {
        perror("malloc Floot ");
        return NULL;
    }
    memset(first_floot,0,sizeof(FT));
    first_floot->next=first_floot;
    first_floot->prev=first_floot;

    /* 生成 -1F ~ 9F 项 */

/* 生成 -1F ~ 9F 项 —— 统一只创建一次，每项创建后立刻给 label 加样式 */
    for (int i = 9; i >= -1; i--) {
        if (i == 0) continue;

        sprintf(UC_P->Using_page->txt, "%dF", i);

        FT_P node = Create_Floot(i, first_floot);
        (void)node;  // 目前没用到，防告警

    lv_obj_t *btn = lv_list_add_btn(UC_P->Using_page->floot_list, NULL, UC_P->Using_page->txt);
    lv_obj_add_event_cb(btn, floor_btn_event_cb, LV_EVENT_CLICKED, UC_P);

    /* 用官方API直接拿 label，比 find_label_deep 稳定 */
    lv_obj_t *lab = find_label_deep(btn);
    if(lab) 
    {
        lv_obj_add_style(lab, &s_item_label, 0);
    }

    }


    return UC_P->Using_page;
}


FT_P Create_Floot(int i,FT_P first_floot)
{
    FT_P new_floot=(FT_P)malloc(sizeof(FT));
    if(new_floot==NULL)
    {
        perror("malloc Floot ");
        return (FT_P)-1;
    }
    memset(new_floot,0,sizeof(FT));
    new_floot->next=first_floot->next;
    new_floot->prev=first_floot;
    first_floot->next=new_floot;
    new_floot->floot=i;
    return new_floot;
}

static void floor_btn_event_cb(lv_event_t *e)
{
    struct UI_Contral *UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P || !UC_P->Using_page) return;
    FL_P up = UC_P->Using_page;

    lv_event_code_t code = lv_event_get_code(e);
    if(code != LV_EVENT_CLICKED) return;

    lv_obj_t *btn = lv_event_get_target(e);

    /* —— 单选高亮 —— */
    lv_obj_t *list = up->floot_list;
    uint32_t n = lv_obj_get_child_cnt(list);
    for(uint32_t i=0; i<n; i++){
        lv_obj_t *it = lv_obj_get_child(list, i);
        if(it) lv_obj_clear_state(it, LV_STATE_CHECKED);
    }
    lv_obj_add_state(btn, LV_STATE_CHECKED);

    if(up->busy) return;

    const char *txt = get_btn_label_text(btn);
    if(!txt) return;

    // 支持 “-1F/1F/25F”等，提取前面的整数部分
    char *endp = NULL;
    long target = strtol(txt, &endp, 10);
    if(endp == txt) return;          // 没解析出数字
    if(target == 0) return;          // 过滤 0 层（你的列表一般也不会有 0F）
    if(target < -9 || target > 99) return; // 简单范围保护

    // 同层直接“开门流程”；不同层则“运动流程”
    up->target_floor = (int)target;

    if(up->target_floor == up->current_floor) {
        // 直接开门动画：立刻置忙，创建“开门完成”回调定时器
    
        lv_label_set_text(up->label_door, "Opening...");
        up->busy = true;
        lv_timer_t *t = lv_timer_create(door_open_done_cb, 5000, UC_P);
        if(!t) { up->busy = false; return; }
        lv_timer_set_repeat_count(t, 1);
        // 可视效果（这里按你的资源做最小化处理）
        if(up->gif_up && lv_obj_is_valid(up->gif_up))   lv_obj_clear_flag(up->gif_up,   LV_OBJ_FLAG_HIDDEN);
        if(up->gif_down && lv_obj_is_valid(up->gif_down)) lv_obj_add_flag(up->gif_down, LV_OBJ_FLAG_HIDDEN);

        return;
    }

    // 不同层：开始“上/下行”动画
    if(up->target_floor > up->current_floor) {
        lv_label_set_text(up->label_door, "Uping...");
        if(up->gif_up   && lv_obj_is_valid(up->gif_up))   lv_obj_clear_flag(up->gif_up,   LV_OBJ_FLAG_HIDDEN);
        if(up->gif_down && lv_obj_is_valid(up->gif_down)) lv_obj_add_flag(up->gif_down, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_label_set_text(up->label_door, "Dwon...");
        if(up->gif_down && lv_obj_is_valid(up->gif_down)) lv_obj_clear_flag(up->gif_down, LV_OBJ_FLAG_HIDDEN);
        if(up->gif_up   && lv_obj_is_valid(up->gif_up))   lv_obj_add_flag(up->gif_up,   LV_OBJ_FLAG_HIDDEN);
    }

    // 立刻置忙，创建“到站”回调定时器（你原来是 2000ms 可按需保持）
    up->busy = true;
    lv_timer_t *t = lv_timer_create(arrive_cb, 2000, UC_P);
    if(!t) { up->busy = false; return; }
    lv_timer_set_repeat_count(t, 1);
}


static void arrive_cb(lv_timer_t *t)
{
    struct UI_Contral *UC_P = (struct UI_Contral *)t->user_data;
    if(!UC_P || !UC_P->Using_page) { lv_timer_del(t); return; }
    FL_P up = UC_P->Using_page;

    // 关键对象有效性保护
    if(!up || !up->floot_ui || !lv_obj_is_valid(up->floot_ui) ||
       !up->label_floor || !lv_obj_is_valid(up->label_floor)) {
        lv_timer_del(t);
        return;
    }

    // 到站：更新当前楼层
    up->current_floor = up->target_floor;

    // 停止运动动画
    if(up->gif_up   && lv_obj_is_valid(up->gif_up))   lv_obj_add_flag(up->gif_up,   LV_OBJ_FLAG_HIDDEN);
    if(up->gif_down && lv_obj_is_valid(up->gif_down)) lv_obj_add_flag(up->gif_down, LV_OBJ_FLAG_HIDDEN);

    // 刷新显示文本
    char buf[32];
    snprintf(buf, sizeof(buf), "local floor: %dF", up->current_floor);
    lv_label_set_text(up->label_floor, buf);

    // 进入开门阶段：创建“开门完成”回调
    lv_label_set_text(up->label_door, "Opening...");
    lv_obj_clear_flag(up->gif_door_open, LV_OBJ_FLAG_HIDDEN);
    lv_gif_restart(up->gif_door_open);

    lv_timer_t *open_t = lv_timer_create(door_open_done_cb, 5000, UC_P);
    if(!open_t) {
        // 开门定时器创建失败则直接“解忙”，但此时门动画就不做了
        up->busy = false;
        lv_timer_del(t);
        return;
    }
    lv_timer_set_repeat_count(open_t, 1);

    // 可以在这里触发一次“开门”可视效果，如果你有门 GIF/动画就切换显示
    // （保持最小改动：这里不强加额外对象）

    lv_timer_del(t);
}

static void door_open_done_cb(lv_timer_t *t)
{
    struct UI_Contral *UC_P = (struct UI_Contral *)t->user_data;
    if(!UC_P || !UC_P->Using_page) { lv_timer_del(t); return; }
    FL_P up = UC_P->Using_page;

    if(!up || !up->floot_ui || !lv_obj_is_valid(up->floot_ui) ||
       !up->label_floor || !lv_obj_is_valid(up->label_floor)) {
        lv_timer_del(t);
        return;
    }

    // 这里是“门已开”的状态，通常等待一小段时间后关门

    // 创建“关门完成”回调（例如 1000ms）
    lv_label_set_text(up->label_door, "Closing...");
    lv_obj_add_flag(up->gif_door_open, LV_OBJ_FLAG_HIDDEN);  // 隐藏开门
    lv_obj_clear_flag(up->gif_door_close, LV_OBJ_FLAG_HIDDEN);
    lv_gif_restart(up->gif_door_close);

    lv_timer_t *close_t = lv_timer_create(door_close_done_cb, 5000, UC_P);
    if(!close_t) {
        // 创建失败就直接解忙
        up->busy = false;
        lv_timer_del(t);
        return;
    }
    lv_timer_set_repeat_count(close_t, 1);

    lv_timer_del(t);

}


static void door_close_done_cb(lv_timer_t *t)
{
    struct UI_Contral *UC_P = (struct UI_Contral *)t->user_data;
    if(!UC_P || !UC_P->Using_page) { lv_timer_del(t); return; }
    FL_P up = UC_P->Using_page;

    if(!up || !up->floot_ui || !lv_obj_is_valid(up->floot_ui) ||
       !up->label_floor || !lv_obj_is_valid(up->label_floor)) {
        lv_timer_del(t);
        return;
    }

    // 关门结束，整个一次流程完成 → 解忙
    lv_obj_add_flag(up->gif_door_close, LV_OBJ_FLAG_HIDDEN);
    lv_label_set_text(up->label_door, "");  // 关门完成后不再显示“Closing...”

    up->busy = false;

    // 如需“关门完成”的可视状态，可在此切换显示对象（最小改动就不加）

    lv_timer_del(t);
}


// 完成主界面后启用管理员界面转入
void Enter_USING_page(lv_event_t * e)
{
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;

    UC_P->loading_page = loading_page(UC_P);
    lv_scr_load_anim(UC_P->loading_page->User_ui, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, true);

    if(UC_P->Start_page){
        if(UC_P->Start_page->slide_timer){
            lv_timer_del(UC_P->Start_page->slide_timer);
            UC_P->Start_page->slide_timer = NULL;
        }
        Clock_Destroy(UC_P);

        for(uint16_t i=0;i<UC_P->Start_page->img_count;i++){
            free(UC_P->Start_page->img_paths[i]);
            UC_P->Start_page->img_paths[i] = NULL;
        }
        // 不要再手动 del Start_ui（已由 lv_scr_load_anim(..., true) 自动删除）
        free(UC_P->Start_page);
        UC_P->Start_page = NULL;
    }
    return;
}



//进入主界面按钮回调
void Enter_Main_page(lv_event_t * e)
{
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;

    /* 改为先进入登录页 */
    UC_P->loading_page = loading_page(UC_P);

    lv_scr_load_anim(UC_P->loading_page->User_ui, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, true);
if(UC_P->Start_page){
    /* 停轮播 */
    if(UC_P->Start_page->slide_timer){
        lv_timer_del(UC_P->Start_page->slide_timer);
        UC_P->Start_page->slide_timer = NULL;
    }
    /* 统一销毁时钟（会删 time_timer 与 g_blink_timer） */
    Clock_Destroy(UC_P);

    /* 不要 lv_obj_del(Start_ui) —— 上面的 lv_scr_load_anim(..., true) 已删旧 screen */

    /* 释放路径数组 */
    for(uint16_t i=0;i<UC_P->Start_page->img_count;i++){
        free(UC_P->Start_page->img_paths[i]);
        UC_P->Start_page->img_paths[i] = NULL;
    }
    UC_P->Start_page->img_count = 0;

    /* 释放结构一次即可 */
    free(UC_P->Start_page);
    UC_P->Start_page = NULL;
}


    /* 释放开始页，保持你原有内存管理风格 */
    if(UC_P->Start_page){ free(UC_P->Start_page); UC_P->Start_page=NULL; }
    return;
}

//——————————————————————————————————————————————————————————————————————————————————————

//登录界面创建
LD_P loading_page(struct UI_Contral * UC_P)
{
        // [新增] 防止重复进入或使用已删除对象
    if (UC_P->loading_page && UC_P->loading_page->User_ui) {
        if (lv_obj_is_valid(UC_P->loading_page->User_ui)) {
            lv_scr_load_anim(UC_P->loading_page->User_ui, LV_SCR_LOAD_ANIM_FADE_ON, 400, 0, true);
            return UC_P->loading_page; // 你的返回值类型按原型来
        } else {
            // 之前指向的对象已被删，清理结构体避免野指针
            free(UC_P->loading_page);
            UC_P->loading_page = NULL;
        }
    }

    if(!UC_P) return NULL;

    /* 只在两者都有效时复用 */
    if(UC_P->loading_page && UC_P->loading_page->User_ui && lv_obj_is_valid(UC_P->loading_page->User_ui)) {
        lv_scr_load_anim(UC_P->loading_page->User_ui, LV_SCR_LOAD_ANIM_FADE_ON, 400, 0, true);
        return UC_P->loading_page;
    }


    UC_P->loading_page = malloc(sizeof(LD));
    if(!UC_P->loading_page) { perror("malloc loading_page "); return NULL; }
    memset(UC_P->loading_page, 0, sizeof(LD));
    LD_P ui = UC_P->loading_page;

    /* Screen + 背景 */
    ui->User_ui = lv_obj_create(NULL);
    ui->backgound = lv_img_create(ui->User_ui);
    lv_img_set_src(ui->backgound, "S:/2.jpg");
    lv_obj_center(ui->backgound);

    /* 卡片容器 */
    ui->card = lv_obj_create(ui->User_ui);
    lv_obj_set_size(ui->card, 420, LV_SIZE_CONTENT);
    lv_obj_center(ui->card);
    lv_obj_set_flex_flow(ui->card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(ui->card, 16, 0);
    lv_obj_set_style_pad_row(ui->card, 12, 0);
    lv_obj_set_style_radius(ui->card, 16, 0);
    lv_obj_set_style_shadow_width(ui->card, 18, 0);
    lv_obj_set_style_shadow_opa(ui->card, LV_OPA_20, 0);

    /* 标题 */
    lv_obj_t *title = lv_label_create(ui->card);
    lv_label_set_recolor(title, true);
    lv_label_set_text(title, "#00d5ff Login / Register#");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);

    /* 用户名 */
    lv_obj_t *lab_user = lv_label_create(ui->card);
    lv_label_set_text(lab_user, "User");
    lv_obj_set_style_text_font(lab_user, &lv_font_montserrat_20, 0);

    ui->ta_user = lv_textarea_create(ui->card);
    lv_textarea_set_one_line(ui->ta_user, true);
    lv_obj_center(ui->ta_user);
    lv_textarea_set_placeholder_text(ui->ta_user, "Please enter User");
    lv_obj_set_width(ui->ta_user, LV_PCT(100));

    /* 密码 */
    lv_obj_t *lab_pass = lv_label_create(ui->card);
    lv_label_set_text(lab_pass, "Password");
    lv_obj_set_style_text_font(lab_pass, &lv_font_montserrat_20, 0);

    ui->ta_pass = lv_textarea_create(ui->card);
    lv_textarea_set_one_line(ui->ta_pass, true);
    lv_textarea_set_password_mode(ui->ta_pass, true);
    lv_textarea_set_password_bullet(ui->ta_pass, "*");
    lv_textarea_set_placeholder_text(ui->ta_pass, "Please enter password");
    lv_obj_set_width(ui->ta_pass, LV_PCT(100));

    lv_obj_add_event_cb(ui->ta_user, ta_event_cb, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->ta_pass, ta_event_cb, LV_EVENT_ALL, ui);

    /* 按钮行 */
    lv_obj_t *row = lv_obj_create(ui->card);
    lv_obj_remove_style_all(row);
    /* 关键：行容器要有确定宽度，百分比才有参照 */
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row,
                        LV_FLEX_ALIGN_START,   /* 主轴对齐 */
                        LV_FLEX_ALIGN_CENTER,  /* 交叉轴对齐 */
                        LV_FLEX_ALIGN_CENTER); /* 行内对齐 */
    lv_obj_set_style_pad_column(row, 10, 0);

    /* Login：占满剩余空间 + 固定高度 */
    ui->btn_login = lv_btn_create(row);
    lv_obj_set_height(ui->btn_login, 44);
    lv_obj_set_flex_grow(ui->btn_login, 1);
    lv_obj_set_style_radius(ui->btn_login, 10, 0);
    lv_obj_t *lab1 = lv_label_create(ui->btn_login);
    lv_label_set_text(lab1, "Login");
    lv_obj_center(lab1);
    lv_obj_add_event_cb(ui->btn_login, login_btn_cb, LV_EVENT_SHORT_CLICKED, UC_P);

    /* Register：固定宽度 + 同步高度 */
    ui->btn_reg = lv_btn_create(row);
    lv_obj_set_size(ui->btn_reg, 120, 44);
    lv_obj_set_style_radius(ui->btn_reg, 10, 0);
    lv_obj_t *lab2 = lv_label_create(ui->btn_reg);
    lv_label_set_text(lab2, "Register");
    lv_obj_center(lab2);
    lv_obj_add_event_cb(ui->btn_reg, reg_btn_cb, LV_EVENT_SHORT_CLICKED, UC_P);

    /* 返回 */
    ui->btn_back = lv_btn_create(ui->card);
    lv_obj_set_width(ui->btn_back, LV_PCT(100));
    lv_obj_set_style_radius(ui->btn_back, 10, 0);
    lv_obj_t *lab3 = lv_label_create(ui->btn_back);
    lv_label_set_text(lab3, "return");
    lv_obj_center(lab3);
    lv_obj_add_event_cb(ui->btn_back, back_btn_cb, LV_EVENT_SHORT_CLICKED, UC_P);

    /* 软键盘（可选） */
    ui->kb = lv_keyboard_create(ui->User_ui);
    lv_obj_add_flag(ui->kb, LV_OBJ_FLAG_HIDDEN);
    // 如需自动弹出，可给 ta 添加 FOCUSED/DEFOCUSED 事件，并 set textarea 到 keyboard

    /* 预填：如果文件中已有用户名，读出来给个提示（可选） */
    char old_user[128] = {0};
    if(read_all_text(kUserFile, old_user, sizeof(old_user)) == 0 && old_user[0]) {
        lv_textarea_set_text(ui->ta_user, old_user);
    }

    lv_scr_load_anim(ui->User_ui, LV_SCR_LOAD_ANIM_FADE_ON, 400, 0, true);
    return ui;
}

//管理员基础创建
M_pg_P Main_page(struct UI_Contral * UC_P)
{
    //创建堆空间
    UC_P->Main_page = malloc(sizeof(struct Main_page));
    if(UC_P->Main_page == NULL) {
        perror("malloc main_page ");
        return NULL;
    }
    memset(UC_P->Main_page, 0, sizeof(struct Main_page));

    if (UC_P->dir_bin_inf_head == NULL) {
        UC_P->dir_bin_inf_head = Create_Node();
        if (UC_P->dir_bin_inf_head == (DBI_P)-1) {
            printf("创建头结点失败！\n");
            return UC_P->Main_page; // 或者直接 return NULL
        }
    }
    //创建主界面与背景
    UC_P->Main_page->main_ui=lv_obj_create(NULL);
    UC_P->Main_page->backgound=lv_img_create(UC_P->Main_page->main_ui);
    lv_img_set_src(UC_P->Main_page->backgound,"S:/1.jpg");
    lv_obj_center(UC_P->Main_page->backgound);
    
    //功能列表与显示
    UC_P->Main_page->main_list=lv_list_create(UC_P->Main_page->main_ui);
    lv_obj_set_size(UC_P->Main_page->main_list,200,450);
    lv_obj_set_pos(UC_P->Main_page->main_list,10,10);
    UC_P->Main_page->main_game=lv_obj_create(UC_P->Main_page->main_ui);
    lv_obj_set_size(UC_P->Main_page->main_game,570,450);
    lv_obj_set_pos(UC_P->Main_page->main_game,220,10);

        // 在 main_list/main_game 之后、调用 DCM_list 之前加上：
    UC_P->Main_page->file_ui = lv_obj_create(UC_P->Main_page->main_ui);
    lv_obj_set_size(UC_P->Main_page->file_ui, 570, 450);
    lv_obj_set_pos(UC_P->Main_page->file_ui, 220, 10);
    // 如果你暂时不用右侧的游戏容器，可以先隐藏：
    lv_obj_add_flag(UC_P->Main_page->main_game, LV_OBJ_FLAG_HIDDEN);

    // 建议：程序第一次进来时，记录一个默认目录
    if (UC_P->cur_dir[0] == '\0') {
        snprintf(UC_P->cur_dir, sizeof(UC_P->cur_dir), "%s", DEFAULT_SHOW_DIR); // 比如 "/"
    }

    // 让开始页的轮播与当前目录保持一致（注意加 "S:" 前缀）
    {
        char fs_dir[600];
        snprintf(fs_dir, sizeof(fs_dir), "S:%s", UC_P->cur_dir);
        Start_page_set_folder(UC_P, fs_dir);
    }

    // 右侧文件区显示当前目录下的图片
    if (!DCM_list(UC_P, UC_P->cur_dir)) {
        perror("show dir list ");
    }





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

    if(DCM_list(UC_P,DEFAULT_SHOW_DIR) == false)
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
void Enter_End_page(struct UI_Contral * UC_P)
{
    UC_P->End_page=End_page(UC_P);
    lv_scr_load_anim(UC_P->End_page->end_ui,LV_SCR_LOAD_ANIM_FADE_ON,500,0,true);
    lv_timer_t * t = lv_timer_create(back_to_start_cb, 5000, UC_P);
    lv_timer_set_repeat_count(t, 1);  // 只执行一次
    return ;
}

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
    lv_obj_set_style_text_font(UC_P->End_page->end_lab, &lv_font_montserrat_32, 0);
    lv_label_set_text(UC_P->End_page->end_lab,"#00ffff Thank you for your use lift system\n#");
    lv_scr_load_anim(UC_P->End_page->end_ui,LV_SCR_LOAD_ANIM_FADE_ON,500,0,true);

    return UC_P->End_page;

}
//——————————————————————————————————————————————————————————————————————————————————————

//返回到初始界面
void back_to_start_cb(lv_timer_t * timer)
{
    struct UI_Contral *UC = (struct UI_Contral *)timer->user_data;

    UC->Start_page = Start_page(UC);
    if(UC->Start_page && UC->Start_page->Start_ui &&
       lv_obj_is_valid(UC->Start_page->Start_ui)) 
    {
        // ★关键：将开始页轮播目录设置为“最后一次浏览的目录”
        if (UC->cur_dir[0] == '\0') {
            snprintf(UC->cur_dir, sizeof(UC->cur_dir), "%s", DEFAULT_SHOW_DIR);
        }
        char fs_dir[600];
        snprintf(fs_dir, sizeof(fs_dir), "S:%s", UC->cur_dir);
        Start_page_set_folder(UC, fs_dir);

        lv_scr_load_anim(UC->Start_page->Start_ui, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, true);

        free(UC->End_page);
        UC->End_page = NULL;
    }
    lv_timer_del(timer);
}

//——————————————————————————————————————————————————————————————————————————————————————

//功能list创建
//——————————————————————————————————————————————————————————————————————————————————————


bool DCM_list(struct UI_Contral * UC_P, const char *obj_dir_path)
{
    if (!UC_P || !UC_P->Main_page || !UC_P->Main_page->main_list || !UC_P->Main_page->file_ui || !obj_dir_path)
        return false;

    // 确保链表头存在
    if (UC_P->dir_bin_inf_head == NULL) {
        UC_P->dir_bin_inf_head = Create_Node();
        if (UC_P->dir_bin_inf_head == (DBI_P)-1) {
            printf("创建头结点失败！\n");
            return false;
        }
    } else {
        // 清掉旧链表（避免野指针/泄漏）
        Destory_Dir_Btn_List(UC_P->dir_bin_inf_head);
    }

    // 清空左右两个区域
    lv_obj_clean(UC_P->Main_page->main_list);
    lv_obj_clean(UC_P->Main_page->file_ui);

    DIR *dq = opendir(obj_dir_path);
    if (!dq) { perror("opendir "); return false; }

    // 左侧：先加一个“返回上级”
    {
        DBI_P up = Create_Node();
        if (up != (DBI_P)-1) {
            up->UC_P = UC_P;
            // 计算上级目录
            if (strcmp(obj_dir_path, "/") == 0) {
                strcpy(up->dir_name, "/");    // 已经是根
            } else {
                snprintf(up->dir_name, sizeof(up->dir_name), "%s", obj_dir_path);
                char *last = strrchr(up->dir_name, '/');
                if (last) {
                    if (last == up->dir_name) { // "/xxx" → "/"
                        up->dir_name[1] = '\0';
                    } else {
                        *last = '\0';
                    }
                }
            }
            Head_Add_Node(UC_P->dir_bin_inf_head, up);
            lv_obj_t *btn = lv_list_add_btn(UC_P->Main_page->main_list, LV_SYMBOL_LEFT, "..");
            lv_obj_add_event_cb(btn, Dir_Btn_Task, LV_EVENT_SHORT_CLICKED, up);
        }
    }

    // 扫描目录，分两遍：第一遍收集并显示文件夹，第二遍显示图片文件
    struct dirent *eq;
    // —— 第一遍：目录 ——
    rewinddir(dq);
    while ((eq = readdir(dq)) != NULL) {
        if (strcmp(eq->d_name, ".") == 0 || strcmp(eq->d_name, "..") == 0) continue;

        bool is_dir = false;
        if (eq->d_type == DT_DIR) {
            is_dir = true;
        } else if (eq->d_type == DT_UNKNOWN) {
            // 用 stat 兜底
            char fullp[1024] = {0};
            if (obj_dir_path[strlen(obj_dir_path)-1] == '/')
                snprintf(fullp, sizeof(fullp), "%s%s", obj_dir_path, eq->d_name);
            else
                snprintf(fullp, sizeof(fullp), "%s/%s", obj_dir_path, eq->d_name);
            struct stat st;
            if (stat(fullp, &st) == 0 && S_ISDIR(st.st_mode)) is_dir = true;
        }
        if (!is_dir) continue;

        // 左侧添加目录按钮
        DBI_P btn_inf = Create_Node();
        if (btn_inf == (DBI_P)-1) { closedir(dq); return false; }
        btn_inf->UC_P = UC_P;

        if (obj_dir_path[strlen(obj_dir_path)-1] == '/')
            snprintf(btn_inf->dir_name, sizeof(btn_inf->dir_name), "%s%s", obj_dir_path, eq->d_name);
        else
            snprintf(btn_inf->dir_name, sizeof(btn_inf->dir_name), "%s/%s", obj_dir_path, eq->d_name);

        Head_Add_Node(UC_P->dir_bin_inf_head, btn_inf);

        lv_obj_t *dir_btn = lv_list_add_btn(UC_P->Main_page->main_list, LV_SYMBOL_DIRECTORY, eq->d_name);
        lv_obj_add_event_cb(dir_btn, Dir_Btn_Task, LV_EVENT_SHORT_CLICKED, btn_inf);
    }

    // —— 第二遍：图片文件 ——
    int file_btn_x = 8, file_btn_y = 8;
    rewinddir(dq);
    while ((eq = readdir(dq)) != NULL) {
        if (strcmp(eq->d_name, ".") == 0 || strcmp(eq->d_name, "..") == 0) continue;

        bool is_reg = false;
        if (eq->d_type == DT_REG) {
            is_reg = true;
        } else if (eq->d_type == DT_UNKNOWN) {
            char fullp[1024] = {0};
            if (obj_dir_path[strlen(obj_dir_path)-1] == '/')
                snprintf(fullp, sizeof(fullp), "%s%s", obj_dir_path, eq->d_name);
            else
                snprintf(fullp, sizeof(fullp), "%s/%s", obj_dir_path, eq->d_name);
            struct stat st;
            if (stat(fullp, &st) == 0 && S_ISREG(st.st_mode)) is_reg = true;
        }
        if (!is_reg) continue;
        if (!has_img_ext_ci(eq->d_name)) continue;

        DBI_P btn_inf = Create_Node();
        if (btn_inf == (DBI_P)-1) { closedir(dq); return false; }
        btn_inf->UC_P = UC_P;

        // 真实路径（/xxx/xxx.jpg），显示时我们再拼 "S:%s"
        if (obj_dir_path[strlen(obj_dir_path)-1] == '/')
            snprintf(btn_inf->dir_name, sizeof(btn_inf->dir_name), "%s%s", obj_dir_path, eq->d_name);
        else
            snprintf(btn_inf->dir_name, sizeof(btn_inf->dir_name), "%s/%s", obj_dir_path, eq->d_name);

        Head_Add_Node(UC_P->dir_bin_inf_head, btn_inf);

        // 右侧文件格子
        lv_obj_t *file_btn = lv_btn_create(UC_P->Main_page->file_ui);
        lv_obj_set_size(file_btn, 80, 80);
        lv_obj_set_pos(file_btn, file_btn_x, file_btn_y);

        lv_obj_t *btn_lab = lv_label_create(file_btn);
        lv_label_set_text(btn_lab, eq->d_name);
        lv_label_set_long_mode(btn_lab, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_width(btn_lab, 80);
        lv_obj_center(btn_lab);

        // 绑定事件：gif 用 gif 回调，其他走图片回调
        const char *dot = strrchr(eq->d_name, '.');
        if (dot && (strcasecmp(dot+1, "gif") == 0)) {
            lv_obj_add_event_cb(file_btn, File_Btn_gif_Task, LV_EVENT_SHORT_CLICKED, btn_inf);
        } else {
            lv_obj_add_event_cb(file_btn, File_Btn_Task, LV_EVENT_SHORT_CLICKED, btn_inf);
        }

        // 摆放
        file_btn_x += 90;
        if (file_btn_x > 360) { file_btn_x = 8; file_btn_y += 90; }
    }

    if (closedir(dq) == -1) { perror("closedir "); return false; }
    return true;
}


void Dir_Btn_Task(lv_event_t *e )
{
    DBI_P btn_inf =(DBI_P)e->user_data;
    lv_obj_clean(btn_inf->UC_P->Main_page->main_list);
    char new_dir_name[256*2] = {0};
    strcpy(new_dir_name,btn_inf->dir_name);
    struct UI_Contral *UC_P = btn_inf->UC_P;


    //顺便把旧按钮对应的堆空间free调用 摧毁链表 剩下 头结点
    if(Destory_Dir_Btn_List(btn_inf->UC_P->dir_bin_inf_head) == false)
    {
        printf("摧毁链表失败！\n");
        return ;
    }
    //再添加新按钮
    // 清空旧列表的控件（不仅 main_list，顺带把文件区也清一遍）
    lv_obj_clean(btn_inf->UC_P->Main_page->main_list);
    lv_obj_clean(btn_inf->UC_P->Main_page->file_ui);

    // // 正确的参数顺序：先 UC_P，后路径
    // DCM_list(UC_P, new_dir_name);
    // 记录为“当前目录”（释放 Main_page 之后也不会丢）
    snprintf(UC_P->cur_dir, sizeof(UC_P->cur_dir), "%s", new_dir_name);

    // 重新加载左右两栏
    DCM_list(UC_P, UC_P->cur_dir);

    //Show_Dir_List(btn_inf->dir_name,btn_inf->UC_P);
    return ;
}

void File_Btn_Task(lv_event_t *e)
{
    DBI_P btn_inf =(DBI_P)e->user_data;

    lv_obj_add_flag(btn_inf->UC_P->Main_page->main_ui,LV_OBJ_FLAG_HIDDEN);

    btn_inf->UC_P->Main_page->img_ui = lv_obj_create(NULL);
    lv_obj_t * img_win= lv_win_create(btn_inf->UC_P->Main_page->img_ui,30);
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

    lv_scr_load(btn_inf->UC_P->Main_page->img_ui);
    return ;
}

void Close_Img_Task(lv_event_t * e)
{
    // 通过 user_data 直接拿 UC_P（因为我们在绑定时传的是 UC_P）
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if (UC_P == NULL || UC_P->Main_page == NULL) return;

    // 1) 先恢复目录界面
    if (UC_P->Main_page->main_ui) {
        lv_obj_clear_flag(UC_P->Main_page->main_ui, LV_OBJ_FLAG_HIDDEN);
        lv_scr_load(UC_P->Main_page->main_ui);
    }

    // 2) 再“异步删除”图片界面，避免在事件处理栈里直接删父对象导致崩溃
    if (UC_P->Main_page->img_ui) {
        lv_obj_del_async(UC_P->Main_page->img_ui);
        UC_P->Main_page->img_ui = NULL;
    }

}

void File_Btn_gif_Task(lv_event_t *e)
{
    DBI_P btn_inf = (DBI_P)e->user_data;

    // 隐藏目录界面
    lv_obj_add_flag(btn_inf->UC_P->Main_page->main_ui, LV_OBJ_FLAG_HIDDEN);

    // 新建一个屏幕承载窗口
    btn_inf->UC_P->Main_page->img_ui = lv_obj_create(NULL);

    // 创建带标题栏的窗口
    lv_obj_t *img_win = lv_win_create(btn_inf->UC_P->Main_page->img_ui, 30);
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
    lv_scr_load(btn_inf->UC_P->Main_page->img_ui);
}

DBI_P Create_Node(void)
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

// ★实现：主界面选定文件夹后调用，开始页将使用该目录轮播
void Start_page_set_folder(struct UI_Contral *UC_P, const char *dir)
{
    if(!UC_P || !UC_P->Start_page || !dir) return;
    snprintf(UC_P->Start_page->img_dir, sizeof(UC_P->Start_page->img_dir), "%s", dir);

    // 若开始页已经创建了控件，可立即刷新一次列表与显示
    if(UC_P->Start_page->bg_img) {
        if(scan_img_dir(UC_P->Start_page) > 0) {
            lv_img_set_src(UC_P->Start_page->bg_img, UC_P->Start_page->img_paths[0]);
            UC_P->Start_page->img_index = 0;
            if(!UC_P->Start_page->slide_timer) {
                UC_P->Start_page->slide_timer = lv_timer_create(slide_timer_cb, UC_P->Start_page->slide_ms, UC_P->Start_page);
            }
        }
    }
}

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

// //游戏游玩界面进入
// void Enter_Game_page(struct UI_Contral * UC_P)
// {
//     UC_P->Game_page=Game_page(UC_P);
//     if(!UC_P->Game_page) return;
//     lv_scr_load_anim(UC_P->Game_page->Game_ui,LV_SCR_LOAD_ANIM_FADE_ON,500,0,true);
//     free(UC_P->Main_page);
//     UC_P->Main_page=NULL;
//     return ;
// }
//——————————————————————————————————————————————————————————————————————————————————————

// //游戏游玩界面创建
// GP_P Game_page(struct UI_Contral * UC_P)
// {
//     if(!UC_P->Game_page)
//     {
//         UC_P->Game_page = malloc(sizeof(struct Game_page));
//         if(!UC_P->Game_page)
//         { 
//             perror("malloc game_page "); 
//             return NULL; 
//         }
//         memset(UC_P->Game_page, 0, sizeof(struct Game_page));
//     }

//     UC_P->Game_page->Game_ui=lv_obj_create(NULL);
//     // UC_P->Game_page->backgound=lv_img_create(UC_P->Game_page->Game_ui);
//     // lv_img_set_src(UC_P->Game_page->backgound,"S:/2.jpg");

//     UC_P->Game_page->back_btn=lv_btn_create(UC_P->Game_page->Game_ui);
//     lv_obj_set_size(UC_P->Game_page->back_btn,70,30);
//     lv_obj_set_pos(UC_P->Game_page->back_btn,720,10);
//     lv_obj_add_event_cb(UC_P->Game_page->back_btn, Return_page, LV_EVENT_SHORT_CLICKED, UC_P);

//     //退出文字
//     UC_P->Game_page->back_lab=lv_label_create(UC_P->Game_page->back_btn);
//     lv_label_set_recolor(UC_P->Game_page->back_lab,1);
//     lv_obj_center(UC_P->Game_page->back_lab);
//     lv_label_set_text(UC_P->Game_page->back_lab,"#ff0000 E##fffb00 X##00ecf0 I##d626e7 T");

//     return UC_P->Game_page;

// }
// //——————————————————————————————————————————————————————————————————————————————————————

// 进入小游戏大厅（事件回调）
void Enter_Game_page(lv_event_t * e)
{
    struct UI_Contral *UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;

    UC_P->Game_page = Game_page(UC_P);
    if(UC_P->Game_page && UC_P->Game_page->Game_ui) {
        lv_scr_load_anim(UC_P->Game_page->Game_ui, LV_SCR_LOAD_ANIM_FADE_ON, 400, 0, true);
    }
}

// 构建小游戏大厅
GP_P Game_page(struct UI_Contral * UC_P)
{
    UC_P->Game_page = malloc(sizeof(GP));
    if(!UC_P->Game_page) { perror("malloc Game_page"); return NULL; }
    memset(UC_P->Game_page, 0, sizeof(GP));
    GP_P gp = UC_P->Game_page;

    init_game_styles();

    gp->Game_ui = lv_obj_create(NULL);

    // 背景
    // gp->backgound = lv_img_create(gp->Game_ui);
    // lv_img_set_src(gp->backgound, "S:/1.jpg");
    // lv_obj_center(gp->backgound);

    // 顶部栏（标题 + 返回）
    lv_obj_t *top = lv_obj_create(gp->Game_ui);
    lv_obj_remove_style_all(top);
    lv_obj_set_size(top, LV_PCT(100), 56);
    lv_obj_align(top, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *title = lv_label_create(top);
    lv_label_set_text(title, "Game Center");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 16, 0);

    gp->back_btn = lv_btn_create(top);
    lv_obj_set_size(gp->back_btn, 96, 36);
    lv_obj_align(gp->back_btn, LV_ALIGN_RIGHT_MID, -16, 0);
    lv_obj_add_style(gp->back_btn, &g_btn, 0);
    lv_obj_add_style(gp->back_btn, &g_btn_pr, LV_STATE_PRESSED);
    gp->back_lab = lv_label_create(gp->back_btn);
    lv_label_set_text(gp->back_lab, "Back");
    lv_obj_center(gp->back_lab);
    // 返回上一界面：回到 USING_page（你也可以回 Main_page）
    lv_obj_add_event_cb(gp->back_btn, Return_page, LV_EVENT_SHORT_CLICKED, UC_P);

    // 主区域：两张游戏卡片（网格/自适应）
    gp->menu_wrap = lv_obj_create(gp->Game_ui);          // ★保存到结构体
    lv_obj_set_size(gp->menu_wrap, 760, 380);
    lv_obj_align(gp->menu_wrap, LV_ALIGN_BOTTOM_MID, 0, -16);
    lv_obj_set_flex_flow(gp->menu_wrap, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_row(gp->menu_wrap, 16, 0);
    lv_obj_set_style_pad_column(gp->menu_wrap, 16, 0);
    lv_obj_add_style(gp->menu_wrap, &g_card, 0);


    // —— 卡片：2048 ——
    lv_obj_t *c1 = lv_obj_create(gp->menu_wrap);
    lv_obj_set_size(c1, 350, 160);
    lv_obj_add_style(c1, &g_card, 0);
    lv_obj_set_flex_flow(c1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(c1, 8, 0);

    lv_obj_t *c1_title = lv_label_create(c1);
    lv_label_set_text(c1_title, "2048");
    lv_obj_set_style_text_font(c1_title, &lv_font_montserrat_24, 0);

    lv_obj_t *c1_desc = lv_label_create(c1);
    lv_label_set_text(c1_desc, "Swipe or arrow keys to merge numbers.");
    lv_label_set_long_mode(c1_desc, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(c1_desc, LV_PCT(100));

    lv_obj_t *c1_row = lv_obj_create(c1);
    lv_obj_remove_style_all(c1_row);
    lv_obj_set_size(c1_row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(c1_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(c1_row, 8, 0);

    lv_obj_t *c1_start = lv_btn_create(c1_row);
    lv_obj_add_style(c1_start, &g_btn, 0);
    lv_obj_add_style(c1_start, &g_btn_pr, LV_STATE_PRESSED);
    lv_obj_t *c1_lab = lv_label_create(c1_start);
    lv_label_set_text(c1_lab, "Start 2048");
    lv_obj_center(c1_lab);
    // 启动 2048
    lv_obj_add_event_cb(c1_start, game_2048_btn_event_handler, LV_EVENT_SHORT_CLICKED, UC_P);

    // —— 卡片：Memory（翻牌） ——
    lv_obj_t *c2 = lv_obj_create(gp->menu_wrap);
    lv_obj_set_size(c2, 350, 160);
    lv_obj_add_style(c2, &g_card, 0);
    lv_obj_set_flex_flow(c2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(c2, 8, 0);

    lv_obj_t *c2_title = lv_label_create(c2);
    lv_label_set_text(c2_title, "Memory Match");
    lv_obj_set_style_text_font(c2_title, &lv_font_montserrat_24, 0);

    lv_obj_t *c2_desc = lv_label_create(c2);
    lv_label_set_text(c2_desc, "Flip two cards to find a pair.");
    lv_label_set_long_mode(c2_desc, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(c2_desc, LV_PCT(100));

    lv_obj_t *c2_row = lv_obj_create(c2);
    lv_obj_remove_style_all(c2_row);
    lv_obj_set_size(c2_row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(c2_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(c2_row, 8, 0);

    lv_obj_t *c2_start = lv_btn_create(c2_row);
    lv_obj_add_style(c2_start, &g_btn, 0);
    lv_obj_add_style(c2_start, &g_btn_pr, LV_STATE_PRESSED);
    lv_obj_t *c2_lab = lv_label_create(c2_start);
    lv_label_set_text(c2_lab, "Start Memory");
    lv_obj_center(c2_lab);
    // 启动 Memory
    lv_obj_add_event_cb(c2_start, game_memory_btn_event_handler, LV_EVENT_SHORT_CLICKED, UC_P);

    return gp;
}


//返回主界面
// 新增：切回后再释放 Game_page（在 LV_EVENT_SCREEN_LOADED 上触发）
static void free_game_after_loaded(lv_event_t *e)
{
    struct UI_Contral *U = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!U) return;

    if (U->Game_page) {
        // 此时旧 screen（游戏）已被 lv_scr_load_anim(..., true) 自动删除
        free(U->Game_page);
        U->Game_page = NULL;
    }

    // 只用一次就可以移除这个回调（可选）
    lv_obj_remove_event_cb_with_user_data(lv_event_get_target(e), free_game_after_loaded, U);
}

void Return_page(lv_event_t * e)
{
    struct UI_Contral *UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if (!UC_P) return;

     if (UC_P->Game_page && UC_P->Game_page->back_btn && lv_obj_is_valid(UC_P->Game_page->back_btn))
        lv_obj_add_state(UC_P->Game_page->back_btn, LV_STATE_DISABLED);
    // 1) 确保 Using 界面存在且有效；无效就重建一个
    if (!UC_P->Using_page || !UC_P->Using_page->User_ui || 
        !lv_obj_is_valid(UC_P->Using_page->User_ui)) {
        UC_P->Using_page = USING_page(UC_P);
        if(!UC_P->Using_page) return;
    }

    // 2) 切回 Using 界面，并让 LVGL 自动删除“当前（游戏）screen”
    lv_scr_load_anim(UC_P->Using_page->User_ui, LV_SCR_LOAD_ANIM_FADE_ON, 400, 0, true);

    // 3) 等新屏加载完成后再 free Game_page 结构体，避免竞态
    lv_obj_add_event_cb(UC_P->Using_page->User_ui, free_game_after_loaded, 
                        LV_EVENT_SCREEN_LOADED, UC_P);
}



//——————
void emergency_enter(lv_event_t * e)
{
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;


    UC_P->emergency_page = EMERGENCY_page(UC_P);
    lv_scr_load_anim(UC_P->emergency_page->emergency_ui, LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, true);


    //if(UC_P->Using_page){ free(UC_P->Using_page); UC_P->Using_page=NULL; }
    return;
}
static void emergency_return_to_using_cb(lv_event_t *e)
{
    struct UI_Contral *UC = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC) return;

    if (UC->Using_page && UC->Using_page->User_ui && lv_obj_is_valid(UC->Using_page->User_ui)) {
        lv_scr_load_anim(UC->Using_page->User_ui, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, true);
    } else {
        // 兜底：回到 Start 界面
        UC->Start_page = Start_page(UC);
        lv_scr_load_anim(UC->Start_page->Start_ui, LV_SCR_LOAD_ANIM_FADE_ON, 400, 0, true);
    }
}


EM_P EMERGENCY_page(struct UI_Contral * UC_P)
{
    UC_P->emergency_page = malloc(sizeof(EM));
    if(!UC_P->emergency_page) { perror("malloc emergency_page"); return NULL; }
    memset(UC_P->emergency_page, 0, sizeof(EM));

    UC_P->emergency_page->emergency_ui=lv_obj_create(NULL);
    UC_P->emergency_page->emergency_btn=lv_btn_create(UC_P->emergency_page->emergency_ui);
    UC_P->emergency_page->emergency_lab=lv_label_create(UC_P->emergency_page->emergency_btn);
    lv_obj_set_size(UC_P->emergency_page->emergency_btn,300,300);
    lv_obj_center(UC_P->emergency_page->emergency_btn);
    lv_obj_center(UC_P->emergency_page->emergency_lab);
    lv_label_set_recolor(UC_P->emergency_page->emergency_lab,1);
    lv_label_set_text(UC_P->emergency_page->emergency_lab,"#ff0000 EMERGENCY BOTTEN#");
    lv_obj_add_event_cb(UC_P->emergency_page->emergency_btn, emergency_big_btn_cb, LV_EVENT_CLICKED, UC_P);



    UC_P->emergency_page->return_btn=lv_btn_create(UC_P->emergency_page->emergency_ui);
    UC_P->emergency_page->return_lab=lv_label_create(UC_P->emergency_page->return_btn);
    lv_obj_center(UC_P->emergency_page->return_lab);
    lv_obj_set_size(UC_P->emergency_page->return_btn,120,40);
    lv_obj_set_pos(UC_P->emergency_page->return_btn,670,10);
    lv_label_set_text(UC_P->emergency_page->return_lab,"return");
    lv_obj_add_event_cb(UC_P->emergency_page->return_btn,back_btn_cb,LV_EVENT_CLICKED,UC_P);
    return UC_P->emergency_page;
}



static void emergency_big_btn_cb(lv_event_t *e)
{
    struct UI_Contral *UC = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC || !UC->emergency_page) return;

    // EMERGENCY_page 的根容器（注意：不要删掉它本身）
    lv_obj_t *root = UC->emergency_page->emergency_ui;
    if(!root || !lv_obj_is_valid(root)) return;

    // 清空所有子控件（保留 root）
    lv_obj_clean(root);

    // 放置主图片（替换成你的资源路径/符号）
    lv_obj_t *img = lv_img_create(root);
    lv_img_set_src(img, "S:/2.jpg");
    lv_obj_center(img);

    // 底部“返回 Using_page”按钮
    lv_obj_t *ret = lv_btn_create(root);
    lv_obj_set_size(ret, 160, 46);
    lv_obj_align(ret, LV_ALIGN_BOTTOM_MID, 0, -12);
    lv_obj_t *lab = lv_label_create(ret);
    lv_label_set_text(lab, "break");
    lv_obj_center(lab);

    lv_obj_add_event_cb(ret, emergency_return_to_using_cb, LV_EVENT_CLICKED, UC);
}






//————————————————————————————————————————————————————————————————————————————————

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
//——————————————————————————————————————————————————————————————————————————————————————

//创建函数调用——清除主界面（释放空间，来保证不怎么卡），转到2048
// 修复一：2048 按钮
static void game_2048_btn_event_handler(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_SHORT_CLICKED && lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;
    if (UC_P->Game_page && UC_P->Game_page->in_game) return;  // ★正在游戏就忽略

    // ❶ 这里原来是 Enter_Game_page(UC_P); —— 错误
    Enter_Game_page(e);  // ✅ 传入事件，函数内部才能用 lv_event_get_user_data(e)

    lv_timer_t * t = lv_timer_create(start_2048_cb, 10, UC_P);
    lv_timer_set_repeat_count(t, 1);
}

//——————————————————————————————————————————————————————————————————————————————————————

/*用延迟等待 Game_ui 之后再创建 2048 */
// static void start_2048_cb(lv_timer_t * timer)
// {
//     struct UI_Contral * UC_P = (struct UI_Contral *)timer->user_data;
//     if(!UC_P || !UC_P->Game_page || !UC_P->Game_page->Game_ui) { lv_timer_del(timer); return; }
//     lv_100ask_2048_simple_test();
//     lv_timer_del(timer);
// }
//—————————————————————————————————————————————————————————————————————————————————————

static void start_2048_cb(lv_timer_t * timer)
{
    struct UI_Contral * UC_P = (struct UI_Contral *)timer->user_data;
    if(!UC_P || !UC_P->Game_page || !UC_P->Game_page->Game_ui) { lv_timer_del(timer); return; }

    // ★隐藏大厅，防止还能点
    if (UC_P->Game_page->menu_wrap && lv_obj_is_valid(UC_P->Game_page->menu_wrap))
        lv_obj_add_flag(UC_P->Game_page->menu_wrap, LV_OBJ_FLAG_HIDDEN);
    UC_P->Game_page->in_game = true;

    lv_100ask_2048_simple_test();   // 会在 lv_scr_act()（=Game_ui）上创建控件
    lv_timer_del(timer);
}

//创建函数调用——清除主界面（释放空间，来保证不怎么卡），转到记忆游戏
// 修复二：记忆游戏按钮
static void game_memory_btn_event_handler(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_SHORT_CLICKED && lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    struct UI_Contral * UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;
    if (UC_P->Game_page && UC_P->Game_page->in_game) return;  // ★正在游戏就忽略

    // ❷ 这里原来是 Enter_Game_page(UC_P); —— 错误
    Enter_Game_page(e);  // ✅

    lv_timer_t * t = lv_timer_create(start_memory_cb, 10, UC_P);
    lv_timer_set_repeat_count(t, 1);
}

//——————————————————————————————————————————————————————————————————————————————————————

/*用延迟等待 Game_ui 之后再创建 记忆游戏 */
static void start_memory_cb(lv_timer_t * timer)
{
    struct UI_Contral * UC_P = (struct UI_Contral *)timer->user_data;
    if(!UC_P || !UC_P->Game_page || !UC_P->Game_page->Game_ui) { lv_timer_del(timer); return; }

    // ★隐藏大厅，防止还能点
    if (UC_P->Game_page->menu_wrap && lv_obj_is_valid(UC_P->Game_page->menu_wrap))
        lv_obj_add_flag(UC_P->Game_page->menu_wrap, LV_OBJ_FLAG_HIDDEN);
    UC_P->Game_page->in_game = true;
    lv_100ask_memory_game_simple_test();
    lv_timer_del(timer);
}
//——————————————————————————————————————————————————————————————————————————————————————
//2048游戏程序--1
static void game_2048_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj_2048 = lv_event_get_target(e);
    lv_obj_t * label = lv_event_get_user_data(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) 
    {
        umask(0000);
        int sc=open("./Max_2042_scores.txt",O_RDWR|O_CREAT,0777);
        int sc_2048=open("./2048_scores.txt",O_RDWR|O_CREAT,0777);
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
        close(sc_2048);
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
/* 登录：读取文件，核对账号密码，成功则进入 Main_page */
static void login_btn_cb(lv_event_t *e)
{
    
    struct UI_Contral *UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P || !UC_P->loading_page) return;
    if(!UC_P->loading_page || !lv_obj_is_valid(UC_P->loading_page->User_ui)) 
    {
        return; // 界面已经被删掉了，直接返回
    }
    LD_P ui = UC_P->loading_page;

    const char *u = lv_textarea_get_text(ui->ta_user);
    const char *p = lv_textarea_get_text(ui->ta_pass);
    if(!u || !p || !u[0] || !p[0]) { toast(ui->card, "please enter User and Password"); return; }

    char file_u[256] = {0}, file_p[256] = {0};
    if(read_all_text(kUserFile, file_u, sizeof(file_u)) != 0 ||
       read_all_text(kPassFile, file_p, sizeof(file_p)) != 0) {
        toast(ui->card, "No Register");
        return;
    }

    if(strcmp(u, file_u)==0 && strcmp(p, file_p)==0) 
    {
        /* 登录通过 → 进入主界面 */
        UC_P->Main_page = Main_page(UC_P);
        if(UC_P->Main_page && UC_P->Main_page->main_ui) {
            lv_scr_load_anim(UC_P->Main_page->main_ui, LV_SCR_LOAD_ANIM_FADE_ON, 400, 0, true);
            /* 可释放 Start_page，按你的流程你在 Enter_Main_page 中也会处理 */
                if (UC_P->loading_page) {
                    UC_P->loading_page->User_ui = NULL;
                    UC_P->loading_page->kb = NULL;  /* 防后续误用 */
                }
            } 
            else 
            {
                toast(ui->card, "Main_page loading error");
            }


        } 
        else 
        {
            toast(ui->card, "Main_page loading error");
        }
    } 

/* 注册：将输入内容覆盖写入 User_name / User_password */
static void reg_btn_cb(lv_event_t *e)
{
    struct UI_Contral *UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P || !UC_P->loading_page) return;
    LD_P ui = UC_P->loading_page;

    const char *u = lv_textarea_get_text(ui->ta_user);
    const char *p = lv_textarea_get_text(ui->ta_pass);


    if(!u || !p || !u[0] || !p[0]) { toast(ui->card, "please enter User and Password"); return; }

    /* 直接覆盖保存到两个文件（按你的要求） */
    if(write_text(kUserFile, u) != 0) { toast(ui->card, "User writing error"); return; }
    if(write_text(kPassFile, p) != 0) { toast(ui->card, "Password writing error"); return; }

    toast(ui->card, "Register Success");
}

/* 返回：回到 Start_page */
static void back_btn_cb(lv_event_t *e)
{
    struct UI_Contral *UC_P = (struct UI_Contral *)lv_event_get_user_data(e);
    if(!UC_P) return;
    UC_P->Start_page = Start_page(UC_P);
    lv_scr_load_anim(UC_P->Start_page->Start_ui, LV_SCR_LOAD_ANIM_FADE_ON, 400, 0, true);
   if (UC_P->loading_page) {
        UC_P->loading_page->User_ui = NULL;
        UC_P->loading_page->kb = NULL;
        /* 此处不 free，留待下次进入 login 时复用或在程序退出时统一释放 */
        /* 如果你非常想释放，可用一个短延时 timer 在动画结束后再 free（可选） */
    }
    if(UC_P->emergency_page)
    {
        UC_P->emergency_page->emergency_ui = NULL;
    }

}

static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    LD_P ui = (LD_P)lv_event_get_user_data(e);
    if(!ui) return;

    if(code == LV_EVENT_FOCUSED) {
        if(ui->kb && lv_obj_is_valid(ui->kb)) {
            lv_keyboard_set_textarea(ui->kb, ta);
            lv_obj_clear_flag(ui->kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_align(ui->kb, LV_ALIGN_BOTTOM_MID, 0, 0);
        }
    } else if(code == LV_EVENT_DEFOCUSED) {
        if(ui->kb && lv_obj_is_valid(ui->kb)) {
            lv_obj_add_flag(ui->kb, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

static bool has_img_ext_ci(const char *name) 
{
    if (!name) return false;
    const char *dot = strrchr(name, '.');
    if (!dot || dot[1] == '\0') return false;
    char ext[8] = {0};
    snprintf(ext, sizeof(ext), "%s", dot + 1);
    for (char *p = ext; *p; ++p) {
        if (*p >= 'A' && *p <= 'Z') *p = *p - 'A' + 'a';
    }
    return strcmp(ext,"jpg")==0 || strcmp(ext,"jpeg")==0
        || strcmp(ext,"png")==0 || strcmp(ext,"gif")==0;
}




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
    lv_btnmatrix_set_map(game_2048->btnm, (const char **)game_2048->btnm_map);
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

    lv_btnmatrix_set_map(game_2048->btnm, (const char **)game_2048->btnm_map);
    lv_btnmatrix_set_btn_ctrl_all(game_2048->btnm, LV_BTNMATRIX_CTRL_DISABLED);

    lv_obj_add_event_cb(game_2048->btnm, btnm_event_cb, LV_EVENT_ALL, NULL);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_100ask_2048_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_100ask_2048_t * game_2048 = (lv_100ask_2048_t *)obj;

    uint16_t index;
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
        lv_btnmatrix_set_map(game_2048->btnm, (const char **)game_2048->btnm_map);
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
        if((dsc->id != LV_BTNMATRIX_BTN_NONE) && dsc->label_dsc)
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
	// if(num < 0)//如果num为负数，将num变正
	// {
	// 	num = -num;
	// 	str[i++] = '-';
	// }
	//转换
	do
	{
		str[i++] = num%10+48;//取num最低位 字符0~9的ASCII码是48~57；简单来说数字0+48=48，ASCII码对应字符'0'
		num /= 10;//去掉最低位
	}while(num);//num不为0继续循环

	str[i] = '\0';

	//确定开始调整的位置
	uint8_t j = 0;
	// if(str[0] == '-')//如果有负号，负号不用调整
	// {
	// 	j = 1;//从第二位开始调整
	// 	++i;//由于有负号，所以交换的对称轴也要后移1位
	// }
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
//Game2----memory
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
        for(int16_t i = 0; i < new_count; i++)
        {
            /*Delete the hit object first*/
            for(uint16_t j = 0; j < lv_obj_get_child_cnt(obj); j++) {
                lv_obj_t * child = lv_obj_get_child(obj, j);
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
//——————————————————————————————————————————————————————————————————————————————————————


