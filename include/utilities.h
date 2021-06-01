#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <SDL.h>

#define DELAY_DEFAULT 1000
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define BACKGROUND_COLOR_R_DEFAULT 255
#define BACKGROUND_COLOR_G_DEFAULT 240
#define BACKGROUND_COLOR_B_DEFAULT 240

extern char mode;

struct background_meta_data {
	int width;
	int height;
	int color_r;
	int color_g;
	int color_b;
};
extern struct background_meta_data bg_meta;

static void print_usage(void);
static void print_patterns(void);
char *parse_pattern_choice(void);
int parse_input(int argc, char *argv[]);
void display_text(SDL_Renderer *renderer, char *text, SDL_Color color, int x, int y, int w, int h);

#endif /* _UTILITIES_H_ */
