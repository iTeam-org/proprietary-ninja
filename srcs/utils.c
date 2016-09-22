
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

void utils_blit_hud(s_game *game)
{
    SDL_Surface *text = NULL;
    char buffer[1024];

    text = TTF_RenderText_Blended(game->font_title, "Proprietary ninja", TEXT_COLOR);
    utils_blit_at(text, game->screen, 200, 20);
    SDL_FreeSurface(text);

    sprintf(buffer, "%d live(s)", game->lives);
    text = TTF_RenderText_Blended(game->font_text, buffer, TEXT_COLOR);
    utils_blit_at(text, game->screen, 20, 60);
    SDL_FreeSurface(text);
}

