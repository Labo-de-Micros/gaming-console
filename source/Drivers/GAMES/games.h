#pragma once

#include <stdbool.h>
 
typedef enum {
    ACTION_CW,
    ACTION_CCW,
    ACTION_PRESS,
    ACTION_DOUBLE_PRESS,
    ACTION_LONG_PRESS
};

typedef enum {
    MENU_SELECT_TETRIS,
    MENU_PLAY_TETRIS,
    MENU_SELECT_SNAKE,
    MENU_PLAY_SNAKE,
    MENU_SELECT_PONG,
    MENU_PLAY_PONG
}menu_states_t;



uint8_t games_get_state();

void print_tetris_board_to_graphics(uint8_t* board, bool print_blank);

void print_snake_board_to_graphics(uint8_t* board, bool print_blank);

void games_state_machine(uint8_t event);

void games_init();