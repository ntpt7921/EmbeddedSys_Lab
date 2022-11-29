#include "functional_task.h"
#include "main.h"
#include "reception_task.h"
#include "request.h"
#include <stdio.h>

QueueHandle_t queue;
SemaphoreHandle_t mt;
uint8_t already_process[FUNCTIONAL_TASK_NUM] = { 0 };
TaskHandle_t func_task_handle_list[FUNCTIONAL_TASK_NUM];

void app_main(void)
{
    queue = xQueueCreate(sizeof(Request_t), QUEUE_SIZE);
    mt = xSemaphoreCreateMutex();

    xTaskCreate(reception_task, "Reception task", 2048, NULL,
            1, NULL);

    for (size_t i = 0; i < FUNCTIONAL_TASK_NUM; i++)
    {
        xTaskCreate(functional_task, "Functional task", 2048, (void*) i,
                1, &func_task_handle_list[i]);
    }

    xTaskCreate(garbage_request_cleaner_task, "Cleanup task", 2048, NULL,
            1, NULL);

    vTaskDelete(NULL);
}
