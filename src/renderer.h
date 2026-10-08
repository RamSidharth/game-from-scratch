#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "game.h"

typedef struct
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* gameTexture;
    TTF_Font* font;
} Renderer;

int renderer_init(Renderer* renderer);

void renderer_render(
    Renderer* renderer,
    const Game* game,
    int mouseX,
    int mouseY
);

int renderer_menu_start_button_contains(
    Renderer* renderer,
    int mouseX,
    int mouseY
);

void renderer_cleanup(Renderer* renderer);

#endif