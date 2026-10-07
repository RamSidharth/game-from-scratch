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

    renderer_init(&renderer);
    input_init(&input);
    game_reset(&game);

    Uint64 previousCounter =
        SDL_GetPerformanceCounter();

    const double performanceFrequency =
        (double)SDL_GetPerformanceFrequency();

    while (input.running)
    {
        Uint64 currentCounter =
            SDL_GetPerformanceCounter();

        float deltaTime =
            (float)(
                (currentCounter - previousCounter) /
                performanceFrequency
            );

        previousCounter = currentCounter;

        if (deltaTime > 0.1f)
            deltaTime = 0.1f;

        input_process(
            &input,
            renderer.window
        );

        if (input.restart && game.gameOver)
        {
            game_reset(&game);
            input.restart = 0;
        }

        game_update(
            &game,
            input_get_keyboard(),
            deltaTime
        );

        renderer_render(
            &renderer,
            &game
        );
    }

    renderer_cleanup(&renderer);

    return 0;
}