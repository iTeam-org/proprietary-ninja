
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "utils.h"
#include "fruit.h"
#include "line.h"


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

    game->lives = 3;

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

    if (
        DIST_SQUARE(xrel, yrel) > SQUARE(MIN_DIST_TO_CUT)
        && ((x != xrel) && (y != yrel))  // happens on first loop
    )
    {
        line_append(game, line_new(x, y, x+xrel, y+yrel));
    }
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
int pn_check_collision(s_logo *logo, s_line *line)
{
    float line_unit_x = 0, line_unit_y = 0;  // unit vector of the line
    int line2logo_dx = 0, line2logo_dy = 0;
    float scalar_projection = 0;
    int projection_x = 0, projection_y = 0;

    // project the center of the logo onto the cuting line

    line_unit_x = (line->x2 - line->x1) / line->norm;
    line_unit_y = (line->y2 - line->y1) / line->norm;

    line2logo_dx = logo->x - line->x1;
    line2logo_dy = logo->y - line->y1;

    scalar_projection = line2logo_dx*line_unit_x + line2logo_dy*line_unit_y;

    projection_x = line->x1 + line_unit_x*scalar_projection;
    projection_y = line->y1 + line_unit_y*scalar_projection;

    // if the projected center of the logo (which is on the cuting line) is
    // inside the logo, the logo is sliced

    if (
        (scalar_projection > 0) && (scalar_projection < line->norm)  // the projection is within the line
        && (DIST_SQUARE(projection_x-logo->x, projection_y-logo->y) < SQUARE(logo->model->radius))  // the projection is within the logo
        && (SDL_GetTicks() - line->timestamp < 2 * 1000/FPS)  // and not too old
    )
    {
        return 1;
    }

    return 0;
}

void pn_check_all_collisions(s_game *game)
{
    int i = 0, j = 0;

    for (i = 0; i < LOGOS_COUNT; ++i)
    {
        if (game->logos[i] != NULL)
        {
            for (j = 0; j < LINES_COUNT; ++j)
            {
                if (game->lines[j] != NULL)
                {
                    if (pn_check_collision(game->logos[i], game->lines[j]))
                    {
                        game->last_logo_captured = SDL_GetTicks();

                        game->logos[i]->is_captured = 1;

                        game->points += 100;
                        // todo more points in function of number of logos sliced (combos)
                    }
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
        line_blit_all(game);
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
