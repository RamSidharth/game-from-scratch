#include "game.h"
#include "config.h"

#include <SDL2/SDL.h>

void game_init(Game* game)
{
    game->score = 0;
    game->targetTimer = TARGET_TIME;
    game->state = GAME_STATE_MENU;

    player_reset(&game->player);
    target_spawn(&game->target);
}

void game_start(Game* game)
{
    player_reset(&game->player);
    target_spawn(&game->target);

    game->score = 0;
    game->targetTimer = TARGET_TIME;
    game->state = GAME_STATE_PLAYING;
}

void game_restart(Game* game)
{
    game_start(game);
}

void game_update(
    Game* game,
    const unsigned char* keyboard,
    float deltaTime
)
{
    if (game->state != GAME_STATE_PLAYING)
    {
        return;
    }

    /*
        Update player
    */
    player_update(
        &game->player,
        keyboard,
        deltaTime
    );

    /*
        Update target timer
    */
    game->targetTimer -= deltaTime;

    if (game->targetTimer <= 0.0f)
    {
        game->targetTimer = 0.0f;
        game->state = GAME_STATE_GAME_OVER;

        return;
    }

    /*
        Build collision rectangles
    */
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

    /*
        Collision
    */
    if (SDL_HasIntersection(
            &playerRect,
            &targetRect))
    {
        game->score += 10;

        target_spawn(&game->target);

        game->targetTimer = TARGET_TIME;
    }
}