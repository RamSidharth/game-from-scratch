#include "game.h"
#include "input.h"
#include "renderer.h"

#include <SDL2/SDL.h>

#include <stdlib.h>
#include <time.h>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    srand((unsigned int)time(NULL));

    Renderer renderer;
    Input input;
    Game game;

    /*
        Initialize renderer
    */
    if (!renderer_init(&renderer))
    {
        return 1;
    }

    /*
        Initialize input
    */
    input_init(&input);

    /*
        Initialize game
    */
    game_init(&game);

    /*
        High-resolution timer
    */
    Uint64 previousCounter =
        SDL_GetPerformanceCounter();

    const double performanceFrequency =
        (double)SDL_GetPerformanceFrequency();

    while (input.running)
    {
        /*
            Calculate delta time
        */
        Uint64 currentCounter =
            SDL_GetPerformanceCounter();

        float deltaTime =
            (float)(
                (currentCounter - previousCounter) /
                performanceFrequency
            );

        previousCounter = currentCounter;

        /*
            Prevent huge time jumps
        */
        if (deltaTime > 0.1f)
        {
            deltaTime = 0.1f;
        }

        /*
            Process input
        */
        input_process(
            &input,
            renderer.window
        );

        /*
            MENU
        */
        if (game.state == GAME_STATE_MENU)
        {
            /*
                ENTER starts the game
            */
            if (input.start)
            {
                game_start(&game);
            }

            /*
                Mouse click on START GAME
            */
            else if (input.mouseClicked &&
                     renderer_menu_start_button_contains(
                         &renderer,
                         input.mouseX,
                         input.mouseY
                     ))
            {
                game_start(&game);
            }
        }

        /*
            PLAYING
        */
        else if (game.state == GAME_STATE_PLAYING)
        {
            if (input.restart)
            {
                game_restart(&game);
            }
        }

        /*
            GAME OVER
        */
        else if (game.state == GAME_STATE_GAME_OVER)
        {
            if (input.restart)
            {
                game_restart(&game);
            }
            else if (input.start)
            {
                game.state = GAME_STATE_MENU;
            }
        }

        /*
            Update gameplay
        */
        game_update(
            &game,
            input_get_keyboard(),
            deltaTime
        );

        /*
            Render
        */
        renderer_render(
            &renderer,
            &game,
            input.mouseX,
            input.mouseY
        );
    }

    /*
        Cleanup
    */
    renderer_cleanup(&renderer);

    return 0;
}