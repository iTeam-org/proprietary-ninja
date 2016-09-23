#ifndef FRUIT_H_INCLUDED
#define FRUIT_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define FRUITS_MODEL_COUNT      10
#define FRUITS_COUNT            10
#define LINES_COUNT             100
#define BOUNCE_COEFF            0.75


typedef struct      _s_fruit_model {
    int             is_fruit;                       // whether the player should cut it
    char            *name;
    SDL_Texture     *img;
}                   s_fruit_model;

typedef struct      _s_fruit {
    int x, y;                                       // position
    int sx, sy;                                     // speed
    int ax, ay;                                     // acceleration

    s_fruit_model   *model;
}                   s_fruit;

typedef struct      _s_line {
    int x1, y1, x2, y2;
    int dx, dy;                                     // lenght in x and y. Auto calculated from coordinates
    float a, b;                                     // equation y = a*x+b. Auto calculated from coordinates
    float norm;                                     // auto calculated from coordinates
    Uint32 timestamp;                               // creation timestamp
}                   s_line;

typedef struct      _s_game {
    SDL_Window      *window;
    SDL_Renderer    *renderer;

    SDL_Texture     *background;
    TTF_Font        *font_title;
    TTF_Font        *font_text;
    s_fruit_model   *models[FRUITS_MODEL_COUNT];    // fruits models: various infos like img

    int             loaded_models;
    int             lives;
    s_fruit         *fruits[FRUITS_COUNT];          // active fruits (~ shown)
    s_line          *lines[LINES_COUNT];            // active lines
}                   s_game;


s_fruit_model *fruit_model_new(SDL_Renderer *game_renderer, int is_fruit, char *name);
void fruit_model_append(s_game *game, s_fruit_model *new);

s_fruit *fruit_new(s_fruit_model *model);
void fruit_append(s_fruit **fruits, s_fruit *new);
void fruit_update_all(s_game *game);
void fruit_blit_all(s_game *game);


#endif
