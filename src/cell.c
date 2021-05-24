#include <stdio.h>
#include <stdio.h>

#include "cell.h"

static cell_t *cell_init(size_t width, size_t height)
{
	cell_t *cell_new = malloc(sizeof(*cell_new));
	if (!cell_new) {
		perror("cell_init: Failed to malloc cell_new");
		exit(EXIT_FAILURE);
	}

	cell_new->rect.w = width;
	cell_new->rect.h = height;
	cell_new->alive = 0;

	return cell_new;
}

body_t *body_init(size_t rows, size_t cols)
{
	size_t x, y;

	body_t *body_new = malloc(sizeof(*body_new));
	if (!body_new) {
		perror("body_init: Failed to malloc body_new");
		exit(EXIT_FAILURE);
	}

	body_new->rows = rows;
	body_new->cols = cols;

	body_new->cells = malloc(CELL_COUNT * sizeof(*body_new->cells));
	if (!body_new->cells) {
		perror("body_init: Failed to malloc body_new->cells");
		exit(EXIT_FAILURE);
	}

	for (x = 0; x < body_new->rows; x++)
		for (y = 0; y < body_new->cols; y++)
			body_new->cells[x * body_new->cols + y] = cell_init(CELL_WIDTH, CELL_HEIGHT);

	return body_new;
}

static void cell_destroy(cell_t *cell)
{
	free(cell);
}

void body_destory(body_t *body)
{
	size_t x, y;

	for (x = 0; x < body->rows; x++)
		for (y = 0; y < body->cols; y++)
			cell_destroy(body->cells[x * body->cols + y]);
	free(body->cells);
	free(body);
}

static int draw_cell(SDL_Renderer *renderer, cell_t *cell, int x, int y)
{
	cell->rect.x = 5 * x;
	cell->rect.y = 5 * y;
	cell->rect.w = cell->rect.h = 5;

	if (cell->alive)
		SDL_SetRenderDrawColor(renderer, 204, 204, 255, SDL_ALPHA_OPAQUE);  /* purple-indigo-blue */
	else
		SDL_SetRenderDrawColor(renderer, 255, 240, 240, SDL_ALPHA_OPAQUE);  /* salmon-ish */

	return SDL_RenderFillRect(renderer, &cell->rect);
}

void draw_generation(SDL_Renderer *renderer, body_t *body)
{
	size_t x, y;

	for (x=0; x < body->rows; x++)
		for (y=0; y < body->cols; y++)
			draw_cell(renderer, body->cells[x * body->cols + y], x, y);
}

void inital_generation(body_t *body_new, body_t *body_old, int *pop)
{
	size_t x, y;
	*pop = 0;

	for (x=(size_t)(CELL_COUNT * 0.25); x < (size_t)(CELL_COUNT * 0.75); x++)
		for (y=(size_t)(CELL_COUNT * 0.25); y < (size_t)(CELL_COUNT * 0.75); y++) {
			body_new->cells[x * body_new->cols + y]->alive = rand() & 1;
			*pop += body_new->cells[x * body_new->cols + y]->alive;
		}

	memcpy(body_old->cells, body_new->cells, CELL_COUNT * CELL_COUNT * sizeof(*body_old->cells));
}

void compute_generation(body_t *body_new, body_t *body_old, int *pop)
{
	size_t x, y, a, b;
	int neighbors;
	*pop = 0;

	for (x=0; x < CELL_COUNT; x++) {
		for (y=0; y < CELL_COUNT; y++) {
			neighbors = 0;

			for (a=0; a < 3; a++) {
				for (b=0; b < 3; b++) {
					if (x - 1 + a < 0 || x - 1 + a > CELL_COUNT - 1 ||
						y - 1 + b < 0 || y - 1 + b > CELL_COUNT - 1)
						continue;

					neighbors += body_new->cells[x - 1 + a + CELL_COUNT * (y - 1 + b)]->alive;
				}
			}

			if (!body_new->cells[x + CELL_COUNT * y]->alive) {
				if (neighbors == 3)
					body_old->cells[x + CELL_COUNT * y]->alive = 1;
			}
			else {
				if (neighbors == 3 || neighbors == 4)
					body_old->cells[x + CELL_COUNT * y]->alive = 1;
				else
					body_old->cells[x + CELL_COUNT * y]->alive = 0;
			}
			*pop += body_old->cells[x + CELL_COUNT * y]->alive;
		}
	}

}
