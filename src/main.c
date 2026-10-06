// Gave a basic point system

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    SDL_Window *window = SDL_CreateWindow(
        "My First Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL)
    {
        printf("Window Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (renderer == NULL)
    {
        printf("Renderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Random number generator
    srand((unsigned int)time(NULL));

    // Player
    float playerX = 375.0f;
    float playerY = 275.0f;

    int playerWidth = 50;
    int playerHeight = 50;

    float speed = 300.0f;

    // Target
    int targetSize = 30;

    SDL_Rect target;

    target.x = rand() % (800 - targetSize);
    target.y = rand() % (600 - targetSize);
    target.w = targetSize;
    target.h = targetSize;

    // Score
    int score = 0;

    // Timer
    float targetTimer = 5.0f;

    // Game state
    int running = 1;
    int gameOver = 0;

    // Timing
    Uint32 lastTime = SDL_GetTicks();

    // Game loop
    while (running)
    {
        // Calculate delta time
        Uint32 currentTime = SDL_GetTicks();

        float deltaTime =
            (currentTime - lastTime) / 1000.0f;

        lastTime = currentTime;

        // Events
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }

            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    running = 0;
                }

                // Restart after game over
                if (gameOver &&
                    event.key.keysym.sym == SDLK_r)
                {
                    playerX = 375.0f;
                    playerY = 275.0f;

                    score = 0;

                    targetTimer = 5.0f;

                    target.x = rand() % (800 - targetSize);
                    target.y = rand() % (600 - targetSize);

                    gameOver = 0;
                }
            }
        }

        // Only update game while playing
        if (!gameOver)
        {
            // Keyboard input
            const Uint8 *keyboard =
                SDL_GetKeyboardState(NULL);

            // Move player
            if (keyboard[SDL_SCANCODE_LEFT] ||
                keyboard[SDL_SCANCODE_A])
            {
                playerX -= speed * deltaTime;
            }

            if (keyboard[SDL_SCANCODE_RIGHT] ||
                keyboard[SDL_SCANCODE_D])
            {
                playerX += speed * deltaTime;
            }

            if (keyboard[SDL_SCANCODE_UP] ||
                keyboard[SDL_SCANCODE_W])
            {
                playerY -= speed * deltaTime;
            }

            if (keyboard[SDL_SCANCODE_DOWN] ||
                keyboard[SDL_SCANCODE_S])
            {
                playerY += speed * deltaTime;
            }

            // Keep player inside window
            if (playerX < 0)
                playerX = 0;

            if (playerX > 800 - playerWidth)
                playerX = 800 - playerWidth;

            if (playerY < 0)
                playerY = 0;

            if (playerY > 600 - playerHeight)
                playerY = 600 - playerHeight;

            // Convert player position to rectangle
            SDL_Rect player = {
                (int)playerX,
                (int)playerY,
                playerWidth,
                playerHeight
            };

            // Countdown timer
            targetTimer -= deltaTime;

            // Time ran out
            if (targetTimer <= 0)
            {
                gameOver = 1;

                printf("GAME OVER!\n");
                printf("Final Score: %d\n", score);
                printf("Press R to restart.\n");
            }

            // Collision detection
            if (SDL_HasIntersection(&player, &target))
            {
                score += 10;

                printf("Target hit! Score: %d\n", score);

                // Generate new target
                target.x = rand() % (800 - targetSize);
                target.y = rand() % (600 - targetSize);

                // Reset timer
                targetTimer = 5.0f;
            }
        }

        // -------------------------
        // RENDER
        // -------------------------

        // Background
        SDL_SetRenderDrawColor(
            renderer,
            20,
            20,
            20,
            255
        );

        SDL_RenderClear(renderer);

        // Player rectangle
        SDL_Rect player = {
            (int)playerX,
            (int)playerY,
            playerWidth,
            playerHeight
        };

        // Draw player WHITE
        SDL_SetRenderDrawColor(
            renderer,
            255,
            255,
            255,
            255
        );

        SDL_RenderFillRect(renderer, &player);

        // Draw target RED
        if (!gameOver)
        {
            SDL_SetRenderDrawColor(
                renderer,
                255,
                0,
                0,
                255
            );

            SDL_RenderFillRect(renderer, &target);
        }

        // Present everything
        SDL_RenderPresent(renderer);

        // Small delay to avoid unnecessary CPU usage
        SDL_Delay(1);
    }

    // Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}