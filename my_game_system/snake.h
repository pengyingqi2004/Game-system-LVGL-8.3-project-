#ifndef _SNAKE_H_
#define _SNAKE_H_
typedef struct snake
{
    lv_obj_t *obj;       // 控件对象本体（可选）
    lv_timer_t *tick;    // 驱动蛇前进的定时器
    uint16_t cols, rows; // 网格尺寸
    uint16_t cell;       // 每格像素大小
    int dir;             // 当前方向：0上1右2下3左
    int next_dir;        // 预读方向（防止立刻掉头）
    int head_x, head_y;  // 蛇头坐标（格）
    int len;             // 当前长度
    int max_len;         // 蛇身容量
    int score;           // 分数
    struct { int x,y; } food;
    // 简单环形队列存蛇身（从 0..max_len-1）
    int *sx;
    int *sy;
    bool game_over;
}sn,*sn_p;


#endif