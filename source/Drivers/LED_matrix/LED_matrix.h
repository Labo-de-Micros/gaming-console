
#ifndef LED_matrix_H
#define LED_matrix_H

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

// Pixel data structure for each led
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} LED_M_pixel_t;

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Default hard-coded color palette
#define LED_M_COLOR_RED      { 50, 0, 0 }
#define LED_M_COLOR_BLUE     { 0, 0, 50 }
#define LED_M_COLOR_GREEN    { 0, 50, 0 }
#define LED_M_COLOR_WHITE    { 50, 50, 50 }

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*
 * @brief Initializes the WS2812 driver
 */
void WS2812Init(void);

/*
 * @brief Sets display mirroring buffer
 * @param buffer  Array of pixels
 * @param size    Number of pixels in array
 */
void WS2812SetDisplayBuffer(LED_M_pixel_t* buffer, size_t size);

/*
 * @brief Mirrors display buffer to the array of WS2812 leds
 */
void WS2812Update(void);

/*******************************************************************************
 ******************************************************************************/


#endif /* LED_matrix_H */