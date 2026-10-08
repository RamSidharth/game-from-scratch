#ifndef GAME_H
#define GAME_H

#include "player.h"
#include "target.h"

typedef enum
{
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_GAME_OVER
} GameState;

typedef struct
{
    Player player;
    Target target;

    int score;
    float targetTimer;

    GameState state;
} Game;

void game_init(Game* game);
void game_start(Game* game);
void game_restart(Game* game);
void game_update(
    Game* game,
    const unsigned char* keyboard,
    float deltaTime
);

#endif