#ifndef _CELL_H_
#define _CELL_H_

#include <stdint.h>
#include <SDL.h>

#define CELL_ROWS_DEFAULT 100
#define CELL_COLS_DEFAULT 100
#define CELL_WIDTH_DEFAULT 8
#define CELL_HEIGHT_DEFAULT 8
#define CELL_GRID_DEFAULT 0

#define CELL_COLOR_R_DEFAULT 204
#define CELL_COLOR_G_DEFAULT 204
#define CELL_COLOR_B_DEFAULT 255

#define CELL_SPAWN_PROBABILITY_DEFAULT 25

struct cell_meta_data {
	int rows;
	int cols;
	int width;
	int height;
	int grid_on;
	int color_r;
	int color_g;
	int color_b;
	int alive_prob;
};
extern struct cell_meta_data cell_meta;

typedef struct cell_s cell_t;
struct cell_s {
	SDL_Rect rect;
	uint8_t alive;
};

typedef struct body_s body_t;
struct body_s {
	size_t rows;
	size_t cols;
	cell_t **cells;
};

static cell_t *cell_init(size_t width, size_t height);
body_t *body_init(size_t rows, size_t cols);
static void cell_destroy(cell_t *cell);
void body_destory(body_t *body);

static void draw_cell(SDL_Renderer *renderer, cell_t *cell, int x, int y);
void draw_generation(SDL_Renderer *renderer, body_t *body);
static body_t *random_mode(body_t *body_new, body_t *body_old, int *pop);
static body_t *pattern_mode(body_t *body_new, body_t *body_old, int *pop);
static body_t *drawing_mode(SDL_Renderer *renderer, body_t *body_new, body_t *body_old, int *pop);
body_t *inital_generation(SDL_Renderer *renderer, body_t *body_new, body_t *body_old, int *pop);
void compute_generation(body_t *body, body_t *body_old, int *pop);

#endif /* _CELL_H_ */
