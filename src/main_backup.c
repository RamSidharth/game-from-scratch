#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* ============================================================
   GAME CONFIGURATION
   ============================================================ */

#define GAME_WIDTH 800
#define GAME_HEIGHT 600

#define UI_HEIGHT 120

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT (GAME_HEIGHT + UI_HEIGHT)

#define MIN_WINDOW_WIDTH 800
#define MIN_WINDOW_HEIGHT 720

#define PLAYER_SIZE 50
#define TARGET_SIZE 30

#define PLAYER_SPEED 300.0f
#define TARGET_TIME 5.0f

/* ============================================================
   COLORS
   ============================================================ */

/*
   Color of the unused area around the game.

   Dark gray gives a strong contrast with the game area.
*/
#define OUTSIDE_R 35
#define OUTSIDE_G 35
#define OUTSIDE_B 35

/* Game background */
#define GAME_BG_R 20
#define GAME_BG_G 20
#define GAME_BG_B 25

/* Player */
#define PLAYER_R 240
#define PLAYER_G 240
#define PLAYER_B 240

/* Target */
#define TARGET_R 220
#define TARGET_G 50
#define TARGET_B 50

/* Border */
#define BORDER_R 100
#define BORDER_G 100
#define BORDER_B 100

/* ============================================================
   HELPER FUNCTIONS
   ============================================================ */

SDL_Texture* createTextTexture(
    SDL_Renderer* renderer,
    TTF_Font* font,
    const char* text,
    SDL_Color color,
    int* width,
    int* height
)
{
    SDL_Surface* surface = TTF_RenderText_Blended(
        font,
        text,
        color
    );

    if (surface == NULL)
    {
        fprintf(stderr, "TTF_RenderText_Blended failed: %s\n",
                TTF_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(
        renderer,
        surface
    );

    if (texture == NULL)
    {
        fprintf(stderr, "SDL_CreateTextureFromSurface failed: %s\n",
                SDL_GetError());

        SDL_FreeSurface(surface);
        return NULL;
    }

    *width = surface->w;
    *height = surface->h;

    SDL_FreeSurface(surface);

    return texture;
}


/* ============================================================
   RESET GAME
   ============================================================ */

void resetGame(
    float* playerX,
    float* playerY,
    float* targetX,
    float* targetY,
    int* score,
    float* targetTimer,
    int* gameOver
)
{
    *playerX = (GAME_WIDTH - PLAYER_SIZE) / 2.0f;
    *playerY = (GAME_HEIGHT - PLAYER_SIZE) / 2.0f;

    *targetX =
        (float)(rand() % (GAME_WIDTH - TARGET_SIZE));

    *targetY =
        (float)(rand() % (GAME_HEIGHT - TARGET_SIZE));

    *score = 0;

    *targetTimer = TARGET_TIME;

    *gameOver = 0;
}


/* ============================================================
   MAIN
   ============================================================ */

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    /* --------------------------------------------------------
       SDL INITIALIZATION
       -------------------------------------------------------- */

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr,
                "SDL_Init failed: %s\n",
                SDL_GetError());

        return 1;
    }


    if (TTF_Init() != 0)
    {
        fprintf(stderr,
                "TTF_Init failed: %s\n",
                TTF_GetError());

        SDL_Quit();

        return 1;
    }


    /* --------------------------------------------------------
       CREATE WINDOW
       -------------------------------------------------------- */

    SDL_Window* window = SDL_CreateWindow(
        "Game From Scratch",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );

    if (window == NULL)
    {
        fprintf(stderr,
                "SDL_CreateWindow failed: %s\n",
                SDL_GetError());

        TTF_Quit();
        SDL_Quit();

        return 1;
    }


    SDL_SetWindowMinimumSize(
        window,
        MIN_WINDOW_WIDTH,
        MIN_WINDOW_HEIGHT
    );


    /* --------------------------------------------------------
       CREATE RENDERER
       -------------------------------------------------------- */

    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED |
        SDL_RENDERER_PRESENTVSYNC |
        SDL_RENDERER_TARGETTEXTURE
    );

    if (renderer == NULL)
    {
        fprintf(stderr,
                "SDL_CreateRenderer failed: %s\n",
                SDL_GetError());

        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();

        return 1;
    }


    /* --------------------------------------------------------
       CREATE FIXED-SIZE GAME TEXTURE
       -------------------------------------------------------- */

    SDL_Texture* gameTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        GAME_WIDTH,
        GAME_HEIGHT
    );

    if (gameTexture == NULL)
    {
        fprintf(stderr,
                "SDL_CreateTexture failed: %s\n",
                SDL_GetError());

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        TTF_Quit();
        SDL_Quit();

        return 1;
    }


    /* --------------------------------------------------------
       LOAD FONT
       -------------------------------------------------------- */

    TTF_Font* font = NULL;

    /*
       First try the font inside the project.

       Recommended location:

       assets/fonts/DejaVuSans.ttf
    */

    font = TTF_OpenFont(
        "assets/fonts/DejaVuSans.ttf",
        24
    );


    /*
       If that doesn't exist, try the MSYS2 installation.
    */

    if (font == NULL)
    {
        font = TTF_OpenFont(
            "C:/msys64/ucrt64/share/fonts/TTF/DejaVuSans.ttf",
            24
        );
    }


    if (font == NULL)
    {
        fprintf(stderr,
                "Could not load DejaVuSans.ttf\n"
                "TTF error: %s\n",
                TTF_GetError());

        SDL_DestroyTexture(gameTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        TTF_Quit();
        SDL_Quit();

        return 1;
    }


    /* --------------------------------------------------------
       RANDOM SEED
       -------------------------------------------------------- */

    srand((unsigned int)time(NULL));


    /* --------------------------------------------------------
       GAME VARIABLES
       -------------------------------------------------------- */

    float playerX;
    float playerY;

    float targetX;
    float targetY;

    int score;

    float targetTimer;

    int gameOver;

    resetGame(
        &playerX,
        &playerY,
        &targetX,
        &targetY,
        &score,
        &targetTimer,
        &gameOver
    );


    /* --------------------------------------------------------
       GAME LOOP VARIABLES
       -------------------------------------------------------- */

    int running = 1;

    Uint64 previousCounter = SDL_GetPerformanceCounter();

    const double performanceFrequency =
        (double)SDL_GetPerformanceFrequency();


    /* ========================================================
       MAIN GAME LOOP
       ======================================================== */

    while (running)
    {
        /* ----------------------------------------------------
           DELTA TIME
           ---------------------------------------------------- */

        Uint64 currentCounter =
            SDL_GetPerformanceCounter();

        float deltaTime =
            (float)((currentCounter - previousCounter)
                    / performanceFrequency);

        previousCounter = currentCounter;


        /*
           Prevent huge movement if the program is paused
           or the window is dragged.
        */

        if (deltaTime > 0.1f)
        {
            deltaTime = 0.1f;
        }


        /* ----------------------------------------------------
           EVENT HANDLING
           ---------------------------------------------------- */

        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }


            if (event.type == SDL_KEYDOWN)
            {
                SDL_Keycode key = event.key.keysym.sym;


                /* ESC = quit */

                if (key == SDLK_ESCAPE)
                {
                    running = 0;
                }


                /* F11 = fullscreen */

                if (key == SDLK_F11)
                {
                    Uint32 flags =
                        SDL_GetWindowFlags(window);

                    if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP)
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


                /* R = restart */

                if (key == SDLK_r && gameOver)
                {
                    resetGame(
                        &playerX,
                        &playerY,
                        &targetX,
                        &targetY,
                        &score,
                        &targetTimer,
                        &gameOver
                    );
                }
            }
        }


        /* ----------------------------------------------------
           KEYBOARD INPUT
           ---------------------------------------------------- */

        const Uint8* keyboard =
            SDL_GetKeyboardState(NULL);


        if (!gameOver)
        {
            float moveX = 0.0f;
            float moveY = 0.0f;


            /*
               A / LEFT
            */

            if (keyboard[SDL_SCANCODE_A] ||
                keyboard[SDL_SCANCODE_LEFT])
            {
                moveX -= 1.0f;
            }


            /*
               D / RIGHT
            */

            if (keyboard[SDL_SCANCODE_D] ||
                keyboard[SDL_SCANCODE_RIGHT])
            {
                moveX += 1.0f;
            }


            /*
               W / UP
            */

            if (keyboard[SDL_SCANCODE_W] ||
                keyboard[SDL_SCANCODE_UP])
            {
                moveY -= 1.0f;
            }


            /*
               S / DOWN
            */

            if (keyboard[SDL_SCANCODE_S] ||
                keyboard[SDL_SCANCODE_DOWN])
            {
                moveY += 1.0f;
            }


            /*
               Normalize diagonal movement.

               Without this:

               W + D

               would move faster than just W.
            */

            if (moveX != 0.0f ||
                moveY != 0.0f)
            {
                float length =
                    sqrtf(
                        moveX * moveX +
                        moveY * moveY
                    );

                moveX /= length;
                moveY /= length;
            }


            /* ------------------------------------------------
               MOVE PLAYER
               ------------------------------------------------ */

            playerX +=
                moveX * PLAYER_SPEED * deltaTime;

            playerY +=
                moveY * PLAYER_SPEED * deltaTime;


            /* ------------------------------------------------
               PLAYER BOUNDARIES
               ------------------------------------------------ */

            if (playerX < 0)
            {
                playerX = 0;
            }


            if (playerY < 0)
            {
                playerY = 0;
            }


            if (playerX >
                GAME_WIDTH - PLAYER_SIZE)
            {
                playerX =
                    GAME_WIDTH - PLAYER_SIZE;
            }


            if (playerY >
                GAME_HEIGHT - PLAYER_SIZE)
            {
                playerY =
                    GAME_HEIGHT - PLAYER_SIZE;
            }


            /* ------------------------------------------------
               TARGET TIMER
               ------------------------------------------------ */

            targetTimer -= deltaTime;


            if (targetTimer <= 0.0f)
            {
                gameOver = 1;
            }


            /* ------------------------------------------------
               COLLISION
               ------------------------------------------------ */

            SDL_Rect playerRect =
            {
                (int)playerX,
                (int)playerY,
                PLAYER_SIZE,
                PLAYER_SIZE
            };


            SDL_Rect targetRect =
            {
                (int)targetX,
                (int)targetY,
                TARGET_SIZE,
                TARGET_SIZE
            };


            if (SDL_HasIntersection(
                    &playerRect,
                    &targetRect))
            {
                score += 10;


                targetX =
                    (float)(
                        rand() %
                        (GAME_WIDTH - TARGET_SIZE)
                    );


                targetY =
                    (float)(
                        rand() %
                        (GAME_HEIGHT - TARGET_SIZE)
                    );


                targetTimer = TARGET_TIME;
            }
        }


        /* ====================================================
           RENDER GAME TO GAME TEXTURE
           ==================================================== */

        SDL_SetRenderTarget(
            renderer,
            gameTexture
        );


        /*
           Game background
        */

        SDL_SetRenderDrawColor(
            renderer,
            GAME_BG_R,
            GAME_BG_G,
            GAME_BG_B,
            255
        );

        SDL_RenderClear(renderer);


        /* ----------------------------------------------------
           DRAW TARGET
           ---------------------------------------------------- */

        if (!gameOver)
        {
            SDL_Rect targetRect =
            {
                (int)targetX,
                (int)targetY,
                TARGET_SIZE,
                TARGET_SIZE
            };


            SDL_SetRenderDrawColor(
                renderer,
                TARGET_R,
                TARGET_G,
                TARGET_B,
                255
            );

            SDL_RenderFillRect(
                renderer,
                &targetRect
            );
        }


        /* ----------------------------------------------------
           DRAW PLAYER
           ---------------------------------------------------- */

        SDL_Rect playerRect =
        {
            (int)playerX,
            (int)playerY,
            PLAYER_SIZE,
            PLAYER_SIZE
        };


        SDL_SetRenderDrawColor(
            renderer,
            PLAYER_R,
            PLAYER_G,
            PLAYER_B,
            255
        );

        SDL_RenderFillRect(
            renderer,
            &playerRect
        );


        /* ----------------------------------------------------
           GAME BORDER
           ---------------------------------------------------- */

        SDL_Rect borderRect =
        {
            0,
            0,
            GAME_WIDTH,
            GAME_HEIGHT
        };


        SDL_SetRenderDrawColor(
            renderer,
            BORDER_R,
            BORDER_G,
            BORDER_B,
            255
        );

        SDL_RenderDrawRect(
            renderer,
            &borderRect
        );


        /* ====================================================
           SWITCH BACK TO ACTUAL WINDOW
           ==================================================== */

        SDL_SetRenderTarget(
            renderer,
            NULL
        );


        /* ====================================================
           GET WINDOW SIZE
           ==================================================== */

        int windowWidth;
        int windowHeight;

        SDL_GetWindowSize(
            window,
            &windowWidth,
            &windowHeight
        );


        /*
           Make sure the UI area exists.
        */

        int availableGameHeight =
            windowHeight - UI_HEIGHT;

        if (availableGameHeight < GAME_HEIGHT)
        {
            availableGameHeight = GAME_HEIGHT;
        }


        /* ====================================================
           CLEAR PHYSICAL WINDOW
           ==================================================== */

        /*
           THIS is the unused-area color.

           Dark gray.
        */

        SDL_SetRenderDrawColor(
            renderer,
            OUTSIDE_R,
            OUTSIDE_G,
            OUTSIDE_B,
            255
        );

        SDL_RenderClear(renderer);


        /* ====================================================
           CALCULATE PROPORTIONAL GAME SIZE
           ==================================================== */

        float scaleX =
            (float)windowWidth /
            (float)GAME_WIDTH;

        float scaleY =
            (float)availableGameHeight /
            (float)GAME_HEIGHT;


        /*
           Use the smaller scale.

           This keeps the original 4:3
           aspect ratio.
        */

        float scale =
            (scaleX < scaleY)
            ? scaleX
            : scaleY;


        int renderWidth =
            (int)(GAME_WIDTH * scale);

        int renderHeight =
            (int)(GAME_HEIGHT * scale);


        /*
           Center horizontally.
        */

        int renderX =
            (windowWidth - renderWidth) / 2;


        /*
           Center vertically inside the
           area below the UI.
        */

        int renderY =
            UI_HEIGHT +
            (availableGameHeight - renderHeight) / 2;


        SDL_Rect destinationRect =
        {
            renderX,
            renderY,
            renderWidth,
            renderHeight
        };


        /* ====================================================
           COPY GAME TEXTURE TO WINDOW
           ==================================================== */

        SDL_RenderCopy(
            renderer,
            gameTexture,
            NULL,
            &destinationRect
        );


        /* ====================================================
           HUD
           ==================================================== */

        SDL_Color white =
        {
            255,
            255,
            255,
            255
        };


        SDL_Color yellow =
        {
            255,
            220,
            80,
            255
        };


        SDL_Color red =
        {
            255,
            90,
            90,
            255
        };


        SDL_Color gray =
        {
            190,
            190,
            190,
            255
        };


        /* ----------------------------------------------------
           TITLE
           ---------------------------------------------------- */

        int titleWidth;
        int titleHeight;

        SDL_Texture* titleTexture =
            createTextTexture(
                renderer,
                font,
                "GAME FROM SCRATCH",
                white,
                &titleWidth,
                &titleHeight
            );


        if (titleTexture != NULL)
        {
            SDL_Rect titleRect =
            {
                (windowWidth - titleWidth) / 2,
                8,
                titleWidth,
                titleHeight
            };


            SDL_RenderCopy(
                renderer,
                titleTexture,
                NULL,
                &titleRect
            );


            SDL_DestroyTexture(titleTexture);
        }


        /* ----------------------------------------------------
           SCORE
           ---------------------------------------------------- */

        char scoreText[64];

        snprintf(
            scoreText,
            sizeof(scoreText),
            "Score: %d",
            score
        );


        int scoreWidth;
        int scoreHeight;

        SDL_Texture* scoreTexture =
            createTextTexture(
                renderer,
                font,
                scoreText,
                white,
                &scoreWidth,
                &scoreHeight
            );


        if (scoreTexture != NULL)
        {
            SDL_Rect scoreRect =
            {
                20,
                45,
                scoreWidth,
                scoreHeight
            };


            SDL_RenderCopy(
                renderer,
                scoreTexture,
                NULL,
                &scoreRect
            );


            SDL_DestroyTexture(scoreTexture);
        }


        /* ----------------------------------------------------
           TIMER TEXT
           ---------------------------------------------------- */

        char timerText[64];

        if (targetTimer < 0)
        {
            targetTimer = 0;
        }


        snprintf(
            timerText,
            sizeof(timerText),
            "Time: %.1f",
            targetTimer
        );


        int timerWidth;
        int timerHeight;

        SDL_Texture* timerTexture =
            createTextTexture(
                renderer,
                font,
                timerText,
                targetTimer <= 2.0f
                    ? red
                    : white,
                &timerWidth,
                &timerHeight
            );


        if (timerTexture != NULL)
        {
            SDL_Rect timerRect =
            {
                windowWidth -
                timerWidth -
                20,
                45,
                timerWidth,
                timerHeight
            };


            SDL_RenderCopy(
                renderer,
                timerTexture,
                NULL,
                &timerRect
            );


            SDL_DestroyTexture(timerTexture);
        }


        /* ----------------------------------------------------
           TIMER BAR
           ---------------------------------------------------- */

        int barX = 20;
        int barY = 85;
        int barWidth = windowWidth - 40;
        int barHeight = 10;


        /*
           Background
        */

        SDL_SetRenderDrawColor(
            renderer,
            70,
            70,
            70,
            255
        );

        SDL_Rect timerBarBackground =
        {
            barX,
            barY,
            barWidth,
            barHeight
        };


        SDL_RenderFillRect(
            renderer,
            &timerBarBackground
        );


        /*
           Progress
        */

        float progress =
            targetTimer / TARGET_TIME;


        if (progress < 0)
        {
            progress = 0;
        }

        if (progress > 1)
        {
            progress = 1;
        }


        SDL_SetRenderDrawColor(
            renderer,
            80,
            200,
            100,
            255
        );


        SDL_Rect timerBar =
        {
            barX,
            barY,
            (int)(barWidth * progress),
            barHeight
        };


        SDL_RenderFillRect(
            renderer,
            &timerBar
        );


        /* ----------------------------------------------------
           STATUS / RESTART MESSAGE
           ---------------------------------------------------- */

        const char* statusText;


        if (gameOver)
        {
            statusText =
                "GAME OVER - Press R to restart";
        }
        else
        {
            statusText =
                "WASD / Arrow Keys to move";
        }


        int statusWidth;
        int statusHeight;


        SDL_Texture* statusTexture =
            createTextTexture(
                renderer,
                font,
                statusText,
                gameOver ? yellow : gray,
                &statusWidth,
                &statusHeight
            );


        if (statusTexture != NULL)
        {
            SDL_Rect statusRect =
            {
                (windowWidth - statusWidth) / 2,
                100,
                statusWidth,
                statusHeight
            };


            /*
               Only draw the status text if
               there is enough UI space.
            */

            if (statusRect.y + statusRect.h <= UI_HEIGHT)
            {
                SDL_RenderCopy(
                    renderer,
                    statusTexture,
                    NULL,
                    &statusRect
                );
            }


            SDL_DestroyTexture(statusTexture);
        }


        /* ====================================================
           PRESENT
           ==================================================== */

        SDL_RenderPresent(renderer);
    }


    /* ========================================================
       CLEANUP
       ======================================================== */

    TTF_CloseFont(font);

    SDL_DestroyTexture(gameTexture);

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    TTF_Quit();

    SDL_Quit();

    return 0;
}