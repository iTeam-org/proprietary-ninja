
#include <stdlib.h>

#include "utils.h"

int rand_int(int min, int max)
{
    return rand()%(max-min) + min;
}
