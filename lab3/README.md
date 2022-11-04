# Can `configUSE_PREEMPTION` and `configUSE_TIME_SLICING` and `configIDLE_SHOULD_YIELD` be changed in ESP-IDF?

- Short answer, for me, probably no

- Long answer:

	- [For `configUSE_PREEMPTION`](https://github.com/espressif/arduino-esp32/issues/3100)

	- [For `configUSE_TIME_SLICING`](https://www.esp32.com/viewtopic.php?t=20081)

	- Haven't search for yielding in idle, but probably same as above

# Idle task hook (callback), how to set it?

- There are ESP-IDF idle hook and FreeRTOS idle hook. What the difference?

	- The vanilla FreeRTOS one will be used in this lab

	- For ESP-IDF specific [idle and tick hook](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_additions.html#esp-idf-tick-and-idle-hooks).

- Idle task hook function will be called by the idle task (once per iteration)

	- Iteration probably means cycle.

	- Not sure if hook is guaranteed to run every time idle task go into running state

	- (not sure about this) Idle task probably implemented as a loop, and hook function will be call each loop iteration

- Limitation on the idle task hook

	- No blocking or suspending

	- Short execution time so that idle task can do other stuff

- Set it by

	- Enable `configUSE_IDLE_HOOK` in within `idf.py menuconfig`

	- Application must provide function `void vApplicationIdleHook(void)`

# Is it the `app_main()` in the program?

- On start up, various task is created and the scheduler will start running

- Those task are

	- Main task (main)

	- Idle task (IDLEX)

	- IPC task (IPCX)

- One of those task will call `main_task(void)` function

# Tickless mode in FreeRTOS?

# Low power state on ESP32?
