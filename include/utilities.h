#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define DELAY_DEFAULT 1000

void print_usage(void);
int parse_input(int argc, char *argv[]);
void display_text(SDL_Renderer *renderer, char *text, SDL_Color color, int x, int y, int w, int h);

#endif /* _UTILITIES_H_ */
