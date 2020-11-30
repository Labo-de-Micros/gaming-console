#include "Tetris.h"
#include <stdlib.h>
// Internal enums

typedef enum {
	OUT_OF_BOUNDS,
	COLLISION,
	NO_COLLISION
}tetris_collision_t;


typedef enum {
	NORMAL,
	CW,
	INVERTED,
	CCW,
}tetris_rotation_t;

// Internal structs

typedef struct {
	uint8_t* tetris_board;
	uint8_t* piece_board;

	uint8_t current_piece;
	uint8_t x_pos;
	uint8_t y_pos;
	uint8_t rotation;

	bool lines_to_delete;

	uint16_t score;

	uint16_t elapsed_ticks;
	uint8_t piece_tick_speed;

	tetris_status_t game_status;

}tetris_game_t;

// Static function headers

static void reset_game(void);

static void clean_board(void);
static void clean_piece_board(void);
static void set_board_borders(void);

static tetris_collision_t check_collision(int8_t piece_start_x, int8_t piece_start_y, uint8_t r);
static uint8_t get_rotated_piece_coord(uint8_t coord_x, uint8_t coord_y, uint8_t rotation);

static void sort_new_piece(void);
static void lock_piece_down(void);
static void write_piece_to_board();
static void write_piece_to_piece_board();

static void check_for_lines(void);
static void delete_lines(void);
static void update_score(uint8_t lines_completed);


// Static variable declarations

static tetris_game_t tetris_game;

static const char* pieces[] = { ".X...X...X...X..",		// 1
								"..X..XX...X.....",		// 2 
								".....XX..XX.....",		// 3
								"..X..XX..X......",		// 4 
								".X...XX...X.....",		// 5
								".X...X...XX.....",		// 6
								"..X...X..XX....." };	// 7

// Public function definitions

void tetris_init() {
	static uint8_t board[BOARD_X * BOARD_Y];
	static uint8_t pboard[BOARD_X * BOARD_Y];
	tetris_game.tetris_board = board;
	tetris_game.piece_board = pboard;
	tetris_game.current_piece = 0;
	tetris_game.x_pos = 0;
	tetris_game.y_pos = 0;
	tetris_game.rotation = NORMAL;
	tetris_game.game_status = TETRIS_IDLE_ST;
	tetris_game.lines_to_delete = false;
	tetris_game.elapsed_ticks = 0;
	tetris_game.piece_tick_speed = 20;
	clean_board();
	clean_piece_board();
	set_board_borders();
}




uint8_t* tetris_get_board() {
	return tetris_game.tetris_board;
};

uint8_t* tetris_get_piece_board() {
	return tetris_game.piece_board;
};

void tetris_move(tetris_direction_t dir) {
	if (check_collision(tetris_game.x_pos, tetris_game.y_pos + 1, tetris_game.rotation) == COLLISION) tetris_game.elapsed_ticks = 0;
	switch (dir) {
	case TETRIS_LEFT:
		if (check_collision(tetris_game.x_pos - 1, tetris_game.y_pos, tetris_game.rotation) == NO_COLLISION)
			tetris_game.x_pos--;
		break;
	case TETRIS_RIGHT:
		if (check_collision(tetris_game.x_pos + 1, tetris_game.y_pos, tetris_game.rotation) == NO_COLLISION)
			tetris_game.x_pos++;
		break;
	case TETRIS_DOWN:
		if (check_collision(tetris_game.x_pos, tetris_game.y_pos + 1, tetris_game.rotation) == NO_COLLISION)
			tetris_game.y_pos++;
		else
			lock_piece_down();
		break;
	case TETRIS_DROP:
		while (check_collision(tetris_game.x_pos, tetris_game.y_pos + 1, tetris_game.rotation) == NO_COLLISION)
			tetris_game.y_pos++;
		lock_piece_down();
		break;
	default:
		break;
	}
}

void tetris_rotate() {
	if (check_collision(tetris_game.x_pos, tetris_game.y_pos + 1, tetris_game.rotation) == COLLISION) tetris_game.elapsed_ticks = 0;
	if (check_collision(tetris_game.x_pos, tetris_game.y_pos, tetris_game.rotation + 1) == NO_COLLISION)
		tetris_game.rotation++;
}

void tetris_update_board(void)
{
	switch (tetris_game.game_status) {
	case TETRIS_RUNNING_ST:

		if (tetris_game.lines_to_delete == true) delete_lines();

		tetris_game.elapsed_ticks++;

		if (tetris_game.elapsed_ticks % tetris_game.piece_tick_speed == 0)
			tetris_move(TETRIS_DOWN);

		write_piece_to_piece_board();

		break;
	case TETRIS_GAME_OVER_ST:
		tetris_game.game_status = TETRIS_RUNNING_ST;
		reset_game();
		tetris_begin_game();
		break;
	default:
		break;
	}
	
}

tetris_status_t tetris_get_status() {
	return tetris_game.game_status;
}
uint16_t tetris_get_score() {
	return tetris_game.score;
}

void tetris_begin_game(void) {
	tetris_game.game_status = TETRIS_RUNNING_ST;
	tetris_game.score = 0;
	sort_new_piece();
}


// Static function definitions

static void reset_game(void) {
	tetris_game.rotation = NORMAL;
	tetris_game.game_status = TETRIS_IDLE_ST;
	tetris_game.lines_to_delete = false;
	tetris_game.elapsed_ticks = 0;
	tetris_game.piece_tick_speed = 20;
	clean_board();
	clean_piece_board();
	set_board_borders();
}

static void clean_board(void) {
	int i = 0;
	int j = 0;
	for (i = 0; i < BOARD_Y; i++)
		for (j = 0; j < BOARD_X; j++)
			tetris_game.tetris_board[i * BOARD_X + j] = EMPTY;
}

static void clean_piece_board(void) {
	int i = 0;
	int j = 0;
	for (i = 0; i < BOARD_Y; i++)
		for (j = 0; j < BOARD_X; j++)
			tetris_game.piece_board[i * BOARD_X + j] = EMPTY;
}

static void set_board_borders(void) {
	uint8_t x, y;
	for (y = 0; y < BOARD_X; y++)
		for (x = 0; x < BOARD_Y; x++)
			tetris_game.tetris_board[y * BOARD_X + x] = (x == 0 || x == BOARD_X - 1 || y == BOARD_Y - 1) ? BORDER : EMPTY;
}


static tetris_collision_t check_collision(int8_t piece_start_x, int8_t piece_start_y, uint8_t r) {
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			int pi = get_rotated_piece_coord(px, py, r);
			int fi = (piece_start_y + py) * BOARD_X + (piece_start_x + px);

			if (1)//(piece_start_x + px >= 0 && piece_start_x + px < BOARD_X)
			{
				if (1)//(piece_start_y + py >= 0 && piece_start_y + py < BOARD_Y)
				{
					if ((pieces[tetris_game.current_piece][pi] != '.') && ((tetris_game.tetris_board[fi] != EMPTY)))
						return COLLISION;
				}
			}
		}
	return NO_COLLISION;
}

static uint8_t get_rotated_piece_coord(uint8_t coord_x, uint8_t coord_y, uint8_t rotation) {
	switch (rotation % 4)
	{
	case NORMAL: return coord_y * 4 + coord_x;
	case CW: return 12 + coord_y - (coord_x * 4);
	case INVERTED: return 15 - (coord_y * 4) - coord_x;
	case CCW: return 3 - coord_y + (coord_x * 4);
	}
}


static void sort_new_piece(void) {
	tetris_game.current_piece = rand() % 7;
	
	tetris_game.rotation = NORMAL;
	if (tetris_game.current_piece!=1) tetris_game.x_pos = BOARD_X / 2-2; //Begin in the middle of the board
	else tetris_game.x_pos = BOARD_X / 2 - 2;
	tetris_game.y_pos = 0; //Begin at the top
	
	if (check_collision(tetris_game.x_pos, tetris_game.y_pos, tetris_game.rotation) == NO_COLLISION)
		tetris_game.game_status = TETRIS_RUNNING_ST;
	else
		tetris_game.game_status = TETRIS_GAME_OVER_ST;
}

static void lock_piece_down(void) {
	write_piece_to_board();
	check_for_lines();
	sort_new_piece();
	return;
}

static void write_piece_to_board() {
	uint8_t px, py;
	for (py = 0; py < 4; py++)
		for (px = 0; px < 4; px++)
			if (pieces[tetris_game.current_piece][get_rotated_piece_coord(px, py, tetris_game.rotation)] != L'.')
				tetris_game.tetris_board[(tetris_game.y_pos + py) * BOARD_X + (tetris_game.x_pos + px)] = tetris_game.current_piece + 1;
}

static void write_piece_to_piece_board() {
	clean_piece_board();

	uint8_t px, py;
	for (py = 0; py < 4; py++)
		for (px = 0; px < 4; px++)
			if (pieces[tetris_game.current_piece][get_rotated_piece_coord(px, py, tetris_game.rotation)] != L'.')
				tetris_game.piece_board[(tetris_game.y_pos + py) * BOARD_X + (tetris_game.x_pos + px)] = tetris_game.current_piece + 1;
}


static void check_for_lines(void) {
	uint8_t px, py, lines_completed;

	lines_completed = 0;
	for (py = 0; py < 4; py++)
		if (tetris_game.y_pos + py < BOARD_X - 1)
		{
			bool bLine = true;
			for (px = 1; px < BOARD_X - 1; px++)
				bLine &= (tetris_game.tetris_board[(tetris_game.y_pos + py) * BOARD_X + px]) != EMPTY;

			if (bLine)
			{
				for (px = 1; px < BOARD_X - 1; px++)
					tetris_game.tetris_board[(tetris_game.y_pos + py) * BOARD_X + px] = LINE;
				lines_completed++;
				tetris_game.lines_to_delete = true;
			}
		}
	update_score(lines_completed);
}

static void delete_lines(void) {
	uint8_t py;
	for (py = 0; py < BOARD_Y; py++) {
		if (tetris_game.tetris_board[py * BOARD_X + 1] == LINE) {

			//tetris_game.board_back[py * tetris_game.board_w + 1] = BORDER;
			uint8_t x, y;
			for (x = 1; x < BOARD_X - 1; x++) {
				for (y = py; y > 0; y--) {
					tetris_game.tetris_board[y * BOARD_X + x] = tetris_game.tetris_board[(y - 1) * BOARD_X + x];
				}
			}
		}
	}
	
	tetris_game.lines_to_delete = false;
}

static void update_score(uint8_t lines_completed) {
	tetris_game.score += lines_completed;
	if (!(tetris_game.score % 4) && tetris_game.piece_tick_speed > 5) tetris_game.piece_tick_speed--;
	return;
}

