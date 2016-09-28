
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "utils.h"
#include "logo.h"


s_game *pn_init(void)
{
    s_game *game = NULL;

    /*
        Malloc of main struct
    */

    game = malloc(sizeof(s_game));
    if (game == NULL)
    {
        fprintf(stderr, "Error malloc() (pn_init())\n");
        exit(1);
    }
    memset(game, 0, sizeof(s_game));

    /*
        SDL
    */

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Error SDL_Init(): %s\n", SDL_GetError());
        exit(1);
    }

    /*
        Window and screen surface
    */

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

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);
    if (game->renderer == NULL)
    {
        fprintf(stderr, "Error SDL_CreateRenderer(): %s\n", SDL_GetError());
        exit(1);
    }

    game->background = utils_load_texture(game->renderer, "res/background.jpg");
    game->net = utils_load_texture(game->renderer, "res/net.png");
    SDL_QueryTexture(game->net, NULL, NULL, &game->net_size.x, &game->net_size.y);

    /*
        SDL_ttf
    */

    if (TTF_Init() == -1)
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

    game->font_title = TTF_OpenFont("res/Courier_New.ttf", 32);
    if (game->font_title == NULL)
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    TTF_SetFontStyle(game->font_title, TTF_STYLE_BOLD | TTF_STYLE_UNDERLINE);

    game->font_text = TTF_OpenFont("res/Courier_New.ttf", 20);
    if (game->font_text == NULL)
        printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());

    /*
        logos
    */

    logo_model_append(game, logo_model_new(game->renderer, 1, "Firefox"));
    logo_model_append(game, logo_model_new(game->renderer, 1, "Blender"));
    logo_model_append(game, logo_model_new(game->renderer, 1, "Gimp"));
    logo_model_append(game, logo_model_new(game->renderer, 1, "vlc"));
    logo_model_append(game, logo_model_new(game->renderer, 0, "Windows"));
    logo_model_append(game, logo_model_new(game->renderer, 0, "OS X"));

    /*
        Miscs
    */

    game->lives = 5;

    return game;
}

void pn_free(s_game *game)
{
    int i = 0;

    for (i = 0; i < LOGO_MODELS_COUNT; ++i)
    {
        if (game->models[i] != NULL)
        {
            SDL_DestroyTexture(game->models[i]->img);
            free(game->models[i]);
        }
    }

    for (i = 0; i < LOGOS_COUNT; ++i)
    {
        if (game->logos[i] != NULL)
            free(game->logos[i]);
    }

    TTF_CloseFont(game->font_title);
    TTF_CloseFont(game->font_text);
    SDL_DestroyTexture(game->background);
    SDL_DestroyTexture(game->net);

    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);

    free(game);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void pn_events_handle_movement(s_game *game, int x, int y, int xrel, int yrel)
{
    game->mouse.x = x;
    game->mouse.y = y;
}

void pn_events_update(int *quit, s_game *game)
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
            case SDL_QUIT:
                *quit = 1;
                break;

            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                    case SDLK_q:
                    case SDLK_ESCAPE:
                        *quit = 1;
                        break;

                    default:
                        break;
                }
                break;

            case SDL_MOUSEMOTION:
                pn_events_handle_movement(game, e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
                break;

            case SDL_FINGERMOTION:
                pn_events_handle_movement(game, e.tfinger.x, e.tfinger.y, e.tfinger.dx, e.tfinger.dy);
                break;

            default:
                break;
        }
    }
}

/*
    Return
        0 if no collision
        1 if collision
*/
int pn_check_collision(s_logo *logo, s_vector mouse)
{
    return (DIST_SQUARE(mouse.x - logo->x, mouse.y - logo->y) < SQUARE(logo->model->radius));
}

void pn_check_all_collisions(s_game *game)
{
    int i = 0;
    Uint32 now = 0;

    now = SDL_GetTicks();

    for (i = 0; i < LOGOS_COUNT; ++i)
    {
        if ((game->logos[i] != NULL) && !game->logos[i]->is_captured)
        {
            if (pn_check_collision(game->logos[i], game->mouse))
            {
                game->logos[i]->is_captured = 1;

                if (!game->logos[i]->model->is_open_source)
                {
                    game->lives --;
                }
                else
                {
                    game->logos[i]->captured_timestamp = now;

                    game->points += 100;
                    game->last_logo_captured = now;
                    // todo more points in function of number of logos sliced (combos)
                }
            }
        }
    }
}


int main(int argc, char *argv[])
{
    s_game *game;
    Uint32 timestamp = 0;
    int should_sleep = 0;
    int quit = 0;

    srand(42);
    game = pn_init();

    while (!quit)
    {
        timestamp = SDL_GetTicks();

        // events
        pn_events_update(&quit, game);

        // new logo
        if (utils_rand_int(0, 1000) < 25)
           logo_append(game->logos, logo_new(game->models[utils_rand_int(0, game->loaded_models)]));

        // update physics
        logo_update_all(game);
        pn_check_all_collisions(game);

        if (game->lives == 0)
            printf("todo - you just lost\n");

        /*
            screen
        */

        // bg
        SDL_SetRenderDrawColor(game->renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(game->renderer);
        utils_blit_at(game->background, game->renderer, 0, 100);

        // title, score, ...
        utils_blit_hud(game);

        // logos, lines and net
        logo_blit_all(game);
        utils_blit_at(game->net, game->renderer, game->mouse.x-game->net_size.x/4, game->mouse.y-game->net_size.y/3);

        // render
        SDL_RenderPresent(game->renderer);

        // sleep
        should_sleep = 1000/FPS - (SDL_GetTicks() - timestamp);
        if (should_sleep > 0)
            SDL_Delay(should_sleep);
    }

    pn_free(game);

    return 0;
}
