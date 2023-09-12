# dled
## This is a simple to use led flicker or other production line library.
It can not only easily complete the fast flashing slow flashing lights 
but also can be used for other tasks that need to be completed periodically, 
such as buzzers, industrial lines, etc

## usage

### first: create led/actuator 
```
//create low level operation
void updata(dled_t *led) {
    printf("%s ", led->name);
    printf("color:%d \n", led->color);
}
//create actuator
dled_t led = {
        .name="led0", 
        .color=0xff,
        .updata=updata,
        .userdata=0,
};
```
### second: create task sequence 
**the TASK_END must have and in the end** 
```
dled_task_t task[] = {
        {.color=10, .delayTime=500},
        {.color=02, .delayTime=400},
        {.color=03, .delayTime=300},
        {.color=04, .delayTime=100},
        TASK_END
};
```
### third: link task to dled's control and Recursive call led_handle();
```
    led_create_task(&led, task, 2);

    while (1) {
        led_handle();
    }
```
Good job,enjoy youself.