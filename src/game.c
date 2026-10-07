#include "game.h"
#include "config.h"

#include <SDL2/SDL.h>

void game_reset(Game* game)
{
    player_reset(&game->player);
    target_spawn(&game->target);

    game->score = 0;
    game->targetTimer = TARGET_TIME;
    game->gameOver = 0;
}

void game_update(
    Game* game,
    const unsigned char* keyboard,
    float deltaTime
)
{
    if (game->gameOver)
    {
        return;
    }

    /* Update player */
    player_update(
        &game->player,
        keyboard,
        deltaTime
    );

    /* Update target timer */
    game->targetTimer -= deltaTime;

    if (game->targetTimer <= 0.0f)
    {
        game->targetTimer = 0.0f;
        game->gameOver = 1;

        return;
    }

    /* Build collision rectangles */
    SDL_Rect playerRect =
    {
        (int)game->player.x,
        (int)game->player.y,
        PLAYER_SIZE,
        PLAYER_SIZE
    };

    SDL_Rect targetRect =
    {
        (int)game->target.x,
        (int)game->target.y,
        TARGET_SIZE,
        TARGET_SIZE
    };

    /* Collision */
    if (SDL_HasIntersection(
            &playerRect,
            &targetRect))
    {
        game->score += 10;

        target_spawn(&game->target);

        game->targetTimer = TARGET_TIME;
    }
}