#include "player.h"
#include "config.h"

#include <SDL2/SDL.h>
#include <math.h>

void player_reset(Player* player)
{
    player->x = (GAME_WIDTH - PLAYER_SIZE) / 2.0f;
    player->y = (GAME_HEIGHT - PLAYER_SIZE) / 2.0f;
}

void player_update(
    Player* player,
    const unsigned char* keyboard,
    float deltaTime
)
{
    float moveX = 0.0f;
    float moveY = 0.0f;

    if (keyboard == NULL)
    {
        return;
    }

    /* A / Left */
    if (keyboard[SDL_SCANCODE_A] ||
        keyboard[SDL_SCANCODE_LEFT])
    {
        moveX -= 1.0f;
    }

    /* D / Right */
    if (keyboard[SDL_SCANCODE_D] ||
        keyboard[SDL_SCANCODE_RIGHT])
    {
        moveX += 1.0f;
    }

    /* W / Up */
    if (keyboard[SDL_SCANCODE_W] ||
        keyboard[SDL_SCANCODE_UP])
    {
        moveY -= 1.0f;
    }

    /* S / Down */
    if (keyboard[SDL_SCANCODE_S] ||
        keyboard[SDL_SCANCODE_DOWN])
    {
        moveY += 1.0f;
    }

    /* Normalize diagonal movement */
    if (moveX != 0.0f || moveY != 0.0f)
    {
        float length = sqrtf(
            moveX * moveX +
            moveY * moveY
        );

        moveX /= length;
        moveY /= length;
    }

    /* Frame-independent movement */
    player->x += moveX * PLAYER_SPEED * deltaTime;
    player->y += moveY * PLAYER_SPEED * deltaTime;

    /* Keep player inside game area */
    if (player->x < 0.0f)
    {
        player->x = 0.0f;
    }

    if (player->y < 0.0f)
    {
        player->y = 0.0f;
    }

    if (player->x > GAME_WIDTH - PLAYER_SIZE)
    {
        player->x = GAME_WIDTH - PLAYER_SIZE;
    }

    if (player->y > GAME_HEIGHT - PLAYER_SIZE)
    {
        player->y = GAME_HEIGHT - PLAYER_SIZE;
    }
}