#include <stdio.h>
#include "../src/dled.h"
#include <pthread.h>


void updata(dled_t *led) {
    printf("%s ", led->name);
    printf("color:%d \n", led->color);
}

dled_t led = {
        .name="led0",
        .color=0xff,
        .updata=updata,
        .userdata=0,
};
dled_t led1 = {
        .name="led1",
        .color=0xff,
        .updata=updata,
        .userdata=0,
};
dled_task_t task[] = {
        {.color=10, .delayTime=500},
        {.color=02, .delayTime=400},
        {.color=03, .delayTime=300},
        {.color=04, .delayTime=100},
        TASK_END
};

dled_task_t task1[] = {
        {.color=110, .delayTime=500},
        {.color=12, .delayTime=400},
        {.color=13, .delayTime=300},
        {.color=14, .delayTime=100},
        TASK_END
};

int * thread(void * arg)
{
    pthread_t newthid;

    newthid = pthread_self();
    printf("this is a new thread, thread ID = %d\n", newthid);
    led_create_task(&led, task, 2);

    return 0;
}

int main() {
    //printf("Hello, World!\n");
    led_create_task(&led1,task,1);
    led_create_task(&led, task1, 1);

    pthread_t thid;
    if(pthread_create(&thid, 0, (void *)thread, 0) != 0) {
        printf("thread creation failed\n");
    }

    while (1) {
        //printf("run\n");
        led_handle();
    }

    return 0;
}
