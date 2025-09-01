#include "game.h"
void *Time_show(void * arg)
{ 
    int *p=local_time;
    while(1)
    {
        sleep(1);
        p[2]++;
        if(p[2]>=60)
        {
            p[1]++;
            p[2]=0;
        }
        if(p[1]>=60)
        {
            p[0]++;
            p[1]=0;
        }
        if(p[0]>=24)
        {
            p[0]=0;
        }
    }
    return NULL;
}