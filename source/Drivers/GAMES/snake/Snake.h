#pragma once
#include <stdint.h>
#include <stdlib.h>


typedef enum {
	SNAKE_EMPTY = 128,
	SNAKE_APPLE = 254,
	SNAKE_WALL = 255
}snake_board_token_t;

void snake_update();

void snake_begin_game();

void snake_move_cw();

void snake_move_ccw();

uint8_t* snake_get_board();

uint8_t snake_get_score();