
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


#define SCREEN_WIDTH            800
#define SCREEN_HEIGHT           600
#define FRUITS_COUNT            10
#define SCREEN_BG_COLOR(screen) SDL_MapRGB(screen->format, 200, 200, 200)


typedef enum        _e_fruit_model {
    FRUIT_MODEL_FIREFOX,
    FRUIT_MODEL_BLENDER,
    FRUIT_MODEL_PYTHON,
    FRUIT_MODEL_WINDOWS,
    FRUIT_MODEL_OSX,
    FRUIT_MODEL_LAST,
}                   e_fruit_model;

typedef struct      _s_fruit_model {
    int             is_fruit;  // should the player cut it ?
    char            *name;
    SDL_Surface     *img;
}                   s_fruit_model;

typedef struct      _s_fruit {
    int x, y;
    int sx, sy;
    int ax, ay;

    s_fruit_model   *model;
}                   s_fruit;

typedef struct      _s_game {
    SDL_Window      *window;
    SDL_Surface     *screen;
    s_fruit_model   *models[FRUIT_MODEL_LAST];
    s_fruit         *fruits[FRUITS_COUNT];
}                   s_game;


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


s_fruit *fruit_new(s_fruit_model *model)
{
    s_fruit *ret = NULL;

    ret = malloc(sizeof(s_fruit));
    if (ret == NULL)
    {
        fprintf(stderr, "Error: malloc() (fruit_new())\n");
        exit(1);
    }

    ret->x = 0;
    ret->y = 0;
    ret->sx = 12;
    ret->sy = 45;
    ret->ax = 0;
    ret->ay = -2;
    ret->model = model;

    return ret;
}

void fruits_update_all(s_fruit **fruits)
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

            SDL_FillRect(game->screen, NULL, SCREEN_BG_COLOR(game->screen));
            SDL_BlitSurface(game->fruits[i]->model->img, NULL, game->screen, &dst);
        }
    }
}


s_game *pn_init(void)
{
    s_game *game = NULL;

    game = malloc(sizeof(s_game));
    if (game == NULL)
    {
        fprintf(stderr, "Error malloc() (pn_init())\n");
        exit(1);
    }
    memset(game, 0, sizeof(s_game));

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Error SDL_Init(): %s\n", SDL_GetError());
        exit(1);
    }

    game->window = SDL_CreateWindow(
        "Open Source Ninja",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (game->window == NULL)
    {
        fprintf(stderr, "Error SDL_CreateWindow(): %s\n", SDL_GetError());
        exit(1);
    }

    game->screen = SDL_GetWindowSurface(game->window);

    game->models[FRUIT_MODEL_FIREFOX] = fruit_model_new(1, "Firefox");
    game->models[FRUIT_MODEL_BLENDER] = fruit_model_new(1, "Blender");
    game->models[FRUIT_MODEL_PYTHON] = fruit_model_new(1, "Gimp");
    game->models[FRUIT_MODEL_PYTHON] = fruit_model_new(1, "vlc");
    game->models[FRUIT_MODEL_WINDOWS] = fruit_model_new(0, "Windows");
    game->models[FRUIT_MODEL_OSX] = fruit_model_new(0, "OS X");
    // vlc

    game->fruits[0] = fruit_new(game->models[FRUIT_MODEL_FIREFOX]);

    return game;
}


void pn_update_events(int *quit)
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            *quit = 1;
        }
        else if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_q:
                case SDLK_ESCAPE:
                    *quit = 1;
                    break;

                // case SDLK_UP:
                //     gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_UP ];
                //     break;
                // case SDLK_DOWN:
                //     gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DOWN ];
                //     break;

                default:
                    // gCurrentSurface = gKeyPressSurfaces[ KEY_PRESS_SURFACE_DEFAULT ];
                    break;
            }
        }
        // else if (e.type == SDL_MOUSEMOTION)
        // {
        //     printf("%d %d\n", e.motion.x, e.motion.y);
        // }
    }

}


int main(int argc, char *argv[])
{
    s_game *game;
    int quit = 0;

    game = pn_init();

    while (!quit)
    {
        pn_update_events(&quit);

        fruits_update_all(game->fruits);

        fruit_blit_all(game);

        SDL_UpdateWindowSurface(game->window);

        // printf("%d %d | %d %d | %d %d\n", fruits[0]->ax, fruits[0]->ay, fruits[0]->sx, fruits[0]->sy, fruits[0]->x, fruits[0]->y);

        SDL_Delay(1000/20);
    }

    // todo free fruit
    SDL_DestroyWindow(game->window);
    SDL_Quit();

    return 0;
}
