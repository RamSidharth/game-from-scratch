#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

typedef struct
{
    int running;

    int start;
    int restart;

    int toggleFullscreen;

    int mouseX;
    int mouseY;
    int mouseClicked;
} Input;

void input_init(Input* input);

void input_process(
    Input* input,
    SDL_Window* window
);

const Uint8* input_get_keyboard(void);

#endif