#include "Snake.h"

#define BOARD_X 10
#define BOARD_Y 10
#define SNAKE_INIT_LENGTH 3


// Enums 

typedef enum {
	SNAKE_UP,
	SNAKE_RIGHT,
	SNAKE_DOWN,
	SNAKE_LEFT
}snake_direction_t;

typedef enum {
	NO_COLLISION,
	COLLISION,
	APPLE_COLLISION
}snake_collision_t;

typedef enum {
	SNAKE_RUNNING,
	SNAKE_GAME_OVER
}snake_status_t;


// Static function headers

static void new_apple();
static void clean_board(void);
static void set_board_boundaries();
static void snake_move();
static snake_collision_t check_next_movement();

// Structs 

typedef struct {
	uint8_t* board;
	uint8_t direction;
	uint16_t score;
	
	uint16_t elapsed_ticks;
	uint8_t tick_speed;

	uint8_t x_pos;
	uint8_t y_pos;

	snake_status_t game_status;

	bool direction_changed;
}snake_game_t;

// Static variable declarations

static snake_game_t snake_game;
static uint8_t board[BOARD_X*BOARD_Y];

// External function declarations

void snake_update() {
	switch (snake_game.game_status) {
	case SNAKE_RUNNING:
		snake_game.elapsed_ticks++;

		if (snake_game.elapsed_ticks % snake_game.tick_speed == 0)
		{
			switch (check_next_movement()) {
			case COLLISION:
				snake_game.game_status = SNAKE_GAME_OVER;
				break;
			case NO_COLLISION:
				snake_move();
				break;
			case APPLE_COLLISION:
				snake_game.elapsed_ticks = 0;
				snake_game.score++;
				if (!(snake_game.score % 2) && snake_game.tick_speed > 5) snake_game.tick_speed--;
				new_apple();
				snake_move();
				break;
			default:
				break;
			}
		}
		break;
	case SNAKE_GAME_OVER:
		snake_begin_game();
		break;
	}
}

void snake_begin_game() {
	uint8_t index;
	snake_game.board = board;
	snake_game.direction = SNAKE_RIGHT;
	snake_game.game_status = SNAKE_RUNNING;
	snake_game.score = 0;
	snake_game.x_pos = BOARD_X / 2;
	snake_game.y_pos = BOARD_Y / 2;
	snake_game.elapsed_ticks = 0;
	snake_game.tick_speed = 20;
	snake_game.direction_changed = false;
	clean_board();
	set_board_boundaries();
	for (index=0;index<SNAKE_INIT_LENGTH;index++)
		snake_game.board[(snake_game.y_pos) * BOARD_X + snake_game.x_pos-index] = index;
	new_apple();
}


void snake_move_cw() {
	if (!snake_game.direction_changed) {
		if (snake_game.direction == SNAKE_LEFT)
			snake_game.direction = SNAKE_UP;
		else
			snake_game.direction++;
		snake_game.direction_changed = true;
	}
}

void snake_move_ccw() {
	if (!snake_game.direction_changed) {
		if (snake_game.direction == SNAKE_UP)
			snake_game.direction = SNAKE_LEFT;
		else
			snake_game.direction--;
		snake_game.direction_changed = true;
	}
}

uint8_t * snake_get_board() {
	return snake_game.board;
}

uint8_t snake_get_score() {
	return snake_game.score;
}



// Static function declarations

static void new_apple() {
	uint8_t suggested_y;
	uint8_t suggested_x;
	while (1) {
		suggested_y = rand() % BOARD_Y;
		suggested_x = rand() % BOARD_X;
		if (snake_game.board[suggested_y * BOARD_X + suggested_x] == SNAKE_EMPTY)
		{
			snake_game.board[suggested_y * BOARD_X + suggested_x] = SNAKE_APPLE;
			break;
		}
	}
}

static void clean_board(void) {
	int i = 0;
	int j = 0;
	for (i = 0; i < BOARD_Y; i++)
		for (j = 0; j < BOARD_X; j++)
			snake_game.board[i * BOARD_X + j] = SNAKE_EMPTY;
}

static void set_board_boundaries() {
	uint8_t x, y;
	for (y = 0; y < BOARD_Y; y++)
		for (x = 0; x < BOARD_X; x++)
			snake_game.board[y * BOARD_X + x] = (x == 0 || x == BOARD_X - 1 || y==0 || y == BOARD_Y - 1) ? SNAKE_WALL : SNAKE_EMPTY;
}

static void snake_move() {
	uint8_t index;
	
	for (index = 0; index < BOARD_X * BOARD_Y; index++)
	{
		if (snake_game.board[index] != SNAKE_WALL && snake_game.board[index] != SNAKE_APPLE)
		{
			snake_game.board[index]++;
			if (snake_game.board[index] > (snake_game.score + SNAKE_INIT_LENGTH-1))
				snake_game.board[index] = SNAKE_EMPTY;
		}
	}
		
	switch (snake_game.direction) {
	case SNAKE_UP:
		snake_game.y_pos--;
		break;
	case SNAKE_DOWN:
		snake_game.y_pos++;
		break;
	case SNAKE_LEFT:
		snake_game.x_pos--;
		break;
	case SNAKE_RIGHT:
		snake_game.x_pos++;
		break;
	default:
		break;
	}

	snake_game.board[(snake_game.y_pos) * BOARD_X + snake_game.x_pos] = 0;

	snake_game.direction_changed = false;
}

static snake_collision_t check_next_movement() {
	uint8_t next_token=128;

	switch (snake_game.direction) {
	case SNAKE_UP:
		next_token = snake_game.board[(snake_game.y_pos - 1) * BOARD_X + snake_game.x_pos];
		break;
	case SNAKE_DOWN:
		next_token = snake_game.board[(snake_game.y_pos + 1) * BOARD_X + snake_game.x_pos];
		break;
	case SNAKE_LEFT:
		next_token = snake_game.board[(snake_game.y_pos) * BOARD_X + snake_game.x_pos - 1];
		break;
	case SNAKE_RIGHT:
		next_token = snake_game.board[(snake_game.y_pos) * BOARD_X + snake_game.x_pos + 1];
		break;
	default:
		break;
	}

	if (next_token == 255 || next_token < 128)
		return COLLISION;
	else if (next_token == 254)
		return APPLE_COLLISION;
	else
		return NO_COLLISION;
}

