/*
 * @Description: 移植Linux2.6内核list.h
 * @Version: V1.0
 * @Autor: https://blog.csdn.net/qq_16933601
 * @Date: 2020-09-12 22:54:51
 * @LastEditors: Carlos
 * @LastEditTime: 2020-09-16 00:35:17
 */
#ifndef DLED_H
#define DLED_H

#include <stdint.h>
#include "list.h"


#define TASK_END         {.color=0xFF000000,.delayTime=0}
#define TASK_TIMES_FOREVER     (0xffff)

typedef struct dled_t {
    char *name;
    uint32_t color;

    void (*updata)(struct dled_t *led);

    void *userdata;
} dled_t;

typedef struct {
    uint32_t color;
    uint16_t delayTime;
} dled_task_t;



//typedef enum {
//    easyLed_Once = 0U,
//    easyLed_Circulation,
//}easyLed_Mode_e;


static void inline dledDelay(uint32_t time) {
    //由用户编写对应的延时函数，单位是tick
}

void led_create_task(dled_t *led, dled_task_t task[], uint16_t task_times);

void led_handle(void);


#endif