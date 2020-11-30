#pragma once
#include <stdint.h>
#include <stdbool.h>

#define BOARD_X 10
#define BOARD_Y 10



typedef enum {
	EMPTY, T1, T2, T3, T4, T5, T6, T7, LINE, BORDER
}tetris_board_token;

typedef enum {
	TETRIS_LEFT,
	TETRIS_RIGHT,
	TETRIS_DOWN,
	TETRIS_DROP
}tetris_direction_t;

typedef enum {
	TETRIS_IDLE_ST,
	TETRIS_RUNNING_ST,
	TETRIS_PAUSED_ST,
	TETRIS_GAME_OVER_ST
}tetris_status_t;


void tetris_init();

void tetris_begin_game(void);

uint8_t* tetris_get_board();

uint8_t* tetris_get_piece_board();

void tetris_move(tetris_direction_t dir);

void tetris_rotate();

void tetris_update_board(void);

tetris_status_t tetris_get_status();

uint16_t tetris_get_score();


