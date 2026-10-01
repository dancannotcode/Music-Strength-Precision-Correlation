#include "esp.h"
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define TARE_RAW_VALUE     14000.0f  // 4 calibration
#define COUNTS_PER_POUND   14000.0f   // 4 calibration

void graph(void *parameter)
{
    while (true)
    {
        int32_t raw = hx711_read();

        float force_lbf =
            ((float)raw - TARE_RAW_VALUE) / COUNTS_PER_POUND;

        if (force_lbf < 0)
        {
            force_lbf = 0;
        }

        printf("HX711 Raw: %ld | Force: %.2f lbf\n",
               (long)raw,
               force_lbf);



        vTaskDelay(pdMS_TO_TICKS(200));
    }
}