#ifndef ESP_H
#define ESP_H

#include <stdint.h>

#ifndef HX711_H
#define HX711_H

#include <stdint.h>

// Reads and returns the HX711 raw value
int32_t hx711_read();

// Rotates the motor one revolution
void rotate_one_revolution();

#endif

#endif