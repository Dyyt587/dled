#include "list.h"
#include "dled.h"
#include "stdint.h"
#include "malloc.h"
#include <limits.h>


typedef struct {
    struct list_head list;
    dled_t *led;
    dled_task_t *task;
    uint16_t task_index;
    uint16_t task_times;//任务次数
    int task_tick;//任务时间
} task_t;


static LIST_HEAD(active_task_start);
static LIST_HEAD(bpk_task_start);
static LIST_HEAD(gc_task_start);
//static uint8_t is_init = 0;




//void __led_init(void);
void led_safe_check(dled_t *led) {
    if (led == NULL) while (1);
    if (led->updata == NULL) while (1);
}

void led_create_task(dled_t *led, dled_task_t task[], uint16_t task_times) {
    //if (is_init != INITED) __led_init();
    led_safe_check(led);
    task_t *new_task=0;
    if(list_empty(&gc_task_start)){
        new_task = (task_t *) malloc(sizeof(task_t));
    }else{
        //将垃圾回收区的块重新使用

            //printf("\t\t\tuse the gc memory\n");

        new_task = list_entry(gc_task_start.next,task_t,list);
        __list_del(new_task->list.prev,new_task->list.prev);
    }
       //new_task = (task_t *) malloc(sizeof(task_t));

    //创建任务并挂载到活动列表中
    if (new_task != NULL && task != NULL) {
        new_task->led = led;
        new_task->task_times = task_times;
        new_task->task_index = 0;
        new_task->task_tick = 0;
        new_task->task = task;
        //可能存在正在运行或等待运行的任务，放入bpk队列等待之后的运行
        task_t *this_task;
        list_for_each_entry(this_task, (&active_task_start), list) {
            if (this_task->led == led) {
            list_move_tail(&this_task->list, &(bpk_task_start));
            break;//bug记录,由于节点被移动，删除会导致进入bpk_task，从而导致死循环
        }
    }

    list_add((&(new_task->list)), (&active_task_start));

    } else while (1);

}
//void __led_init(void) {}

uint16_t get_min_tick(void) {
#define GET_DELAY_TIME(_task) _task->task_tick
    task_t *this_task;
    uint16_t time = _UI16_MAX;
    list_for_each_entry(this_task, (&active_task_start), list) {
        if (GET_DELAY_TIME(this_task) < time) {
            time = GET_DELAY_TIME(this_task);
        }
    }
    return time;
}

void led_handle(void) {

#define GET_Color(_task) _task->task[_task->task_index].color
    if (!list_empty(&active_task_start)) {
        static uint16_t time = 0;

        task_t *this_task;
        static task_t* will_gc_task=0;
        list_for_each_entry(this_task, (&active_task_start), list)
         {
            this_task->task_tick -= time;
            if (this_task->task_tick <= 0) {
                //超时执行更新数据任务
                uint32_t color = GET_Color(this_task);

                this_task->led->color = color;
                this_task->led->updata(this_task->led);
                //重载计数器
                this_task->task_tick = this_task->task[this_task->task_index].delayTime;//装载新的延时时间
                this_task->task_index++;
                if (this_task->task[this_task->task_index].color == 0xFF000000 &&
                    this_task->task[this_task->task_index].delayTime == 0) {

                    this_task->task_index = 0;
                    if (this_task->task_times != TASK_TIMES_FOREVER)this_task->task_times--;
                    if (this_task->task_times <= 0)
                    {
                        //销毁任务，销毁到回收区中
                        __list_del(this_task->list.prev, this_task->list.next);
                        if(will_gc_task!=0){
                            //有上一个要gc的task，先gc然后为这次的腾出位置
                            list_add_tail(&(will_gc_task->list),&gc_task_start);
                            will_gc_task=0;
                        }
                        will_gc_task = this_task;

                        //可能存在待恢复的任务(之前中断的)，现在恢复
                        task_t *thisTask;
                        list_for_each_entry(thisTask, (&bpk_task_start), list) {
                            if (thisTask->led == this_task->led) {
                                list_move(&thisTask->list, &(active_task_start));
                                break;
                            }
                        }
                    }
                } else {
                }
            } else {
            }

        }
        if(will_gc_task!=0){
            //有上一个要gc的task，将它丢入回收区
            list_add_tail(&(will_gc_task->list),&gc_task_start);
            will_gc_task =0;
        }
        time = get_min_tick();
        //printf("\t\tgc length=%d\n",list_length(&gc_task_start));

        dledDelay(time);
        //sleep(time);
#if 1
#else
        osDelay(time);
#endif

    }
}

void dled_gc_clear(void)
{
    task_t *Task;
//    list_for_each_entry(thisTask,&gc_task_start,list){
//        task_t *Task=0;
//        free(thisTask);
//
//
//    }
//    Task = list_entry(thisTask->list.next,task_t,list);
    while(gc_task_start.next  != &gc_task_start)
    {

        Task = list_entry(gc_task_start.next,task_t,list);
        list_del(gc_task_start.next);

        free(Task);
    }
    printf("cleared\n");

}



