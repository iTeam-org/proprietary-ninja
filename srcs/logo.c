
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "utils.h"
#include "logo.h"

/*
    logo model
*/

s_logo_model *logo_model_new(SDL_Renderer *game_renderer, int is_open_source, char *name)
{
    s_logo_model *ret = NULL;
    char filename[1024];

    ret = malloc(sizeof(s_logo_model));
    if (ret == NULL)
    {
        fprintf(stderr, "Error: malloc() (logo_model_new())\n");
        exit(1);
    }

    ret->name = name;
    sprintf(filename, "res/%s.png", name);
    ret->img = utils_load_texture(game_renderer, filename);
    ret->is_open_source = is_open_source;
    ret->radius = LOGO_RADIUS;

    return ret;
}

void logo_model_append(s_game *game, s_logo_model *new)
{
    int i = 0;

    for (i = 0; i < LOGO_MODELS_COUNT; ++i)
    {
        if (game->models[i] == NULL)
        {
            game->models[i] = new;
            game->loaded_models ++;
            return;
        }
    }

    fprintf(stderr, "Error: no free slot (logo_model_append())\n");
    exit(1);
}

/*
    logo
*/

s_logo *logo_new(s_logo_model *model, s_vector window_size)
{
    s_logo *ret = NULL;
    int min_sy = 0, max_sy = 0;

    ret = malloc(sizeof(s_logo));
    if (ret == NULL)
    {
        fprintf(stderr, "Error: malloc() (logo_new())\n");
        exit(1);
    }

    ret->x = utils_rand_int(0, window_size.x);
    ret->y = window_size.y-0;
    ret->sx = utils_rand_int(-20, 20);

    min_sy = -window_size.y/40;
    max_sy = -15;
    if (max_sy < min_sy)
    {
        printf("shit. %s %s %d\n", __FUNCTION__, __FILE__, __LINE__);
        exit(1);
    }
    ret->sy = utils_rand_int(min_sy, max_sy);
    ret->ax = 0;
    ret->ay = 0.5;
    ret->is_captured = 0;
    ret->model = model;

    return ret;
}


void logo_append(s_logo **logos, s_logo *new)
{
    int i = 0;

    for (i = 0; i < LOGOS_COUNT; ++i)
    {
        if (logos[i] == NULL)
        {
            logos[i] = new;
            return;
        }
    }

    fprintf(stderr, "Error: no free slot (logo_append())\n");
    exit(1);
}


void logo_update_all(s_game *game)
{
    int i = 0;
    s_logo **logos = NULL;

    logos = game->logos;

    for (i = 0; i < LOGOS_COUNT; ++i)
    {
        if (logos[i] != NULL)
        {
            int w = 0, h = 0, radius = 0;

            SDL_QueryTexture(logos[i]->model->img, NULL, NULL, &w, &h);
            radius = logos[i]->model->radius;

            // apply physics (basic)

            logos[i]->sx += logos[i]->ax;
            logos[i]->sy += logos[i]->ay;

            logos[i]->x += logos[i]->sx;
            logos[i]->y += logos[i]->sy;

            // apply physics (collisons)

            // make it bounce
            if (logos[i]->x - radius < 0)
            {
                logos[i]->x = - (logos[i]->x - radius) + radius;
                logos[i]->sx = -BOUNCE_COEFF * logos[i]->sx;
            }
            if (logos[i]->x + radius >= game->window_size.x)
            {
                logos[i]->x = game->window_size.x - radius - (logos[i]->x + radius - game->window_size.x);
                logos[i]->sx = -BOUNCE_COEFF * logos[i]->sx;
            }

            // if under the floor, remove it from the list of active logos
            if (logos[i]->y - radius >= game->window_size.y)
            {
                if (! logos[i]->is_captured)
                {
                    if (logos[i]->model->is_open_source)
                        game->lives --;
                }

                free(logos[i]);
                logos[i] = NULL;
            }
        }
    }
}

void logo_blit_all(s_game *game)
{
    int i = 0;

    for (i = 0; i < LOGOS_COUNT; ++i)
    {
        if (game->logos[i] != NULL)
        {
            if (game->logos[i]->is_captured && !game->logos[i]->model->is_open_source)
            {
                SDL_SetTextureColorMod(game->logos[i]->model->img, 64, 64, 64);

                utils_blit_at(
                    game->logos[i]->model->img, game->renderer,
                    game->logos[i]->x - game->logos[i]->model->radius, game->logos[i]->y - game->logos[i]->model->radius
                );

                SDL_SetTextureColorMod(game->logos[i]->model->img, 255, 255, 255);
            }
            else
            {
                float size = 1;  // default zoom is 1 - no modification

                if (game->logos[i]->is_captured)
                    size = 1 - (1.0 * (SDL_GetTicks() - game->logos[i]->captured_timestamp) / 500);

                if ((0 <= size) && (size <= 1))
                {

                    utils_blit_atzoom(
                        game->logos[i]->model->img, game->renderer,
                        game->logos[i]->x - game->logos[i]->model->radius, game->logos[i]->y - game->logos[i]->model->radius,
                        size
                    );
                }
            }
        }
    }
}
