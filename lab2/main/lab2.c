#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

// change this to define to print the free stack
#undef PRINT_FREE_STACK
#define STUDENT_ID 1912059
#define GPIO_BUTTON GPIO_NUM_12

static TaskHandle_t printID_handle = NULL;
static TaskHandle_t detectButton_handle = NULL;

void vPrintStudentID(void *pvParameters)
{
    while (1)
    {
        printf("Student ID: %d\n", STUDENT_ID);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void vDetectButtonPressAndPrint(void *pvParameters)
{
    int currentState = 0;
    int lastState = 0;
    int diffCount = 0;
    while (1)
    {
        currentState = gpio_get_level(GPIO_BUTTON);
        if (currentState != lastState)
            diffCount++;

        if (diffCount >= 5)
        {
            diffCount = 0;
            lastState = currentState;
            if (currentState == 0)
            printf("ESP32\n");
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void initializeGPIO(void)
{
    // set the pin to be input
    gpio_reset_pin(GPIO_BUTTON);
    gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT);
    // set the pin to have internal pullup
    gpio_set_pull_mode(GPIO_BUTTON, GPIO_PULLUP_ONLY);
    gpio_pullup_en(GPIO_BUTTON);
}

void app_main(void)
{
    initializeGPIO();

    xTaskCreate(
        vPrintStudentID,
        "Print student ID",
        2048,
        NULL,
        1,
        &printID_handle
    );

    xTaskCreate(
        vDetectButtonPressAndPrint,
        "Detect button press and print ESP32 if so",
        1024,
        NULL,
        1,
        &detectButton_handle
    );

#ifdef PRINT_FREE_STACK
    while (1)
    {
        printf("%u\n", uxTaskGetStackHighWaterMark(printID_handle));
        printf("%u\n", uxTaskGetStackHighWaterMark(detectButton_handle));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
#endif
}
