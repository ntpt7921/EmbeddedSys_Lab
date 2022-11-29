#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "functional_task.h"
#include "main.h"
#include "request.h"
#include <inttypes.h>
#include <stdio.h>

// can cause race condition, use only in protected section
static inline uint8_t check_all_already_process()
{
    for (uint32_t i = 0; i < FUNCTIONAL_TASK_NUM; i++)
    {
        if (!already_process[i])
            return 0;
    }

    return 1;
}

// can cause race condition, use only in protected section
static inline void clear_already_process()
{
    for (uint32_t i = 0; i < FUNCTIONAL_TASK_NUM; i++)
    {
        already_process[i] = 0;
    }
}

static inline void wake_all_functional()
{
    for (uint32_t i = 0; i < FUNCTIONAL_TASK_NUM; i++)
    {
        if (eTaskGetState(func_task_handle_list[i]) == eSuspended)
        {
            vTaskResume(func_task_handle_list[i]);
        }
    }
}

// can cause race condition, use only in protected section
static inline void debug_print_already_process()
{
    for (uint32_t i = 0; i < FUNCTIONAL_TASK_NUM; i++)
    {
        printf("%" PRIu8 " ", already_process[i]);
    }
    printf("\n");
}

void functional_task(void *param)
{
    size_t id = (size_t) param;

    while (1)
    {
        Request_t new_req;
        if (xQueuePeek(queue, &new_req, portMAX_DELAY) == pdPASS)
        {
            if (new_req.forTaskID == id)
            {
                // remove the request from queue
                xQueueReceive(queue, &new_req, portMAX_DELAY); // should return success

                xSemaphoreTake(mt, portMAX_DELAY);
                clear_already_process();
                xSemaphoreGive(mt);

                // do something
                printf("Task with ID %zu checked the queue and run\n", id);
                // then wake up all other suspended task
                wake_all_functional();
            }
            else
            {
                // not match, set already_process to 1, then go into suspend to prevent
                // checking the request again
                xSemaphoreTake(mt, portMAX_DELAY);
                already_process[id] = 1;
                xSemaphoreGive(mt);

                vTaskSuspend(NULL);
                continue;
            }
        }
    }

    vTaskDelete(NULL);
}


void garbage_request_cleaner_task(void *param)
{
    while (1)
    {
        // check if all task has process the request
        xSemaphoreTake(mt, portMAX_DELAY);
        uint8_t all_checked = check_all_already_process();
        xSemaphoreGive(mt);

        if (all_checked)
        {
            Request_t garbage_reg;

            // remove the request from queue
            xQueueReceive(queue, &garbage_reg, portMAX_DELAY); // should return success

            xSemaphoreTake(mt, portMAX_DELAY);
            clear_already_process();
            xSemaphoreGive(mt);

            printf("Error: request not performed by any task\n");
            printf("Clean up garbage request %zu\n", garbage_reg.forTaskID);
            // then wake up all other suspended task
            wake_all_functional();
        }
    }

    vTaskDelete(NULL);
}
