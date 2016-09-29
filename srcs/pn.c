
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "utils.h"
#include "logo.h"


#define SLEEP_AFTER_LOST    3000
#define LIVE_COUNT          5


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
        GAME_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP
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

    game->lives = LIVE_COUNT;

    SDL_GetWindowSize(game->window, &game->window_size.x, &game->window_size.y);
    printf("Window size: %d %d\n", game->window_size.x, game->window_size.y);

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

void pn_events_update(int *quit, s_game *game, int *key_down)
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
                if (key_down)
                    *key_down = 1;

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
                if (game != NULL)
                    pn_events_handle_movement(game, e.motion.x, e.motion.y, e.motion.xrel, e.motion.yrel);
                break;

            case SDL_FINGERDOWN:
                if (key_down)
                    *key_down = 1;
                break;

            case SDL_FINGERMOTION:
                if (game != NULL)
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

                    printf("Captured\n");
                    game->points += 100;

                    if (now - game->last_logo_captured < 500)
                    {
                        printf("Combo !\n");
                        game->points += 100;
                    }

                    game->last_logo_captured = now;
                }
            }
        }
    }
}

void you_just_lost(s_game *game)
{
    SDL_Rect rect;

    printf("todo - you just lost\n");

    SDL_SetRenderDrawColor(game->renderer, 128, 128, 128, 128);

    rect.x = game->window_size.x/2-200;
    rect.y = game->window_size.y/2-50;
    rect.w = 2*200;
    rect.h = 2*50;
    SDL_RenderFillRect(game->renderer, &rect);

    utils_text(
        game->renderer, game->font_title, "You just lost !",
        game->window_size.x/2, game->window_size.y/2-10, TEXT_ALIGN_CENTER
    );

    SDL_RenderPresent(game->renderer);

    SDL_Delay(SLEEP_AFTER_LOST);

    pn_free(game);

    exit(0);
}


int pn_main(s_game *game)
{
    Uint32 timestamp = 0;
    int should_sleep = 0;
    int quit = 0;

    while (!quit)
    {
        timestamp = SDL_GetTicks();

        // events
        pn_events_update(&quit, game, NULL);

        // new logo
        if (utils_rand_int(0, 1000) < 25)
           logo_append(game->logos, logo_new(game->models[utils_rand_int(0, game->loaded_models)], game->window_size));

        // update physics
        logo_update_all(game);
        pn_check_all_collisions(game);

        if (game->lives == 0)
            you_just_lost(game);

        /*
            screen
        */

        // bg
        SDL_SetRenderDrawColor(game->renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(game->renderer);
        utils_blit_atsize(game->background, game->renderer, 0, 100, game->window_size.x, game->window_size.y-100);

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
}

void menu(s_game *game)
{
    int quit = 0;
    int i = 0, logo_open_source = 0, logo_proprio = 0;

    while (!quit)
    {
        pn_events_update(&quit, NULL, &quit);

        // bg
        SDL_SetRenderDrawColor(game->renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(game->renderer);

        // titles

#define OS_X            100
#define OS_Y            100
#define PROPRIO_X       100
#define PROPRIO_Y       400
#define LOGOS_PER_LINE  6
#define LOGO_MARGIN     50

        utils_text(game->renderer, game->font_title, "Open source", 50, OS_Y, TEXT_ALIGN_LEFT);
        utils_text(game->renderer, game->font_title, "Closed source", 50, PROPRIO_Y, TEXT_ALIGN_LEFT);

        // logos
        logo_open_source = 0;
        logo_proprio = 0;

        for (i = 0; i < LOGO_MODELS_COUNT; ++i)
        {
            if (game->models[i] == NULL)
                continue;

            if (game->models[i]->is_open_source)
            {
                utils_blit_at(
                    game->models[i]->img,
                    game->renderer,
                    OS_X + (logo_open_source % LOGOS_PER_LINE) * (game->models[i]->radius + LOGO_MARGIN),
                    OS_Y + 50 + (logo_open_source / LOGOS_PER_LINE) * (game->models[i]->radius + LOGO_MARGIN)
                );

                logo_open_source ++;
            }
            else
            {
                utils_blit_at(
                    game->models[i]->img,
                    game->renderer,
                    PROPRIO_X + (logo_proprio % LOGOS_PER_LINE) * (game->models[i]->radius + LOGO_MARGIN),
                    PROPRIO_Y + 50 + (logo_proprio / LOGOS_PER_LINE) * (game->models[i]->radius + LOGO_MARGIN)
                );

                logo_proprio ++;
            }
        }

        // render
        SDL_RenderPresent(game->renderer);

        SDL_Delay(1000/FPS);
    }
}

int main(void)
{
    s_game *game;

    srand(42);
    game = pn_init();

    menu(game);
    pn_main(game);


    pn_free(game);

    return 0;
}
