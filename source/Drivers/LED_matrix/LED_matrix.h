#ifndef LED_matrix_H
#define LED_matrix_H

#include <stdint.h>
#include <stdbool.h>


void led_m_set_pixel_color_rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t row, uint8_t col);
void led_m_set_pixel_color(led_m_color_t color, uint8_t row, uint8_t col);
void led_m_set_brightness(led_m_brightness_t brightness);
void led_m_set_all_to_color(led_m_color_t color);
void led_m_init();

#endif /* LED_matrix_H */