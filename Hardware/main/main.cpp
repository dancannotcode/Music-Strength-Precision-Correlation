#include "esp.h"

#include <stdio.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_rom_sys.h"

// ---------------- HX711 ----------------
#define HX711_DT  GPIO_NUM_4
#define HX711_SCK GPIO_NUM_5

// ---------------- TMC2209 ----------------
#define STEP_PIN GPIO_NUM_6
#define DIR_PIN  GPIO_NUM_7
#define EN_PIN   GPIO_NUM_8 

#define STEPS_PER_REVOLUTION 200


int32_t hx711_read()
{
    int timeout = 1000;

    while (gpio_get_level(HX711_DT) == 1)
    {
        vTaskDelay(pdMS_TO_TICKS(1));

        if (--timeout <= 0)
        {
            printf("ERROR: HX711 not responding\n");
            return 0;
        }
    }

    uint32_t value = 0;

    for (int i = 0; i < 24; i++)
    {
        gpio_set_level(HX711_SCK, 1);
        esp_rom_delay_us(2);

        value = (value << 1) | gpio_get_level(HX711_DT);

        gpio_set_level(HX711_SCK, 0);
        esp_rom_delay_us(2);
    }

    gpio_set_level(HX711_SCK, 1);
    esp_rom_delay_us(2);

    gpio_set_level(HX711_SCK, 0);
    esp_rom_delay_us(2);
 
    if (value & 0x800000)
    {
        value |= 0xFF000000;
    }

    return (int32_t)value;
}


void rotate_one_revolution()
{
    printf("Motor starting 360 degree rotation...\n");

    // Enable TMC2209
    gpio_set_level(EN_PIN, 0);

    // Direction
    gpio_set_level(DIR_PIN, 1);

    for (int i = 0; i < STEPS_PER_REVOLUTION; i++)
    {
        gpio_set_level(STEP_PIN, 1);
        esp_rom_delay_us(1000);

        gpio_set_level(STEP_PIN, 0);
        esp_rom_delay_us(1000);
    }

    printf("Motor rotation complete.\n");

    // Disable motor
    gpio_set_level(EN_PIN, 1);
}

void graph(void *parameter);

extern "C" void app_main(void)
{
    // HX711 setup
    gpio_reset_pin(HX711_DT);
    gpio_reset_pin(HX711_SCK);

    gpio_set_direction(HX711_DT, GPIO_MODE_INPUT);
    gpio_set_direction(HX711_SCK, GPIO_MODE_OUTPUT);

    gpio_set_level(HX711_SCK, 0);

    // TMC2209 setup
    gpio_reset_pin(STEP_PIN);
    gpio_reset_pin(DIR_PIN);
    gpio_reset_pin(EN_PIN);

    gpio_set_direction(STEP_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(DIR_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(EN_PIN, GPIO_MODE_OUTPUT);

    gpio_set_level(STEP_PIN, 0);

    // Start motor disabled
    gpio_set_level(EN_PIN, 1);

    printf("\n=== HX711 + TMC2209 Test ===\n");

    // Give hardware time to start
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Read HX711 before moving
    int32_t raw_before = hx711_read();

    printf("HX711 before motor: %ld\n",
           (long)raw_before);

    // Rotate motor once
    rotate_one_revolution();

    // Start the controller task
    xTaskCreate(
        graph,
        "graph",
        4096,
        nullptr,
        5,
        nullptr
    );
}