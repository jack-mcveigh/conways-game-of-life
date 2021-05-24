#ifndef _CELL_H_
#define _CELL_H_

#include <stdint.h>
#include <SDL.h>

#define CELL_COUNT 100
#define CELL_WIDTH 5
#define CELL_HEIGHT 5
#define CELL_SPAWN_PROBABILITY 5 /* 1/n */

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

static int draw_cell(SDL_Renderer *renderer, cell_t *cell, int x, int y);
void draw_generation(SDL_Renderer *renderer, body_t *body);
void inital_generation(body_t *body_new, body_t *body_old, int *pop);
void compute_generation(body_t *body, body_t *body_old, int *pop);

#endif /* _CELL_H_ */
