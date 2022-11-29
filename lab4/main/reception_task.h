#ifndef _RECEPTION_TASK_
#define _RECEPTION_TASK_


#include "main.h"

/*
 * Will use the hardware RNG to generate random number to use as task ID
 *
 * The output number range is 0 -> FUNCTIONAL_TASK_NUM
 *
 * Input: param is a pointer to a QueueHandle_t, the queue to add request to
 */
void reception_task(void *param);

#endif
