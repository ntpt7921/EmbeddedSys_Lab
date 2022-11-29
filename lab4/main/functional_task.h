#ifndef _FUNCTIONAL_TASK_
#define _FUNCTIONAL_TASK_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "main.h"
#include <stdint.h>

void functional_task(void *param);
void garbage_request_cleaner_task(void *param);

#endif
