#ifndef logo_H_INCLUDED
#define logo_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define LOGO_MODELS_COUNT       10
#define LOGOS_COUNT             10
#define LINES_COUNT             100
#define BOUNCE_COEFF            0.75
#define LOGO_RADIUS             50


typedef struct      _s_logo_model {
    char            *name;
    SDL_Texture     *img;
    int             is_open_source;                 // whether the player should capture it
    int             radius;                         // radius of logo in px - used for collision
}                   s_logo_model;

typedef struct      _s_logo {
    float x, y;                                     // position - center of logo
    float sx, sy;                                   // speed
    float ax, ay;                                   // acceleration

    int             is_captured;
    Uint32          captured_timestamp;

    s_logo_model    *model;
}                   s_logo;

typedef struct      _s_vector {
    int x, y;
}                   s_vector;

typedef struct      _s_game {
    SDL_Window      *window;
    SDL_Renderer    *renderer;

    SDL_Texture     *background;
    SDL_Texture     *net;
    s_vector        net_size;
    TTF_Font        *font_title;
    TTF_Font        *font_text;
    s_logo_model    *models[LOGO_MODELS_COUNT];     // logos models: various infos like img

    int             loaded_models;
    int             lives;
    int             points;
    int             last_logo_captured;             // timestamp. Used for combos
    s_vector        mouse;
    s_logo          *logos[LOGOS_COUNT];            // active logos (~ shown)
}                   s_game;


s_logo_model *logo_model_new(SDL_Renderer *game_renderer, int is_open_source, char *name);
void logo_model_append(s_game *game, s_logo_model *new);

s_logo *logo_new(s_logo_model *model);
void logo_append(s_logo **logos, s_logo *new);
void logo_update_all(s_game *game);
void logo_blit_all(s_game *game);


#endif
