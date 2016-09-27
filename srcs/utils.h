#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <SDL2/SDL.h>
#include "logo.h"

#define SCREEN_WIDTH                800
#define SCREEN_HEIGHT               600
#define FPS                         25
#define TEXT_COLOR                  (SDL_Color){0, 0, 0}

#define SQUARE(x)                   ((x)*(x))
#define DIST_SQUARE(x, y)           (SQUARE(x) + SQUARE(y))


int utils_rand_int(int min, int max);
SDL_Texture *utils_load_texture(SDL_Renderer *game_renderer, char *filepath);
void utils_blit_at(SDL_Texture *src, SDL_Renderer *dst, int x, int y);
void utils_blit_hud(s_game *game);

#endif
