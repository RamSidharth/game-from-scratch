#ifndef PLAYER_H
#define PLAYER_H

typedef struct
{
    float x;
    float y;
} Player;

void player_reset(Player* player);
void player_update(Player* player, const unsigned char* keyboard, float deltaTime);

#endif