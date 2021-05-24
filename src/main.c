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
	cell_t **temp;
	body_t *body, *body_old;
	SDL_Window* window;
	SDL_Renderer *renderer;
	SDL_Event event;

	srand(time(0));

	/* Initialize bodies */
	temp = malloc(sizeof(*temp));
	body = body_init(CELL_COUNT, CELL_COUNT);
	body_old = body_init(CELL_COUNT, CELL_COUNT);
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

	printf("Starting main loop\n");
	/* Main loop */
	done = 0;
	while (!done) {
		printf("Current Population: %d\n", population);

		/* Render */
		SDL_RenderClear(renderer); /* Clear */
		draw_generation(renderer, body); /* Draw */
		SDL_RenderPresent(renderer); /* Present drawing */

		/* Compute next generation */
		compute_generation(body, body_old, &population);

		/* Ping-pong buffer */
		temp = body->cells;
		body->cells = body_old->cells;
		body_old->cells = temp;

		SDL_Delay(1000);

		/* Poll for events */
		while (SDL_PollEvent(&event))
			switch (event.type) {
				case SDL_QUIT:
					printf("Quiting\n");
					done = 1;
					break;
			}
	}

	body_destory(body);
	body_destory(body_old);

	/* Destory program */
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
