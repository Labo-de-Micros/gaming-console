#pragma once
#include <stdint.h>

typedef enum {
	PONG_RIGHT,
	PONG_LEFT
}pong_direction_t;

typedef enum {
	PONG_BOARD_EMPTY,
	PONG_BOARD_BAR,
	PONG_BOARD_WALL,
	PONG_BOARD_FLOOR,
	PONG_BOARD_PONG,
	PONG_BOARD_BRICK
}pong_board_token_t;

void pong_begin_game();
void pong_next_level();
uint8_t * pong_get_board();
uint8_t pong_get_score();
void pong_update_game();
void pong_move(pong_direction_t direction);

