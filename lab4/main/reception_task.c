#include "bootloader_random.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "main.h"
#include "reception_task.h"
#include "request.h"
#include <stdio.h>
#include <inttypes.h>

void reception_task(void *param)
{
    bootloader_random_enable();

    while (1)
    {
        size_t random_task_id = esp_random() % (FUNCTIONAL_TASK_NUM + ADDED_GARBAGE_REQUEST);

        Request_t new_req =
        {
            .forTaskID = random_task_id
        };

        if  (xQueueSend(queue, &new_req, portMAX_DELAY) == pdPASS)
        {
            // success, do nothing
            printf("Dispatch request for task ID %zu\n", random_task_id);
        }
        else
        {
            // fail to add request to queue, print error
            printf("Error: reception_task can't add request to queue\n");
            printf("New req: %zu", new_req.forTaskID);
        }
    }

    bootloader_random_disable();

    vTaskDelete(NULL);
}
