#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "target.h"

typedef struct
{
    Player player;
    Target target;

    int score;
    float targetTimer;
    int gameOver;
} Game;

void game_reset(Game* game);
void game_update(
    Game* game,
    const unsigned char* keyboard,
    float deltaTime
);

#endif