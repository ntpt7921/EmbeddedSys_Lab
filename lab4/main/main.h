#ifndef _MAIN_H_
#define _MAIN_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#define FUNCTIONAL_TASK_NUM (5)
#define ADDED_GARBAGE_REQUEST (5)
#define QUEUE_SIZE (FUNCTIONAL_TASK_NUM)

// global queue
extern QueueHandle_t queue;
// mutex to avoid race condition when accessing queue and already_process
// the esp32 have two core after all
extern SemaphoreHandle_t mt;
// global uint8_t array that serve as bool, each task will set the corresponding element to signify
// that the signal has been read by the task and determined not to process it
extern uint8_t already_process[FUNCTIONAL_TASK_NUM];
// task handle to selectively resume functional task
extern TaskHandle_t func_task_handle_list[FUNCTIONAL_TASK_NUM];

#endif

