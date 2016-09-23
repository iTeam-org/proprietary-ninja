#ifndef LINE_H_INCLUDED
#define LINE_H_INCLUDED

#define MIN_DIST_TO_CUT     20
#define LINE_SHOW_TIME      1000

#include "fruit.h"

s_line *line_new(int x1, int y1, int x2, int y2);
void line_append(s_game *game, s_line *new);
void line_blit_all(s_game *game);

#endif
