#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

typedef struct
{
    int running;
    int restart;
    int toggleFullscreen;
} Input;

void input_init(Input* input);
void input_process(Input* input, SDL_Window* window);

const Uint8* input_get_keyboard(void);

#endif