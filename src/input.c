#include "input.h"

#include <SDL2/SDL.h>

static const Uint8* keyboardState = NULL;

void input_init(Input* input)
{
    input->running = 1;
    input->restart = 0;
    input->toggleFullscreen = 0;

    keyboardState = SDL_GetKeyboardState(NULL);
}

void input_process(
    Input* input,
    SDL_Window* window
)
{
    SDL_Event event;

    /* Reset one-frame actions */
    input->restart = 0;
    input->toggleFullscreen = 0;

    while (SDL_PollEvent(&event))
    {
        /* Window closed */
        if (event.type == SDL_QUIT)
        {
            input->running = 0;
        }

        /* Keyboard events */
        if (event.type == SDL_KEYDOWN &&
            !event.key.repeat)
        {
            SDL_Keycode key =
                event.key.keysym.sym;

            /* ESC = quit */
            if (key == SDLK_ESCAPE)
            {
                input->running = 0;
            }

            /* R = restart */
            if (key == SDLK_r)
            {
                input->restart = 1;
            }

            /* F11 = fullscreen */
            if (key == SDLK_F11)
            {
                input->toggleFullscreen = 1;

                Uint32 flags =
                    SDL_GetWindowFlags(window);

                if (flags &
                    SDL_WINDOW_FULLSCREEN_DESKTOP)
                {
                    SDL_SetWindowFullscreen(
                        window,
                        0
                    );
                }
                else
                {
                    SDL_SetWindowFullscreen(
                        window,
                        SDL_WINDOW_FULLSCREEN_DESKTOP
                    );
                }
            }
        }
    }

    keyboardState = SDL_GetKeyboardState(NULL);
}

const Uint8* input_get_keyboard(void)
{
    return keyboardState;
}