#include "esp.h"
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"


#define TARE_RAW_VALUE     14000.0f  // 4 calibration
#define COUNTS_PER_POUND   14000.0f   // 4 calibration

int64_t startTime;

void setTime() {
    startTime = esp_timer_get_time();
}

double getTime() {
    return (esp_timer_get_time() - startTime) / 1000000.0;
}

void graph(void *parameter)
{
    setTime();
    double stopwatch = getTime();
    while (stopwatch < 30)
    {
        int32_t raw = hx711_read();

        float force_lbf =
            ((float)raw - TARE_RAW_VALUE) / COUNTS_PER_POUND;

        if (force_lbf < 0)
        {
            force_lbf = 0;
        }

        printf("HX711 Raw: %ld | Force: %.2f lbf | Elapsed Time: %.1f\n",
                (long)raw,
                force_lbf,
                stopwatch);


        stopwatch = getTime();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}