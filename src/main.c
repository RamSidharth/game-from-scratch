// I am now making the box move 

#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // ==========================
    // INITIALIZE SDL
    // ==========================

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }


    // ==========================
    // CREATE WINDOW
    // ==========================

    SDL_Window *window = SDL_CreateWindow(
        "My First Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        0
    );

    if (window == NULL)
    {
        printf("Window creation failed: %s\n", SDL_GetError());

        SDL_Quit();

        return 1;
    }


    // ==========================
    // CREATE RENDERER
    // ==========================

    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (renderer == NULL)
    {
        printf(
            "Renderer creation failed: %s\n",
            SDL_GetError()
        );

        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }


    // ==========================
    // GAME STATE
    // ==========================

    int running = 1;

    SDL_Event event;


    // Player position
    float playerX = 375.0f;
    float playerY = 275.0f;

    // Player size
    int playerWidth = 50;
    int playerHeight = 50;

    // SDL rectangle used for rendering
    SDL_Rect player = {
        375,
        275,
        playerWidth,
        playerHeight
    };


    // ==========================
    // TIME
    // ==========================

    Uint32 lastTime = SDL_GetTicks();


    // ==========================
    // GAME LOOP
    // ==========================

    while (running)
    {

        // ----------------------
        // CALCULATE DELTA TIME
        // ----------------------

        Uint32 currentTime = SDL_GetTicks();

        float deltaTime =
            (currentTime - lastTime) / 1000.0f;

        lastTime = currentTime;


        // ----------------------
        // HANDLE EVENTS
        // ----------------------

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
        }


        // ----------------------
        // KEYBOARD INPUT
        // ----------------------

        const Uint8 *keyboard =
            SDL_GetKeyboardState(NULL);

        float speed = 300.0f;


        if (keyboard[SDL_SCANCODE_LEFT])
        {
            playerX -= speed * deltaTime;
        }

        if (keyboard[SDL_SCANCODE_RIGHT])
        {
            playerX += speed * deltaTime;
        }

        if (keyboard[SDL_SCANCODE_UP])
        {
            playerY -= speed * deltaTime;
        }

        if (keyboard[SDL_SCANCODE_DOWN])
        {
            playerY += speed * deltaTime;
        }


        // ----------------------
        // KEEP PLAYER INSIDE
        // WINDOW
        // ----------------------

        if (playerX < 0)
        {
            playerX = 0;
        }

        if (playerX > 800 - playerWidth)
        {
            playerX = 800 - playerWidth;
        }

        if (playerY < 0)
        {
            playerY = 0;
        }

        if (playerY > 600 - playerHeight)
        {
            playerY = 600 - playerHeight;
        }


        // ----------------------
        // UPDATE PLAYER RECTANGLE
        // ----------------------

        player.x = (int)playerX;
        player.y = (int)playerY;


        // ======================
        // RENDER
        // ======================

        // Clear screen
        SDL_SetRenderDrawColor(
            renderer,
            20,
            20,
            20,
            255
        );

        SDL_RenderClear(renderer);


        // Draw player
        SDL_SetRenderDrawColor(
            renderer,
            255,
            255,
            255,
            255
        );

        SDL_RenderFillRect(
            renderer,
            &player
        );


        // Show frame
        SDL_RenderPresent(renderer);
    }


    // ==========================
    // CLEANUP
    // ==========================

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}