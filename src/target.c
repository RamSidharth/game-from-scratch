#include "target.h"
#include "config.h"

#include <stdlib.h>

void target_spawn(Target* target)
{
    target->x =
        (float)(rand() % (GAME_WIDTH - TARGET_SIZE));

    target->y =
        (float)(rand() % (GAME_HEIGHT - TARGET_SIZE));
}