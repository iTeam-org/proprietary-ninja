
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

s_fruit_model *fruit_model_new(int is_fruit, char *name)
{
    s_fruit_model *ret = NULL;
    char filename[1024];

    ret = malloc(sizeof(s_fruit_model));
    if (ret == NULL)
    {
        fprintf(stderr, "Error: malloc() (fruit_model_new())\n");
        exit(1);
    }

    ret->is_fruit = is_fruit;
    ret->name = name;
    sprintf(filename, "res/%s.png", name);
    ret->img = IMG_Load(filename);
    if (ret->img == NULL)
    {
        fprintf(stderr, "Error IMG_Load() (fruit_model_new()): %s\n", IMG_GetError());
        exit(1);
    }

    return ret;
}

void fruit_model_append(s_fruit_model **models, s_fruit_model *new)
{
    int i = 0;

    for (i = 0; i < FRUITS_MODEL_COUNT; ++i)
    {
        if (models[i] == NULL)
        {
            models[i] = new;
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

    ret->x = rand_int(0, SCREEN_WIDTH);
    ret->y = 0;
    ret->sx = rand_int(-30, 30);
    ret->sy = rand_int(20, 60);
    ret->ax = 0;
    ret->ay = -2;
    ret->model = model;

    printf("[Debug] new fruit\n");

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


void fruit_update_all(s_fruit **fruits)
{
    int i = 0;

    for (i = 0; i < FRUITS_COUNT; ++i)
    {
        if (fruits[i] != NULL)
        {
            fruits[i]->sx += fruits[i]->ax;
            fruits[i]->sy += fruits[i]->ay;

            fruits[i]->x += fruits[i]->sx;
            fruits[i]->y += fruits[i]->sy;

            // make it bounce
            if (fruits[i]->x < 0)
            {
                fruits[i]->x = -fruits[i]->x;
                fruits[i]->sx = -BOUNCE_COEFF * fruits[i]->sx;
            }
            if (fruits[i]->x+fruits[i]->model->img->w >= SCREEN_WIDTH)
            {
                fruits[i]->x = SCREEN_WIDTH - (fruits[i]->x+fruits[i]->model->img->w - SCREEN_WIDTH) - fruits[i]->model->img->w;
                fruits[i]->sx = -BOUNCE_COEFF * fruits[i]->sx;
            }

            // if under the floor, remove it from the list of active fruits
            // todo
        }
    }
}

void fruit_blit_all(s_game *game)
{
    int i = 0;
    SDL_Rect dst;

    for (i = 0; i < FRUITS_COUNT; ++i)
    {
        if (game->fruits[i] != NULL)
        {
            dst.x = game->fruits[i]->x;
            dst.y = SCREEN_HEIGHT - game->fruits[i]->y;
            dst.w = game->fruits[i]->model->img->w;
            dst.h = game->fruits[i]->model->img->h;

            SDL_BlitSurface(game->fruits[i]->model->img, NULL, game->screen, &dst);
        }
    }
}
