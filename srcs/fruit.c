
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "utils.h"
#include "fruit.h"

/*
    fruit model
*/

s_fruit_model *fruit_model_new(SDL_Renderer *game_renderer, int is_fruit, char *name)
{
    s_fruit_model *ret = NULL;
    char filename[1024];

    ret = malloc(sizeof(s_fruit_model));
    if (ret == NULL)
    {
        fprintf(stderr, "Error: malloc() (fruit_model_new())\n");
        exit(1);
    }

    ret->name = name;
    sprintf(filename, "res/%s.png", name);
    ret->img = utils_load_texture(game_renderer, filename);
    ret->is_fruit = is_fruit;
    ret->radius = FRUIT_RADIUS;

    return ret;
}

void fruit_model_append(s_game *game, s_fruit_model *new)
{
    int i = 0;

    for (i = 0; i < FRUITS_MODEL_COUNT; ++i)
    {
        if (game->models[i] == NULL)
        {
            game->models[i] = new;
            game->loaded_models ++;
            return;
        }
    }

    fprintf(stderr, "Error: no free slot (fruit_model_append())\n");
    exit(1);
}

/*
    fruit
*/

s_fruit *fruit_new(s_fruit_model *model)
{
    s_fruit *ret = NULL;

    ret = malloc(sizeof(s_fruit));
    if (ret == NULL)
    {
        fprintf(stderr, "Error: malloc() (fruit_new())\n");
        exit(1);
    }

    ret->x = utils_rand_int(0, SCREEN_WIDTH);
    ret->y = SCREEN_HEIGHT-0;
    ret->sx = utils_rand_int(-25, 25);
    ret->sy = utils_rand_int(-30, -15);
    ret->ax = 0;
    ret->ay = 1;
    ret->is_sliced = 0;
    ret->model = model;

    return ret;
}


void fruit_append(s_fruit **fruits, s_fruit *new)
{
    int i = 0;

    for (i = 0; i < FRUITS_COUNT; ++i)
    {
        if (fruits[i] == NULL)
        {
            fruits[i] = new;
            return;
        }
    }

    fprintf(stderr, "Error: no free slot (fruit_append())\n");
    exit(1);
}


void fruit_update_all(s_game *game)
{
    int i = 0;
    s_fruit **fruits = NULL;

    fruits = game->fruits;

    for (i = 0; i < FRUITS_COUNT; ++i)
    {
        if (fruits[i] != NULL)
        {
            int w = 0, h = 0, radius = 0;

            SDL_QueryTexture(fruits[i]->model->img, NULL, NULL, &w, &h);
            radius = fruits[i]->model->radius;

            // apply physics (basic)

            fruits[i]->sx += fruits[i]->ax;
            fruits[i]->sy += fruits[i]->ay;

            fruits[i]->x += fruits[i]->sx;
            fruits[i]->y += fruits[i]->sy;

            // apply physics (collisons)

            // make it bounce
            if (fruits[i]->x - radius < 0)
            {
                fruits[i]->x = - (fruits[i]->x - radius) + radius;
                fruits[i]->sx = -BOUNCE_COEFF * fruits[i]->sx;
            }
            if (fruits[i]->x + radius >= SCREEN_WIDTH)
            {
                fruits[i]->x = SCREEN_WIDTH - radius - (fruits[i]->x + radius - SCREEN_WIDTH);
                fruits[i]->sx = -BOUNCE_COEFF * fruits[i]->sx;
            }

            // if under the floor, remove it from the list of active fruits
            if (
                (fruits[i]->y - radius >= SCREEN_HEIGHT)
                && (! fruits[i]->is_sliced)
            )
            {
                if (fruits[i]->model->is_fruit) // missed a fruit
                    game->lives --;

                free(fruits[i]);
                fruits[i] = NULL;
            }
        }
    }
}

void fruit_blit_all(s_game *game)
{
    int i = 0;

    for (i = 0; i < FRUITS_COUNT; ++i)
    {
        if (game->fruits[i] != NULL)
        {
            if (game->fruits[i]->is_sliced)
                SDL_SetTextureColorMod(game->fruits[i]->model->img, 128, 128, 128);
            else
                SDL_SetTextureColorMod(game->fruits[i]->model->img, 255, 255, 255);

            utils_blit_at(
                game->fruits[i]->model->img, game->renderer,
                game->fruits[i]->x - game->fruits[i]->model->radius, game->fruits[i]->y - game->fruits[i]->model->radius
            );
        }
    }
}
