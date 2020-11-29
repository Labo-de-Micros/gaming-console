#ifndef LED_matrix_H
#define LED_matrix_H

#include <stdint.h>
#include <stdbool.h>

void led_m_init();
void led_m_set_pixel(uint8_t color, uint8_t brightness, uint8_t row, uint8_t col);
static void led_m_set_pixel_brightness(uint16_t *ptr, uint8_t brightness);

#endif /* LED_matrix_H */