
#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "line.h"
#include "utils.h"
#include "fruit.h"


s_line *line_new(int x1, int y1, int x2, int y2)
{
    s_line *ret = NULL;

    ret = malloc(sizeof(s_line));
    if (ret == NULL)
    {
        fprintf(stderr, "Error: malloc() (line_new())\n");
        exit(1);
    }

    ret->x1 = x1;
    ret->y1 = y1;
    ret->x2 = x2;
    ret->y2 = y2;

    ret->dx = x2 - x1;
    ret->dy = y2 - y1;

    ret->a = 1.0 * ret->dy / ret->dx;
    ret->b = y1 - ret->a*x1;
    ret->norm = sqrt(DIST_SQUARE(ret->dx, ret->dy));

    ret->timestamp = SDL_GetTicks();

    return ret;
}

void line_append(s_game *game, s_line *new)
{
    int i = 0;

    for (i = 0; i < LINES_COUNT; ++i)
    {
        if (game->lines[i] == NULL)
        {
            game->lines[i] = new;
            return;
        }
    }

    fprintf(stderr, "Error: no free slot (line_append())\n");
    exit(1);
}

void line_blit_all(s_game *game)
{
    int i = 0;
    Uint32 now = 0;

    now = SDL_GetTicks();

    for (i = 0; i < LINES_COUNT; ++i)
    {
        if (game->lines[i] != NULL)
        {
            if (game->lines[i]->timestamp + LINE_SHOW_TIME < now)
            {
                free(game->lines[i]);
                game->lines[i] = NULL;
            }
            else
            {
                SDL_SetRenderDrawColor(game->renderer, 200, 50, 50, SDL_ALPHA_OPAQUE);

                SDL_RenderDrawLine(
                    game->renderer,
                    game->lines[i]->x1, game->lines[i]->y1, game->lines[i]->x2, game->lines[i]->y2
                );
            }
        }
    }
}
