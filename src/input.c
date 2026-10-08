#include "input.h"

#include <SDL2/SDL.h>

static const Uint8* keyboardState = NULL;

void input_init(Input* input)
{
    input->running = 1;

    input->start = 0;
    input->restart = 0;

    input->toggleFullscreen = 0;

    input->mouseX = 0;
    input->mouseY = 0;
    input->mouseClicked = 0;

    keyboardState =
        SDL_GetKeyboardState(NULL);
}

void input_process(
    Input* input,
    SDL_Window* window
)
{
    SDL_Event event;

    /*
        Reset one-frame actions
    */
    input->start = 0;
    input->restart = 0;
    input->toggleFullscreen = 0;
    input->mouseClicked = 0;

    /*
        Process events
    */
    while (SDL_PollEvent(&event))
    {
        /*
            Window closed
        */
        if (event.type == SDL_QUIT)
        {
            input->running = 0;
        }

        /*
            Mouse movement
        */
        if (event.type == SDL_MOUSEMOTION)
        {
            input->mouseX = event.motion.x;
            input->mouseY = event.motion.y;
        }

        /*
            Left mouse button
        */
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                input->mouseX = event.button.x;
                input->mouseY = event.button.y;

                input->mouseClicked = 1;
            }
        }

        /*
            Keyboard
        */
        if (event.type == SDL_KEYDOWN &&
            !event.key.repeat)
        {
            SDL_Keycode key =
                event.key.keysym.sym;

            /*
                ESC = quit
            */
            if (key == SDLK_ESCAPE)
            {
                input->running = 0;
            }

            /*
                ENTER = start
            */
            if (key == SDLK_RETURN ||
                key == SDLK_KP_ENTER)
            {
                input->start = 1;
            }

            /*
                R = restart
            */
            if (key == SDLK_r)
            {
                input->restart = 1;
            }

            /*
                F11 = fullscreen
            */
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

    /*
        Update keyboard state
    */
    keyboardState =
        SDL_GetKeyboardState(NULL);

    /*
        Update mouse position even when
        no mouse event occurred this frame.
    */
    SDL_GetMouseState(
        &input->mouseX,
        &input->mouseY
    );
}

const Uint8* input_get_keyboard(void)
{
    return keyboardState;
}