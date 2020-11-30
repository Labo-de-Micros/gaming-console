#include "Pong.h"
#include "stdint.h"


#define BOARD_X 10
#define BOARD_Y 10
#define PONG_INIT_LENGTH 2


// Enums 

typedef enum {
	PONG_NO_COLLISION,
	PONG_COLLISION,
	PONG_BRICK_COLLISION
}pong_collision_t;

typedef enum {
	PONG_RUNNING,
	PONG_NEXT_LEVEL,
	PONG_GAME_OVER
}pong_status_t;


// Static function headers

static void set_level_bricks(uint8_t level);
static void clean_board(void);
static void set_board_boundaries();
static uint8_t get_remaining_bricks();
static pong_status_t pong_update();

// Structs 

typedef struct {
	uint8_t* board;
	uint16_t score;

	uint16_t elapsed_ticks;
	uint8_t tick_speed;

	uint8_t x_pos;
	uint8_t y_pos;

	int8_t x_speed;
	int8_t y_speed;

	uint8_t bar_x_pos;
	uint8_t bar_length;

	uint8_t level;

	pong_status_t game_status;

	bool direction_changed;
}pong_game_t;

// Static variable declarations

static pong_game_t pong_game;
static uint8_t board[BOARD_X * BOARD_Y];

// External function declarations

void pong_begin_game() {
	uint8_t index;
	pong_game.board = board;
	pong_game.bar_x_pos = BOARD_X/2-1;
	pong_game.bar_length = 2;
	pong_game.game_status = PONG_RUNNING;
	pong_game.score = 0;
	pong_game.x_pos = BOARD_X / 2;
	pong_game.y_pos = BOARD_Y / 2;
	pong_game.x_speed = 1;
	pong_game.y_speed = -1;
	pong_game.elapsed_ticks = 0;
	pong_game.tick_speed = 20;
	pong_game.level = 0;
	clean_board();
	set_board_boundaries();
	for (index = 0; index < pong_game.bar_length; index++)
		pong_game.board[(BOARD_Y-2) * BOARD_X + pong_game.bar_x_pos + index] = PONG_BOARD_BAR;
	set_level_bricks(pong_game.level);
}

void pong_next_level() {
	uint8_t index;
	pong_game.bar_x_pos = BOARD_X / 2 - 1;
	pong_game.bar_length = 2;
	pong_game.game_status = PONG_RUNNING;
	pong_game.score = 0;
	pong_game.x_pos = BOARD_X / 2;
	pong_game.y_pos = BOARD_Y / 2;
	pong_game.x_speed = 1;
	pong_game.y_speed = -1;
	pong_game.elapsed_ticks = 0;
	pong_game.level++;
	clean_board();
	set_board_boundaries();
	for (index = 0; index < pong_game.bar_length; index++)
		pong_game.board[(BOARD_Y - 2) * BOARD_X + pong_game.bar_x_pos + index] = PONG_BOARD_BAR;
	set_level_bricks(pong_game.level);
}

uint8_t* pong_get_board() {
	return pong_game.board;
}

uint8_t pong_get_score() {
	return pong_game.score;
}

void pong_update_game() {
	switch (pong_update()) {
	case PONG_RUNNING:
		break;
	case PONG_NEXT_LEVEL:
		pong_next_level();
		break;
	case PONG_GAME_OVER:
		pong_begin_game();
		break;
	default:
		break;
	}
}

void pong_move(pong_direction_t direction) {
	uint8_t index;
	for (index = 0; index < pong_game.bar_length; index++)
		pong_game.board[BOARD_X * (BOARD_Y - 2) + pong_game.bar_x_pos + index] = PONG_BOARD_EMPTY;

	switch (direction) {
	case PONG_LEFT:
		if (pong_game.board[BOARD_X * (BOARD_Y - 2) + pong_game.bar_x_pos - 1] == PONG_BOARD_EMPTY)
			pong_game.bar_x_pos--;
		break;
	case PONG_RIGHT:
		if (pong_game.board[BOARD_X * (BOARD_Y - 2) + pong_game.bar_x_pos + pong_game.bar_length] == PONG_BOARD_EMPTY)
			pong_game.bar_x_pos++;
		break;
	default:
		break;
	}

	for (index = 0; index < pong_game.bar_length; index++)
		pong_game.board[BOARD_X * (BOARD_Y - 2) + pong_game.bar_x_pos + index] = PONG_BOARD_BAR;

}

// Static function declarations

static void set_level_bricks(uint8_t level) {
	int i = 0;
	int j = 0;
	for (i = 1; i < level+2; i++)
		for (j = 1; j < BOARD_X-1; j++)
			pong_game.board[i * BOARD_X + j] = PONG_BOARD_BRICK;
}

static void clean_board(void) {
	int i = 0;
	int j = 0;
	for (i = 0; i < BOARD_Y; i++)
		for (j = 0; j < BOARD_X; j++)
			pong_game.board[i * BOARD_X + j] = PONG_BOARD_EMPTY;
}

static void set_board_boundaries() {
	uint8_t x, y;
	for (y = 0; y < BOARD_Y; y++)
		for (x = 0; x < BOARD_X; x++) {
			pong_game.board[y * BOARD_X + x] = (x == 0 || x == BOARD_X - 1 || y == 0 ) ? PONG_BOARD_WALL : PONG_BOARD_EMPTY;
			if (y == BOARD_Y - 1) pong_game.board[y * BOARD_X + x] = PONG_BOARD_FLOOR;
		}
}

static uint8_t get_remaining_bricks() {
	int i = 0;
	int j = 0;
	uint8_t count = 0;
	for (i = 0; i < BOARD_Y; i++)
		for (j = 0; j < BOARD_X; j++)
			if(pong_game.board[i * BOARD_X + j] == PONG_BOARD_BRICK) count++;
	return count;
}

static pong_status_t pong_update() {
	if (pong_game.elapsed_ticks % pong_game.tick_speed == 0) {
		uint8_t x_next = pong_game.x_pos + pong_game.x_speed;
		uint8_t y_next = pong_game.y_pos + pong_game.y_speed;

		switch (pong_game.board[y_next * BOARD_X + pong_game.x_pos]) {
		case PONG_BOARD_EMPTY:
			break;
		case PONG_BOARD_FLOOR:
			return PONG_GAME_OVER;
		case PONG_BOARD_BRICK:
			pong_game.board[y_next * BOARD_X + pong_game.x_pos] = PONG_BOARD_EMPTY;
		case PONG_BOARD_WALL:
		case PONG_BOARD_BAR:
			pong_game.y_speed = -pong_game.y_speed;
			break;
		default:
			break;
		}

		switch (pong_game.board[pong_game.y_pos * BOARD_X + x_next]) {
		case PONG_BOARD_EMPTY:
			break;
		case PONG_BOARD_FLOOR:
			return PONG_GAME_OVER;
		case PONG_BOARD_BRICK:
			pong_game.board[pong_game.y_pos * BOARD_X + x_next] = PONG_BOARD_EMPTY;
		case PONG_BOARD_WALL:
		case PONG_BOARD_BAR:
			pong_game.x_speed = -pong_game.x_speed;
			break;
		default:
			break;
		}

		switch (pong_game.board[y_next * BOARD_X + x_next]) {
		case PONG_BOARD_EMPTY:
			break;
		case PONG_BOARD_FLOOR:
			return PONG_GAME_OVER;
		case PONG_BOARD_BRICK:
			pong_game.board[y_next * BOARD_X + x_next] = PONG_BOARD_EMPTY;
		case PONG_BOARD_WALL:
		case PONG_BOARD_BAR:
			break;
		default:
			break;
		}


		pong_game.board[pong_game.y_pos * BOARD_X + pong_game.x_pos] = PONG_BOARD_EMPTY;

		pong_game.x_pos = pong_game.x_pos + pong_game.x_speed;
		pong_game.y_pos = pong_game.y_pos + pong_game.y_speed;

		pong_game.board[pong_game.y_pos * BOARD_X + pong_game.x_pos] = PONG_BOARD_PONG;
		
		if (pong_game.elapsed_ticks % 7&&pong_game.tick_speed>5) pong_game.tick_speed--;
	}
	pong_game.elapsed_ticks++;
	

	if (get_remaining_bricks() == 0)
		return PONG_NEXT_LEVEL;

	return PONG_RUNNING;
}

