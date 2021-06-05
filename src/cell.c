#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "cell.h"
#include "utilities.h"

/*
 * Function:	cell_init
 * ----------------------
 * Initialize a unique cell.
 *
 * width: width of the cell in pixels.
 * height: height of the cell in pixels.
 *
 * returns: pointer to the newly allocated cell.
 */
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

/*
 * Function:	body_init
 * ----------------------
 * Initialize a body of cells.
 *
 * rows: number of rows in the body.
 * cols: number of cols in the body.
 *
 * returns: pointer to the newly allocated body.
 */
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

	body_new->cells = malloc(body_new->rows * body_new->cols * sizeof(*body_new->cells));
	if (!body_new->cells) {
		perror("body_init: Failed to malloc body_new->cells");
		exit(EXIT_FAILURE);
	}

	for (x = 0; x < body_new->cols; x++)
		for (y = 0; y < body_new->rows; y++)
			body_new->cells[x * body_new->cols + y] = cell_init(cell_meta.width, cell_meta.height);

	return body_new;
}

/*
 * Function:	cell_destroy
 * -------------------------
 * Destroy a cell.
 *
 * cell: pointer to the cell allocated in memory.
 */
static void cell_destroy(cell_t *cell)
{
	free(cell);
}

/*
 * Function:	body_destroy
 * -------------------------
 * Destroy a body.
 *
 * body: pointer to the body allocated in memory.
 */
void body_destory(body_t *body)
{
	size_t x, y;

	for (x = 0; x < body->cols; x++)
		for (y = 0; y < body->rows; y++)
			cell_destroy(body->cells[x * body->cols + y]);
	free(body->cells);
	free(body);
}

/*
 * Function:	draw_cell
 * ----------------------
 * Draw a cell square.
 *
 * renderer: SDL_Renderer struct used for rendering the cell square.
 * cell: cell stuct stores the location, width, and hight of the cell square.
 * x: the column of the cell.
 * y: the row of the cell.
 */
static int draw_cell(SDL_Renderer *renderer, cell_t *cell, int x, int y)
{
	cell->rect.x = cell_meta.width * x;
	cell->rect.y = cell_meta.height * y;
	cell->rect.w = cell_meta.width;
	cell->rect.h = cell_meta.height;

	if (cell->alive)
		SDL_SetRenderDrawColor(renderer, cell_meta.color_r, cell_meta.color_g, cell_meta.color_b, SDL_ALPHA_OPAQUE);
	else
		SDL_SetRenderDrawColor(renderer, bg_meta.color_r, bg_meta.color_g, bg_meta.color_b, SDL_ALPHA_OPAQUE);

	return SDL_RenderFillRect(renderer, &cell->rect);
}

/*
 * Function:	draw_generation
 * ----------------------------
 * Draw the current generation's cells.
 *
 * renderer: SDL_Renderer struct used for rendering the cell square.
 * body: the body containing the current generation of cells.
 */
void draw_generation(SDL_Renderer *renderer, body_t *body)
{
	size_t x, y;

	for (x=0; x < body->cols; x++)
		for (y=0; y < body->rows; y++)
			draw_cell(renderer, body->cells[x * body->cols + y], x, y);
}

/*
 * Function:	random_mode
 * ------------------------
 * Default mode of cell generation, randomly assigns cells as alive in the center
 * 	1/4th of the body using a probability of being alive.
 *
 * body_new: pointer to the body that will store the updated cells.
 * body_old: pointer to the body that stores the previous generation of cells.
 * pop: pointer to the population count used for tracking the body's progress.
 */
static void random_mode(body_t *body_new, body_t *body_old, int *pop)
{
	size_t x, y;

	for (x=(size_t)(body_new->cols * 0.25); x < (size_t)(body_new->cols * 0.75); x++) {
		for (y=(size_t)(body_new->rows * 0.25); y < (size_t)(body_new->rows * 0.75); y++) {
			body_new->cells[x * body_new->cols + y]->alive = ((rand() % 100 + 1) <= cell_meta.alive_prob);
			*pop += body_new->cells[x * body_new->cols + y]->alive;
		}
	}
}

/*
 * Function:	pattern_mode
 * ------------------------
 * A mode of cell generation, given a pattern selected by the user, generate the cells.
 *
 * body_new: pointer to the body that will store the updated cells.
 * body_old: pointer to the body that stores the previous generation of cells.
 * pop: pointer to the population count used for tracking the body's progress.
 */
static void pattern_mode(body_t *body_new, body_t *body_old, int *pop)
{
	FILE *pattern_fd;
	size_t len, x, y;
	char *pattern, *point;

	pattern = parse_pattern_choice();

	pattern_fd = fopen(pattern, "r");
	if (!pattern_fd) {
		perror("pattern_mode: Error opening pattern file");
		exit(EXIT_FAILURE);
	}
	free(pattern);

	point = NULL;
	getline(&point, &len, pattern_fd); /* Skip header */
	while (getline(&point, &len, pattern_fd) != -1) {
		point[strlen(point) - 1] = '\0';
		x = (size_t)atoi(strtok(point, ",")) + (body_new->cols * 0.5);
		y = (size_t)atoi(strtok(NULL, ",")) + (body_new->rows * 0.5);
		body_new->cells[x * body_new->cols + y]->alive = 1;
		*pop += 1;
	}

	fclose(pattern_fd);
	free(point);
}

/*
 * Function:	drawing_mode
 * ------------------------
 * A mode of cell generation, user selects the cells to be alive and then starts simulation.
 *
 * body_new: pointer to the body that will store the updated cells.
 * body_old: pointer to the body that stores the previous generation of cells.
 * pop: pointer to the population count used for tracking the body's progress.
 */
static void drawing_mode(body_t *body_new, body_t *body_old, int *pop)
{
	return;
}

/*
 * Function:	initial_generation
 * -------------------------------
 * The api for the selected mode to populate the initial body.
 *
 * body_new: pointer to the body that will store the updated cells.
 * body_old: pointer to the body that stores the previous generation of cells.
 * pop: pointer to the population count used for tracking the body's progress.
 */
void inital_generation(body_t *body_new, body_t *body_old, int *pop)
{
	*pop = 0;

	switch (mode) {
		case 'r':
			random_mode(body_new, body_old, pop);
			break;
		case 'p':
			pattern_mode(body_new, body_old, pop);
			break;
		case 'd':
			fprintf(stderr, "initial_generation: mode %c is not implemented yet.\n", mode);
	}

}

/*
 * Function:	compute_generation
 * -------------------------------
 * Computes the next generation given the previous generation and the rules
 * 	defining the game of life.
 *
 * body_new: pointer to the body that will store the updated cells.
 * body_old: pointer to the body that stores the previous generation of cells.
 * pop: pointer to the population count used for tracking the body's progress.
 */
void compute_generation(body_t *body_new, body_t *body_old, int *pop)
{
	int neighbors, x, y, a, b, i;
	*pop = 0;

	for (x=0; x < body_old->cols; x++) {
		for (y=0; y < body_old->rows; y++) {
			i = x * body_old->cols + y;
			neighbors = body_old->cells[i]->alive ? -1 : 0;

			for (a=-1; a < 2; a++) {
				for (b=-1; b < 2; b++) {
					if (!(x + a < 0 || x + a > body_old->cols - 1 ||
					    y + b < 0 || y + b > body_old->rows - 1))
						neighbors += body_old->cells[(x + a) * body_old->cols + (y + b)]->alive;
				}
			}

			if (body_old->cells[i]->alive && ((neighbors < 2) || (neighbors > 3)))
				body_new->cells[i]->alive = 0;
			else if (!body_old->cells[i]->alive && (neighbors == 3))
				body_new->cells[i]->alive = 1;
			else
				body_new->cells[i]->alive = body_old->cells[i]->alive;

			*pop += body_new->cells[i]->alive;
		}
	}

}
