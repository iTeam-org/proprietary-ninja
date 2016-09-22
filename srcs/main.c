
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "utils.h"
#include "fruit.h"


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

    game->background = IMG_Load("res/background.jpg");
    if (game->background == NULL)
    {
        fprintf(stderr, "Error IMG_Load() (pn_init()): %s\n", IMG_GetError());
        exit(1);
    }

    fruit_model_append(game, fruit_model_new(1, "Firefox"));
    fruit_model_append(game, fruit_model_new(1, "Blender"));
    fruit_model_append(game, fruit_model_new(1, "Gimp"));
    fruit_model_append(game, fruit_model_new(1, "vlc"));
    fruit_model_append(game, fruit_model_new(0, "Windows"));
    fruit_model_append(game, fruit_model_new(0, "OS X"));

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

    srand(42);
    game = pn_init();

    while (!quit)
    {
        // events
        pn_update_events(&quit);

        // new fruit
        if (utils_rand_int(0, 100) == 0)
           fruit_append(game->fruits, fruit_new(game->models[utils_rand_int(0, game->loaded_models)]));

        // update physics
        fruit_update_all(game->fruits);

        // screen
        SDL_FillRect(game->screen, NULL, SCREEN_BG_COLOR(game->screen));
        utils_blit_at(game->background, game->screen, 0, 100);
        fruit_blit_all(game);
        SDL_UpdateWindowSurface(game->window);

        // sleep
        SDL_Delay(1000/FPS);
    }

    // todo free fruit
    SDL_DestroyWindow(game->window);
    SDL_Quit();

    return 0;
}
