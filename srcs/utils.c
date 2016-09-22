
#include <stdlib.h>

#include "utils.h"

int utils_rand_int(int min, int max)
{
    return rand()%(max-min) + min;
}

void utils_blit_at(SDL_Surface *src, SDL_Surface *dst, int x, int y)
{
    SDL_Rect pos;

    pos.x = x;
    pos.y = y;
    pos.w = dst->w;
    pos.h = dst->h;

    SDL_BlitSurface(src, NULL, dst, &pos);
}
