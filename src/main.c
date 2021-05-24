#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL.h>

#include "cell.h"

#define WIDTH 500
#define HEIGHT 500

int main()
{
	int done, population;
	body_t *body, *body_old, *body_temp;
	SDL_Window* window;
	SDL_Renderer *renderer;
	SDL_Event *event;

	srand(time(NULL));

	body = body_init(CELL_COUNT, CELL_COUNT);
	body_old = body_init(CELL_COUNT, CELL_COUNT);
	body_temp = body_init(CELL_COUNT, CELL_COUNT);
	inital_generation(body, body_old, &population);

	/* Initialize window */
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("Conway's Game of Life - Jack McVeigh", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (!window) {
		perror("main: Failed to create window");
		exit(EXIT_FAILURE);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!window) {
		perror("main: Failed to create renderer");
		exit(EXIT_FAILURE);
	}
	SDL_SetRenderDrawColor(renderer, 255, 240, 240, SDL_ALPHA_OPAQUE); /* salmon-ish */

	/* Main loop */
	done = 0;
	while (!done) {
		SDL_PollEvent(event);
		if (event->type == SDL_QUIT)
			done = 1;

		SDL_RenderClear(renderer); /* Clear */
		draw_generation(renderer, body); /* Draw */
		SDL_RenderPresent(renderer); /* Present drawing */

		compute_generation(body, body_old, &population);

		body_temp->cells = body->cells;
		body->cells = body_old->cells;
		body_old->cells = body_temp->cells;
	}

	/* Destory program */
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
