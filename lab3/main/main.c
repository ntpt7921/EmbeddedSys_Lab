#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <inttypes.h>
#include <stdio.h>
#include <sys/time.h>

// choosing one option and define that, undef the other
// don't forget changing option in FreeRTOSConfig.h
#define POLICY_PREEMPTIVE
#undef POLICY_COOPERATIVE

#define GPIO_BUTTON GPIO_NUM_12
#define SHORT_TASK_LOOPS_COUNT 2000000u
#define YIELD_TASK_LOOPS_COUNT 2000000u

static uint32_t uxLastTaskID = -1u;
static uint32_t uxLastTaskID_old = -1u;

// Idle hook
void vApplicationIdleHook(void)
{
    uxLastTaskID = uxLastTaskID_old = 0u;
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



#ifdef POLICY_PREEMPTIVE
static TaskHandle_t contTask01_handle = NULL;
static TaskHandle_t contTask02_handle = NULL;
static TaskHandle_t eventTask03_handle = NULL;

void IRAM_ATTR button_press_handler(void* arg)
{
    xTaskResumeFromISR(eventTask03_handle);
}

void initializeGPIO(void)
{
    // set the pin to be input
    gpio_reset_pin(GPIO_BUTTON);
    gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT);
    // set the pin to have internal pullup
    gpio_set_pull_mode(GPIO_BUTTON, GPIO_PULLUP_ONLY);
    gpio_pullup_en(GPIO_BUTTON);
    //set up the interrupt
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_set_intr_type(GPIO_BUTTON, GPIO_INTR_NEGEDGE); // on negative edge
    gpio_isr_handler_add(GPIO_BUTTON, button_press_handler, NULL); // no paramenter for isr
    gpio_intr_enable(GPIO_BUTTON);
}

void vContinuousTask(void *param)
{
    while (1)
    {
        uxLastTaskID = (uint32_t) param;
        printCurrentState();
    }

    vTaskDelete(NULL);
}

void vEventTask(void *param)
{
    // suspend task upon creation, will be woke up by button
    vTaskSuspend(NULL);

    while (1)
    {
        printf("Event task %lu start running\n", (uint32_t) param);

        for (UBaseType_t i = 0; i < SHORT_TASK_LOOPS_COUNT; i++)
        {
            uxLastTaskID = (uint32_t) param;
            printCurrentState();
        }

        // go into suspend, the button interrupt will wake this up again
        printf("Event task %lu go into suspend\n", (uint32_t) param);
        vTaskSuspend(NULL);
    }
    vTaskDelete(NULL);
}
#endif



#ifdef POLICY_COOPERATIVE
static TaskHandle_t yieldTask01_handle = NULL;
static TaskHandle_t yieldTask02_handle = NULL;
static TaskHandle_t yieldTask03_handle = NULL;
static TaskHandle_t yieldTask04_handle = NULL;

void IRAM_ATTR button_press_handler(void* arg)
{
    xTaskResumeFromISR(yieldTask01_handle);
    xTaskResumeFromISR(yieldTask02_handle);
    xTaskResumeFromISR(yieldTask03_handle);
    xTaskResumeFromISR(yieldTask04_handle);
}

void initializeGPIO(void)
{
    // set the pin to be input
    gpio_reset_pin(GPIO_BUTTON);
    gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT);
    // set the pin to have internal pullup
    gpio_set_pull_mode(GPIO_BUTTON, GPIO_PULLUP_ONLY);
    gpio_pullup_en(GPIO_BUTTON);
    //set up the interrupt
    gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);
    gpio_set_intr_type(GPIO_BUTTON, GPIO_INTR_NEGEDGE); // on negative edge
    gpio_isr_handler_add(GPIO_BUTTON, button_press_handler, NULL); // no paramenter for isr
    gpio_intr_enable(GPIO_BUTTON);
}

// these task run for a while (YIELD_TASK_LOOPS_COUNT) and then yield
void vTaskWithYield(void *param)
{
    uint32_t taskID = (uint32_t) param;
    // delay starting the task by taskid * 10 ticks
    vTaskDelay(10 * taskID);

    while (1)
    {
        for (UBaseType_t i = 0; i < YIELD_TASK_LOOPS_COUNT; i++)
        {
            uxLastTaskID = taskID;
            printCurrentState();
        }

        if (taskID > 1)
        {
            // suspend higher priority task to simulate them going into blocked state
            // they will be resume on button press
            vTaskSuspend(NULL);
        }
        else
        {
            taskYIELD();
        }
    }

    vTaskDelete(NULL);
}
#endif



void app_main(void)
{
    // increase priority of current task so newly created task
    // doesn't hog cpu until finish creating all task
    vTaskPrioritySet(NULL, 10);

    // set up gpio for button and interrupt
    initializeGPIO();

#ifdef POLICY_PREEMPTIVE
    // two continuous task, equal priority
    xTaskCreate(vContinuousTask, "Continous task #1", 2048, (void*) 1u, 1, &contTask01_handle);
    xTaskCreate(vContinuousTask, "Continous task #2", 2048, (void*) 2u, 1, &contTask02_handle);

    // one event task
    xTaskCreate(vEventTask, "Event task #3", 2048, (void*) 3, 2, &eventTask03_handle);
#endif

#ifdef POLICY_COOPERATIVE
    // four task with yield, increasing priority, id = priority
    xTaskCreate(vTaskWithYield, "Yielding task #1", 2048, (void*) 1u, 1, &yieldTask01_handle);
    xTaskCreate(vTaskWithYield, "Yielding task #2", 2048, (void*) 2u, 2, &yieldTask02_handle);
    xTaskCreate(vTaskWithYield, "Yielding task #3", 2048, (void*) 3u, 3, &yieldTask03_handle);
    xTaskCreate(vTaskWithYield, "Yielding task #4", 2048, (void*) 4u, 4, &yieldTask04_handle);
#endif

    vTaskDelete(NULL);
}
