#include "renderer.h"
#include "config.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>

/* ============================================================
   TEXT HELPER
   ============================================================ */

static SDL_Texture* createTextTexture(
    SDL_Renderer* renderer,
    TTF_Font* font,
    const char* text,
    SDL_Color color,
    int* width,
    int* height
)
{
    SDL_Surface* surface =
        TTF_RenderText_Blended(
            font,
            text,
            color
        );

    if (surface == NULL)
    {
        fprintf(
            stderr,
            "TTF_RenderText_Blended failed: %s\n",
            TTF_GetError()
        );

        return NULL;
    }

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            renderer,
            surface
        );

    if (texture == NULL)
    {
        fprintf(
            stderr,
            "SDL_CreateTextureFromSurface failed: %s\n",
            SDL_GetError()
        );

        SDL_FreeSurface(surface);

        return NULL;
    }

    *width = surface->w;
    *height = surface->h;

    SDL_FreeSurface(surface);

    return texture;
}


/* ============================================================
   INITIALIZATION
   ============================================================ */

int renderer_init(Renderer* renderer)
{
    renderer->window = NULL;
    renderer->renderer = NULL;
    renderer->gameTexture = NULL;
    renderer->font = NULL;

    /* SDL */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(
            stderr,
            "SDL_Init failed: %s\n",
            SDL_GetError()
        );

        return 0;
    }

    /* SDL_ttf */
    if (TTF_Init() != 0)
    {
        fprintf(
            stderr,
            "TTF_Init failed: %s\n",
            TTF_GetError()
        );

        SDL_Quit();

        return 0;
    }

    /* Window */
    renderer->window =
        SDL_CreateWindow(
            "Game From Scratch",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_RESIZABLE
        );

    if (renderer->window == NULL)
    {
        fprintf(
            stderr,
            "SDL_CreateWindow failed: %s\n",
            SDL_GetError()
        );

        TTF_Quit();
        SDL_Quit();

        return 0;
    }

    SDL_SetWindowMinimumSize(
        renderer->window,
        MIN_WINDOW_WIDTH,
        MIN_WINDOW_HEIGHT
    );

    /* Renderer */
    renderer->renderer =
        SDL_CreateRenderer(
            renderer->window,
            -1,
            SDL_RENDERER_ACCELERATED |
            SDL_RENDERER_PRESENTVSYNC |
            SDL_RENDERER_TARGETTEXTURE
        );

    if (renderer->renderer == NULL)
    {
        fprintf(
            stderr,
            "SDL_CreateRenderer failed: %s\n",
            SDL_GetError()
        );

        SDL_DestroyWindow(renderer->window);

        renderer->window = NULL;

        TTF_Quit();
        SDL_Quit();

        return 0;
    }

    /* Fixed 800x600 game render target */
    renderer->gameTexture =
        SDL_CreateTexture(
            renderer->renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            GAME_WIDTH,
            GAME_HEIGHT
        );

    if (renderer->gameTexture == NULL)
    {
        fprintf(
            stderr,
            "SDL_CreateTexture failed: %s\n",
            SDL_GetError()
        );

        SDL_DestroyRenderer(renderer->renderer);
        SDL_DestroyWindow(renderer->window);

        renderer->renderer = NULL;
        renderer->window = NULL;

        TTF_Quit();
        SDL_Quit();

        return 0;
    }

    /* Project-local font */
    renderer->font =
        TTF_OpenFont(
            "assets/fonts/DejaVuSans.ttf",
            24
        );

    if (renderer->font == NULL)
    {
        fprintf(
            stderr,
            "Could not load assets/fonts/DejaVuSans.ttf\n"
            "TTF error: %s\n",
            TTF_GetError()
        );

        SDL_DestroyTexture(renderer->gameTexture);
        SDL_DestroyRenderer(renderer->renderer);
        SDL_DestroyWindow(renderer->window);

        renderer->gameTexture = NULL;
        renderer->renderer = NULL;
        renderer->window = NULL;

        TTF_Quit();
        SDL_Quit();

        return 0;
    }

    return 1;
}


/* ============================================================
   RENDER
   ============================================================ */

void renderer_render(
    Renderer* renderer,
    const Game* game
)
{
    SDL_Renderer* sdlRenderer =
        renderer->renderer;

    /* --------------------------------------------------------
       Draw game into 800x600 texture
       -------------------------------------------------------- */

    SDL_SetRenderTarget(
        sdlRenderer,
        renderer->gameTexture
    );

    /* Game background */
    SDL_SetRenderDrawColor(
        sdlRenderer,
        GAME_BG_R,
        GAME_BG_G,
        GAME_BG_B,
        255
    );

    SDL_RenderClear(sdlRenderer);

    /* Target */
    if (!game->gameOver)
    {
        SDL_Rect targetRect =
        {
            (int)game->target.x,
            (int)game->target.y,
            TARGET_SIZE,
            TARGET_SIZE
        };

        SDL_SetRenderDrawColor(
            sdlRenderer,
            TARGET_R,
            TARGET_G,
            TARGET_B,
            255
        );

        SDL_RenderFillRect(
            sdlRenderer,
            &targetRect
        );
    }

    /* Player */
    SDL_Rect playerRect =
    {
        (int)game->player.x,
        (int)game->player.y,
        PLAYER_SIZE,
        PLAYER_SIZE
    };

    SDL_SetRenderDrawColor(
        sdlRenderer,
        PLAYER_R,
        PLAYER_G,
        PLAYER_B,
        255
    );

    SDL_RenderFillRect(
        sdlRenderer,
        &playerRect
    );

    /* Game border */
    SDL_Rect borderRect =
    {
        0,
        0,
        GAME_WIDTH,
        GAME_HEIGHT
    };

    SDL_SetRenderDrawColor(
        sdlRenderer,
        BORDER_R,
        BORDER_G,
        BORDER_B,
        255
    );

    SDL_RenderDrawRect(
        sdlRenderer,
        &borderRect
    );


    /* --------------------------------------------------------
       Switch back to physical window
       -------------------------------------------------------- */

    SDL_SetRenderTarget(
        sdlRenderer,
        NULL
    );

    int windowWidth;
    int windowHeight;

    SDL_GetWindowSize(
        renderer->window,
        &windowWidth,
        &windowHeight
    );


    /* --------------------------------------------------------
       Calculate area available for game
       -------------------------------------------------------- */

    int availableGameHeight =
        windowHeight - UI_HEIGHT;

    if (availableGameHeight < GAME_HEIGHT)
    {
        availableGameHeight = GAME_HEIGHT;
    }


    /* --------------------------------------------------------
       Clear entire physical window
       -------------------------------------------------------- */

    SDL_SetRenderDrawColor(
        sdlRenderer,
        OUTSIDE_R,
        OUTSIDE_G,
        OUTSIDE_B,
        255
    );

    SDL_RenderClear(sdlRenderer);


    /* --------------------------------------------------------
       Preserve 4:3 aspect ratio
       -------------------------------------------------------- */

    float scaleX =
        (float)windowWidth /
        (float)GAME_WIDTH;

    float scaleY =
        (float)availableGameHeight /
        (float)GAME_HEIGHT;

    float scale =
        (scaleX < scaleY)
        ? scaleX
        : scaleY;

    int renderWidth =
        (int)(GAME_WIDTH * scale);

    int renderHeight =
        (int)(GAME_HEIGHT * scale);

    int renderX =
        (windowWidth - renderWidth) / 2;

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


    /* --------------------------------------------------------
       Copy game texture to physical window
       -------------------------------------------------------- */

    SDL_RenderCopy(
        sdlRenderer,
        renderer->gameTexture,
        NULL,
        &destinationRect
    );


    /* ========================================================
       HUD
       ======================================================== */

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


    /* --------------------------------------------------------
       Title
       -------------------------------------------------------- */

    int titleWidth;
    int titleHeight;

    SDL_Texture* titleTexture =
        createTextTexture(
            sdlRenderer,
            renderer->font,
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
            4,
            titleWidth,
            titleHeight
        };

        SDL_RenderCopy(
            sdlRenderer,
            titleTexture,
            NULL,
            &titleRect
        );

        SDL_DestroyTexture(titleTexture);
    }


    /* --------------------------------------------------------
       Score
       -------------------------------------------------------- */

    char scoreText[64];

    snprintf(
        scoreText,
        sizeof(scoreText),
        "Score: %d",
        game->score
    );

    int scoreWidth;
    int scoreHeight;

    SDL_Texture* scoreTexture =
        createTextTexture(
            sdlRenderer,
            renderer->font,
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
            34,
            scoreWidth,
            scoreHeight
        };

        SDL_RenderCopy(
            sdlRenderer,
            scoreTexture,
            NULL,
            &scoreRect
        );

        SDL_DestroyTexture(scoreTexture);
    }


    /* --------------------------------------------------------
       Timer text
       -------------------------------------------------------- */

    float displayTimer =
        game->targetTimer;

    if (displayTimer < 0.0f)
    {
        displayTimer = 0.0f;
    }

    char timerText[64];

    snprintf(
        timerText,
        sizeof(timerText),
        "Time: %.1f",
        displayTimer
    );

    int timerWidth;
    int timerHeight;

    SDL_Texture* timerTexture =
        createTextTexture(
            sdlRenderer,
            renderer->font,
            timerText,
            displayTimer <= 2.0f
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
            34,
            timerWidth,
            timerHeight
        };

        SDL_RenderCopy(
            sdlRenderer,
            timerTexture,
            NULL,
            &timerRect
        );

        SDL_DestroyTexture(timerTexture);
    }


    /* --------------------------------------------------------
       Timer bar
       -------------------------------------------------------- */

    int barX = 20;
    int barY = 62;
    int barWidth = windowWidth - 40;
    int barHeight = 10;

    /* Background */
    SDL_SetRenderDrawColor(
        sdlRenderer,
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
        sdlRenderer,
        &timerBarBackground
    );

    /* Progress */
    float progress =
        displayTimer / TARGET_TIME;

    if (progress < 0.0f)
    {
        progress = 0.0f;
    }

    if (progress > 1.0f)
    {
        progress = 1.0f;
    }

    SDL_SetRenderDrawColor(
        sdlRenderer,
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
        sdlRenderer,
        &timerBar
    );


    /* --------------------------------------------------------
       Status
       -------------------------------------------------------- */

    const char* statusText;

    SDL_Color statusColor;

    if (game->gameOver)
    {
        statusText =
            "GAME OVER - Press R to restart";

        statusColor = yellow;
    }
    else
    {
        statusText =
            "WASD / Arrow Keys to move";

        statusColor = gray;
    }

    int statusWidth;
    int statusHeight;

    SDL_Texture* statusTexture =
        createTextTexture(
            sdlRenderer,
            renderer->font,
            statusText,
            statusColor,
            &statusWidth,
            &statusHeight
        );

    if (statusTexture != NULL)
    {
        SDL_Rect statusRect =
        {
            (windowWidth - statusWidth) / 2,
            88,
            statusWidth,
            statusHeight
        };

        if (statusRect.y + statusRect.h <= UI_HEIGHT)
        {
            SDL_RenderCopy(
                sdlRenderer,
                statusTexture,
                NULL,
                &statusRect
            );
        }

        SDL_DestroyTexture(statusTexture);
    }


    /* --------------------------------------------------------
       Present
       -------------------------------------------------------- */

    SDL_RenderPresent(sdlRenderer);
}


/* ============================================================
   CLEANUP
   ============================================================ */

void renderer_cleanup(Renderer* renderer)
{
    if (renderer->font != NULL)
    {
        TTF_CloseFont(renderer->font);
        renderer->font = NULL;
    }

    if (renderer->gameTexture != NULL)
    {
        SDL_DestroyTexture(
            renderer->gameTexture
        );

        renderer->gameTexture = NULL;
    }

    if (renderer->renderer != NULL)
    {
        SDL_DestroyRenderer(
            renderer->renderer
        );

        renderer->renderer = NULL;
    }

    if (renderer->window != NULL)
    {
        SDL_DestroyWindow(
            renderer->window
        );

        renderer->window = NULL;
    }

    TTF_Quit();
    SDL_Quit();
}