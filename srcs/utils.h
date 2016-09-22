#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "fruit.h"

#define SCREEN_WIDTH            800
#define SCREEN_HEIGHT           600
#define FPS                     20
#define SCREEN_BG_COLOR(screen) SDL_MapRGB(screen->format, 128, 128, 128)

int rand_int(int min, int max);

#endif
