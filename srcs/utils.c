
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "utils.h"

int utils_rand_int(int min, int max)
{
    return rand()%(max-min) + min;
}

SDL_Texture *utils_load_texture(SDL_Renderer *game_renderer, char *filepath)
{
    SDL_Surface *tmp = NULL;
    SDL_Texture *ret = NULL;

    tmp = IMG_Load(filepath);
    if (tmp == NULL)
    {
        fprintf(stderr, "Error IMG_Load() (utils_load_texture()): %s\n", IMG_GetError());
        exit(1);
    }

    ret = SDL_CreateTextureFromSurface(game_renderer, tmp);
    if (ret == NULL)
    {
        fprintf(stderr, "Error SDL_CreateTextureFromSurface() (utils_load_texture()): %s\n", IMG_GetError());
        exit(1);
    }

    SDL_FreeSurface(tmp);

    return ret;
}

void utils_blit_at(SDL_Texture *src, SDL_Renderer *dst, int x, int y)
{
    SDL_Rect pos;

    pos.x = x;
    pos.y = y;
    SDL_QueryTexture(src, NULL, NULL, &pos.w, &pos.h);

    SDL_RenderCopy(dst, src, NULL, &pos);
}

void utils_blit_hud(s_game *game)
{
    SDL_Surface *text = NULL;
    char buffer[1024];

    text = TTF_RenderText_Blended(game->font_title, "Proprietary ninja", TEXT_COLOR);
    // utils_blit_at(text, game->renderer, 200, 20, text->w, text->h);
    // todo
    SDL_FreeSurface(text);

    sprintf(buffer, "%d live(s)", game->lives);
    text = TTF_RenderText_Blended(game->font_text, buffer, TEXT_COLOR);
    // utils_blit_at(text, game->renderer, 20, 60 text->w, text->h);
    // todo
    SDL_FreeSurface(text);
}
