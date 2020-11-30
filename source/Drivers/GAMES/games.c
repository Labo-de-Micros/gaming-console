#include "games.h"
#include "Tetris.h"
#include "Snake.h"
#include "Pong.h"
#include <stdlib.h>
#include <stdint.h>
#include "../LED_matrix/LED_matrix.h"
#include "../timer/timer.h"


static uint8_t state;
static tim_id_t games_timer_id;

static led_m_color_t colors[]={
	{10,10,10},
	{242,5,159},
	{5,219,242},
	{4,217,79},
	{242,203,5},
	{242,138,5},
	{191,27,27},
	{103,86,140}
};


static uint8_t tetris_splash[] =      { 1,1,1,1,1,1,1,1,1,1,
                                        1,1,1,1,1,1,1,1,1,1,
                                        1,1,0,0,0,0,0,0,1,1,
                                        1,1,0,0,0,0,0,0,1,1,
                                        1,1,1,1,0,0,1,1,1,1,
                                        1,1,1,1,0,0,1,1,1,1,
                                        1,1,1,1,0,0,1,1,1,1,
                                        1,1,1,1,0,0,1,1,1,1, 
                                        1,1,1,1,1,1,1,1,1,1 };

static uint8_t snake_splash[] = {       2,2,2,2,2,2,2,2,2,2,
                                        2,2,2,2,2,2,2,2,2,2,
                                        2,2,0,0,0,0,0,0,2,2,
                                        2,2,0,2,2,2,2,2,2,2,
                                        2,2,0,0,0,0,0,0,2,2,
                                        2,2,0,0,0,0,0,0,2,2,
                                        2,2,2,2,2,2,2,0,2,2,
                                        2,2,0,0,0,0,0,0,2,2,
                                        2,2,2,2,2,2,2,2,2,2 };

static uint8_t pong_splash[] = {        3,3,3,3,3,3,3,3,3,3,
                                        3,3,3,3,3,3,3,3,3,3,
                                        3,3,0,0,0,0,0,0,3,3,
                                        3,3,0,3,3,3,3,0,3,3,
                                        3,3,0,3,3,3,3,0,3,3,
                                        3,3,0,0,0,0,0,0,3,3,
                                        3,3,0,0,3,3,3,3,3,3,
                                        3,3,0,0,3,3,3,3,3,3,
                                        3,3,3,3,3,3,3,3,3,3 };


void load_splash(uint8_t* splash);
void games_state_change(uint8_t state);


void games_init() {
    games_state_change(MENU_SELECT_TETRIS);
    games_timer_id=timerGetId();
    timerStart(games_timer_id,TIMER_MS2TICKS(100),TIM_MODE_PERIODIC, games_update_handler);
}

void games_state_change(uint8_t nstate) {
    state = nstate;
    switch (nstate) {
    case MENU_SELECT_TETRIS:
        load_splash(tetris_splash);
        break;
    case MENU_SELECT_SNAKE:
        load_splash(snake_splash);
        break;
    case MENU_SELECT_PONG:
        load_splash(pong_splash);
        break;
    case MENU_PLAY_TETRIS:
        //timer de update tetris
        tetris_init();
        tetris_begin_game();
        break;
    case MENU_PLAY_SNAKE:
        //timer de update snake
        snake_begin_game();
        break;
    case MENU_PLAY_PONG:
        //timer de update snake
        pong_begin_game();
        break;
    default:

        break;
    }
}

uint8_t games_get_state() {
    return state;
}

void games_state_machine(uint8_t event) {
    switch (state) {
    case MENU_SELECT_TETRIS:
        switch (event) {
        case ACTION_CW:
            games_state_change(MENU_SELECT_SNAKE);
            break;
        case ACTION_CCW:
            break;
        case ACTION_PRESS:
            games_state_change(MENU_PLAY_TETRIS);
            break;
        case ACTION_DOUBLE_PRESS:
            break;
        case ACTION_LONG_PRESS:
            break;
        }
        break;
    case MENU_SELECT_SNAKE:
        switch (event) {
        case ACTION_CW:
            games_state_change(MENU_SELECT_PONG);
            break;
        case ACTION_CCW:
            games_state_change(MENU_SELECT_TETRIS);
            break;
        case ACTION_PRESS:
            games_state_change(MENU_PLAY_SNAKE);
            break;
        case ACTION_DOUBLE_PRESS:
            break;
        case ACTION_LONG_PRESS:
            break;
        }
        break;
    case MENU_SELECT_PONG:
        switch (event) {
        case ACTION_CW:
            break;
        case ACTION_CCW:
            games_state_change(MENU_SELECT_SNAKE);
            break;
        case ACTION_PRESS:
            games_state_change(MENU_PLAY_PONG);
            break;
        case ACTION_DOUBLE_PRESS:
            break;
        case ACTION_LONG_PRESS:
            break;
        }
        break;
    case MENU_PLAY_TETRIS:
        switch (event) {
        case ACTION_CW:
            tetris_move(TETRIS_RIGHT);
            break;
        case ACTION_CCW:
            tetris_move(TETRIS_LEFT);
            break;
        case ACTION_PRESS:
            tetris_rotate();
            break;
        case ACTION_DOUBLE_PRESS:
            games_state_change(MENU_SELECT_TETRIS);
            break;
        case ACTION_LONG_PRESS:
            tetris_move(TETRIS_DROP);
            break;
        }
        break;
    case MENU_PLAY_SNAKE:
        switch (event) {
        case ACTION_CW:
            snake_move_cw();
            break;
        case ACTION_CCW:
            snake_move_ccw();
            break;
        case ACTION_PRESS:
            break;
        case ACTION_DOUBLE_PRESS:
            games_state_change(MENU_SELECT_SNAKE);
            break;
        case ACTION_LONG_PRESS:
            break;
        }
        break;
    case MENU_PLAY_PONG:
        switch (event) {
        case ACTION_CW:
            pong_move(PONG_RIGHT);
            break;
        case ACTION_CCW:
            pong_move(PONG_LEFT);
            break;
        case ACTION_PRESS:
            break;
        case ACTION_DOUBLE_PRESS:
            games_state_change(MENU_SELECT_PONG);
            break;
        case ACTION_LONG_PRESS:
            break;
        }
        break;
    }
}

void games_update_handler(){
        switch (games_get_state()) {
        case MENU_PLAY_TETRIS:
            tetris_update_board();
            led_m_set_all_to_color(Black);
            print_tetris_board_to_graphics(tetris_get_board(), true);
            print_tetris_board_to_graphics(tetris_get_piece_board(), false);
            break;
        case MENU_PLAY_SNAKE:
            snake_update();
            led_m_set_all_to_color(Black);
            print_snake_board_to_graphics(snake_get_board(), true);
            break;
        case MENU_PLAY_PONG:
            pong_update_game();
            led_m_set_all_to_color(Black);
            print_pong_board_to_graphics(pong_get_board(), true);
            break;
        default:
            break;
        }
}

void print_tetris_board_to_graphics(uint8_t* board, bool print_blank) {
    for (int col = 1; col < COL_SIZE + 1; col++)
        for (int row = 1; row < ROW_SIZE + 1; row++)
            if (board[row * BOARD_X + col] || (!board[row * BOARD_X + col] && print_blank))
                led_m_set_pixel_color(colors[board[row * BOARD_X + col]], row - 1, col - 1);
    return;
}

void print_snake_board_to_graphics(uint8_t* board, bool print_blank) {
    for (int col = 1; col < COL_SIZE + 1; col++)
        for (int row = 1; row < ROW_SIZE + 1; row++)
            if (board[row * BOARD_X + col] != SNAKE_EMPTY || (board[row * BOARD_X + col] == SNAKE_EMPTY && print_blank)) {
                switch (board[row * BOARD_X + col]) {
                case SNAKE_APPLE:
                    led_m_set_pixel_color_rgb(166,79,3 , row - 1, col - 1);
                    break;
                case SNAKE_EMPTY:
                    led_m_set_pixel_color_rgb(10,10,10 , row - 1, col - 1);
                    break;
                default:
                    led_m_set_pixel_color_rgb( 5,89,2 , row - 1, col - 1);
                    break;
                }
            }
    return;
}

void print_pong_board_to_graphics(uint8_t* board, bool print_blank) {
    for (int col = 1; col < COL_SIZE + 1; col++)
        for (int row = 1; row < ROW_SIZE + 1; row++)
            if (board[row * BOARD_X + col] != SNAKE_EMPTY || (board[row * BOARD_X + col] == SNAKE_EMPTY && print_blank)) {
                switch (board[row * BOARD_X + col]) {
                case PONG_BOARD_PONG:
                    led_m_set_pixel_color_rgb(156,34,38, row - 1, col - 1);
                    break;
                case PONG_BOARD_EMPTY:
                    led_m_set_pixel_color_rgb(10,10,10, row - 1, col - 1);
                    break;
                case PONG_BOARD_BAR:
                    led_m_set_pixel_color_rgb(80,80,80, row - 1, col - 1);
                    break;
                case PONG_BOARD_BRICK:
                    led_m_set_pixel_color(colors[row], row - 1, col - 1);
                    break;
                default:
                    break;
                }
            }
    return;
}

void load_splash(uint8_t* splash) {
    print_tetris_board_to_graphics(splash, true);
}
