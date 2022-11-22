#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <inttypes.h>
#include <stdio.h>
#include <sys/time.h>

static uint32_t uxLastTaskID = -1u;
static uint32_t uxLastTaskID_old = -1u;
static TickType_t xLastIdleEnterTick = 0;

#define EXPONENTIAL_WEIGHT_PAST 0.9f
#define EXPONENTIAL_WEIGHT_CURRENT 0.1f
static float ultilization = 0;

// Idle hook
// NOTE: configIDLE_SHOULD_YIELD needs to be set to 0
void vApplicationIdleHook(void)
{
    uxLastTaskID = uxLastTaskID_old = 0u;
    TickType_t xCurrentIdleEnterTick = xTaskGetTickCount();

    // the utilization during the period (xLastIdleEnterTick -> xCurrentIdleEnterTick)
    // is 1 - 1 / (xCurrentIdleEnterTick - xLastIdleEnterTick)
    // this is because the idle task will take up only one tick during that period
    float ultilizationDuringPeriod;
    if (xCurrentIdleEnterTick == xLastIdleEnterTick)
    {
        ultilizationDuringPeriod = 0;
    }
    else
    {
        ultilizationDuringPeriod = 1.0f - 1.0f / (xCurrentIdleEnterTick - xLastIdleEnterTick);
    }

    // update the total ultilization
    // the formula is of exponential moving average (smoothing)
    // https://en.wikipedia.org/wiki/Exponential_smoothing
    ultilization = ultilization * EXPONENTIAL_WEIGHT_PAST
        + ultilizationDuringPeriod * EXPONENTIAL_WEIGHT_CURRENT;

    xLastIdleEnterTick = xCurrentIdleEnterTick;
}

// compare the last task id with the current task id
// if different, some new task is running in the current tick, notify the change
// if not different, do nothing
void printCurrentState(void)
{
    if (uxLastTaskID != uxLastTaskID_old)
    {
        uxLastTaskID_old = uxLastTaskID;

        // print out the current time and tick count and task id
        struct timeval tv_now;
        gettimeofday(&tv_now, NULL);
        printf("%lld.%06ld\tTick: %04lu, ID: %lu\n",
               tv_now.tv_sec, tv_now.tv_usec, xTaskGetTickCount(), uxLastTaskID);
    }
}

void vTestTask(void *param)
{
    while (1)
    {
        uxLastTaskID = (uint32_t) param;
        printCurrentState();

        // run a lot
        printf("Run a lot\n");
        for (BaseType_t i = 0; i < 10000000u; i++)
        {
            __asm__ ("NOP"); // nop - do nothing
        }
        printf("Ultilization: %f%%\n", ultilization);
        vTaskDelay(3); // change this to see change in utilization
    }

    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(vTestTask, "Test task", 2048, (void*) 2, 1, NULL);

    vTaskDelete(NULL);
}
