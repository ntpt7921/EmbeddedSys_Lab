#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "sys/time.h"
#include <inttypes.h>
#include <stdio.h>

#define TIMER_MESSEAGE_1 "ahihi"
#define TIMER_MESSEAGE_2 "ihaha"
#define TIMER_ID_1 (0)
#define TIMER_ID_2 (1)
#define TIMER_REPEAT_1 (10)
#define TIMER_REPEAT_2 (5)
#define TIMER_PERIOD_1_MS (2000)
#define TIMER_PERIOD_2_MS (3000)

typedef struct
{
    uint32_t timer_elapsed_count;
    uint32_t id;
} Timer_info_t;

TimerHandle_t timer[2];
Timer_info_t info[2] = { 0 };

static inline void print_current_time()
{
    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    printf("%03lld:%ld\t", tv_now.tv_sec, tv_now.tv_usec);
}

void vTimerCallbackPrintMess(TimerHandle_t tim)
{
    Timer_info_t *info = (Timer_info_t*) pvTimerGetTimerID(tim);

    if (info->id == TIMER_ID_1)
    {
        print_current_time();
        printf(TIMER_MESSEAGE_1 "\n");
        info->timer_elapsed_count++;
        if (info->timer_elapsed_count == TIMER_REPEAT_1)
        {
            if (xTimerStop(tim, 0) == pdFAIL)
            {
                printf("Fail to stop timer 1\n");
            }
        }
    }
    else if (info->id == TIMER_ID_2)
    {
        print_current_time();
        printf(TIMER_MESSEAGE_2 "\n");
        info->timer_elapsed_count++;
        if (info->timer_elapsed_count == TIMER_REPEAT_2)
        {
            if (xTimerStop(tim, 0) == pdFAIL)
            {
                printf("Fail to stop timer 2\n");
            }
        }
    }
}

void app_main(void)
{
    info[0].timer_elapsed_count = 0;
    info[0].id = TIMER_ID_1;
    timer[0] = xTimerCreate("Timer 1", pdMS_TO_TICKS(TIMER_PERIOD_1_MS),
            pdTRUE, (void*) &info[0], vTimerCallbackPrintMess);

    info[1].timer_elapsed_count = 0;
    info[1].id = TIMER_ID_2;
    timer[1] = xTimerCreate("Timer 2", pdMS_TO_TICKS(TIMER_PERIOD_2_MS),
            pdTRUE, (void*) &info[1], vTimerCallbackPrintMess);

    for (uint32_t i = 0; i < 2; i++)
    {
        if (timer[i] == NULL)
        {
            printf("Timer %" PRIu32 " failed to be created\n", i + 1);
        }
        else
        {
            if (xTimerStart(timer[i], 0) == pdFAIL)
            {
                printf("Timer %" PRIu32 " failed to start\n", i + 1);
            }
        }

    }

    vTaskDelete(NULL);
}
