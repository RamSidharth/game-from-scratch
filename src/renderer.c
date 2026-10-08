#include "renderer.h"
#include "config.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>

/* ============================================================
   MENU CONSTANTS
   ============================================================ */

#define MENU_BUTTON_WIDTH  280
#define MENU_BUTTON_HEIGHT 64

#define MENU_BUTTON_Y 260

#define MENU_BUTTON_BORDER 3

#define MENU_BUTTON_NORMAL_R 45
#define MENU_BUTTON_NORMAL_G 45
#define MENU_BUTTON_NORMAL_B 55

#define MENU_BUTTON_HOVER_R 80
#define MENU_BUTTON_HOVER_G 80
#define MENU_BUTTON_HOVER_B 100


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
   CENTERED TEXT
   ============================================================ */

static void drawCenteredText(
    SDL_Renderer* renderer,
    TTF_Font* font,
    const char* text,
    SDL_Color color,
    int windowWidth,
    int y
)
{
    int width;
    int height;

    SDL_Texture* texture =
        createTextTexture(
            renderer,
            font,
            text,
            color,
            &width,
            &height
        );

    if (texture != NULL)
    {
        SDL_Rect rect =
        {
            (windowWidth - width) / 2,
            y,
            width,
            height
        };

        SDL_RenderCopy(
            renderer,
            texture,
            NULL,
            &rect
        );

        SDL_DestroyTexture(texture);
    }
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

    /*
        SDL
    */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(
            stderr,
            "SDL_Init failed: %s\n",
            SDL_GetError()
        );

        return 0;
    }

    /*
        SDL_ttf
    */
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

    /*
        Window
    */
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

    /*
        Renderer
    */
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

        SDL_DestroyWindow(
            renderer->window
        );

        renderer->window = NULL;

        TTF_Quit();
        SDL_Quit();

        return 0;
    }

    /*
        Fixed 800x600 game texture
    */
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

        SDL_DestroyRenderer(
            renderer->renderer
        );

        SDL_DestroyWindow(
            renderer->window
        );

        renderer->renderer = NULL;
        renderer->window = NULL;

        TTF_Quit();
        SDL_Quit();

        return 0;
    }

    /*
        Font
    */
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

        SDL_DestroyTexture(
            renderer->gameTexture
        );

        SDL_DestroyRenderer(
            renderer->renderer
        );

        SDL_DestroyWindow(
            renderer->window
        );

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
   MENU BUTTON RECTANGLE
   ============================================================ */

static SDL_Rect getMenuStartButtonRect(
    int windowWidth
)
{
    SDL_Rect rect =
    {
        (windowWidth - MENU_BUTTON_WIDTH) / 2,
        MENU_BUTTON_Y,
        MENU_BUTTON_WIDTH,
        MENU_BUTTON_HEIGHT
    };

    return rect;
}


/* ============================================================
   MENU BUTTON HIT TEST
   ============================================================ */

int renderer_menu_start_button_contains(
    Renderer* renderer,
    int mouseX,
    int mouseY
)
{
    int windowWidth;
    int windowHeight;

    SDL_GetWindowSize(
        renderer->window,
        &windowWidth,
        &windowHeight
    );

    (void)windowHeight;

    SDL_Rect buttonRect =
        getMenuStartButtonRect(
            windowWidth
        );

    return SDL_PointInRect(
        &(SDL_Point)
        {
            mouseX,
            mouseY
        },
        &buttonRect
    );
}


/* ============================================================
   MENU BUTTON
   ============================================================ */

static void renderMenuButton(
    Renderer* renderer,
    int windowWidth,
    int mouseX,
    int mouseY
)
{
    SDL_Renderer* sdlRenderer =
        renderer->renderer;

    SDL_Rect buttonRect =
        getMenuStartButtonRect(
            windowWidth
        );

    int hovered =
        SDL_PointInRect(
            &(SDL_Point)
            {
                mouseX,
                mouseY
            },
            &buttonRect
        );

    /*
        Button background
    */
    if (hovered)
    {
        SDL_SetRenderDrawColor(
            sdlRenderer,
            MENU_BUTTON_HOVER_R,
            MENU_BUTTON_HOVER_G,
            MENU_BUTTON_HOVER_B,
            255
        );
    }
    else
    {
        SDL_SetRenderDrawColor(
            sdlRenderer,
            MENU_BUTTON_NORMAL_R,
            MENU_BUTTON_NORMAL_G,
            MENU_BUTTON_NORMAL_B,
            255
        );
    }

    SDL_RenderFillRect(
        sdlRenderer,
        &buttonRect
    );

    /*
        Button border
    */
    if (hovered)
    {
        SDL_SetRenderDrawColor(
            sdlRenderer,
            255,
            220,
            80,
            255
        );
    }
    else
    {
        SDL_SetRenderDrawColor(
            sdlRenderer,
            120,
            120,
            130,
            255
        );
    }

    SDL_RenderDrawRect(
        sdlRenderer,
        &buttonRect
    );

    /*
        Draw a thicker border when hovered
    */
    if (hovered)
    {
        SDL_Rect outerRect =
        {
            buttonRect.x - 2,
            buttonRect.y - 2,
            buttonRect.w + 4,
            buttonRect.h + 4
        };

        SDL_RenderDrawRect(
            sdlRenderer,
            &outerRect
        );
    }

    /*
        Button text
    */
    SDL_Color textColor =
    {
        255,
        255,
        255,
        255
    };

    if (hovered)
    {
        textColor.r = 255;
        textColor.g = 220;
        textColor.b = 80;
    }

    int textWidth;
    int textHeight;

    SDL_Texture* textTexture =
        createTextTexture(
            sdlRenderer,
            renderer->font,
            "START GAME",
            textColor,
            &textWidth,
            &textHeight
        );

    if (textTexture != NULL)
    {
        SDL_Rect textRect =
        {
            buttonRect.x +
                (buttonRect.w - textWidth) / 2,

            buttonRect.y +
                (buttonRect.h - textHeight) / 2,

            textWidth,
            textHeight
        };

        SDL_RenderCopy(
            sdlRenderer,
            textTexture,
            NULL,
            &textRect
        );

        SDL_DestroyTexture(
            textTexture
        );
    }
}


/* ============================================================
   MENU
   ============================================================ */

static void renderMenu(
    Renderer* renderer,
    int windowWidth,
    int mouseX,
    int mouseY
)
{
    SDL_Renderer* sdlRenderer =
        renderer->renderer;

    SDL_Color white =
    {
        255,
        255,
        255,
        255
    };

    SDL_Color gray =
    {
        190,
        190,
        190,
        255
    };

    /*
        Title
    */
    drawCenteredText(
        sdlRenderer,
        renderer->font,
        "GAME FROM SCRATCH",
        white,
        windowWidth,
        120
    );

    /*
        Menu box
    */
    SDL_Rect menuBox =
    {
        (windowWidth - 420) / 2,
        90,
        420,
        360
    };

    SDL_SetRenderDrawColor(
        sdlRenderer,
        25,
        25,
        30,
        255
    );

    SDL_RenderFillRect(
        sdlRenderer,
        &menuBox
    );

    SDL_SetRenderDrawColor(
        sdlRenderer,
        90,
        90,
        100,
        255
    );

    SDL_RenderDrawRect(
        sdlRenderer,
        &menuBox
    );

    /*
        Description
    */
    drawCenteredText(
        sdlRenderer,
        renderer->font,
        "COLLECT THE RED TARGETS",
        gray,
        windowWidth,
        175
    );

    /*
        Start button
    */
    renderMenuButton(
        renderer,
        windowWidth,
        mouseX,
        mouseY
    );

    /*
        Controls
    */
    drawCenteredText(
        sdlRenderer,
        renderer->font,
        "WASD / Arrow Keys to move",
        gray,
        windowWidth,
        350
    );

    drawCenteredText(
        sdlRenderer,
        renderer->font,
        "ENTER or click START GAME",
        gray,
        windowWidth,
        385
    );

    drawCenteredText(
        sdlRenderer,
        renderer->font,
        "ESC to quit",
        gray,
        windowWidth,
        420
    );
}


/* ============================================================
   GAME WORLD
   ============================================================ */

static void renderGameWorld(
    Renderer* renderer,
    const Game* game
)
{
    SDL_Renderer* sdlRenderer =
        renderer->renderer;

    SDL_SetRenderTarget(
        sdlRenderer,
        renderer->gameTexture
    );

    /*
        Background
    */
    SDL_SetRenderDrawColor(
        sdlRenderer,
        GAME_BG_R,
        GAME_BG_G,
        GAME_BG_B,
        255
    );

    SDL_RenderClear(
        sdlRenderer
    );

    /*
        Target
    */
    if (game->state == GAME_STATE_PLAYING)
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

    /*
        Player
    */
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

    /*
        Border
    */
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

    SDL_SetRenderTarget(
        sdlRenderer,
        NULL
    );
}


/* ============================================================
   GAME TEXTURE TO WINDOW
   ============================================================ */

static void drawGameTexture(
    Renderer* renderer
)
{
    SDL_Renderer* sdlRenderer =
        renderer->renderer;

    int windowWidth;
    int windowHeight;

    SDL_GetWindowSize(
        renderer->window,
        &windowWidth,
        &windowHeight
    );

    int availableGameHeight =
        windowHeight - UI_HEIGHT;

    if (availableGameHeight < GAME_HEIGHT)
    {
        availableGameHeight = GAME_HEIGHT;
    }

    /*
        Clear physical window
    */
    SDL_SetRenderDrawColor(
        sdlRenderer,
        OUTSIDE_R,
        OUTSIDE_G,
        OUTSIDE_B,
        255
    );

    SDL_RenderClear(
        sdlRenderer
    );

    /*
        Preserve aspect ratio
    */
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

    SDL_RenderCopy(
        sdlRenderer,
        renderer->gameTexture,
        NULL,
        &destinationRect
    );
}


/* ============================================================
   HUD
   ============================================================ */

static void renderHUD(
    Renderer* renderer,
    const Game* game,
    int windowWidth
)
{
    SDL_Renderer* sdlRenderer =
        renderer->renderer;

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

    /*
        Title
    */
    drawCenteredText(
        sdlRenderer,
        renderer->font,
        "GAME FROM SCRATCH",
        white,
        windowWidth,
        4
    );

    /*
        Score
    */
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

        SDL_DestroyTexture(
            scoreTexture
        );
    }

    /*
        Timer
    */
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

        SDL_DestroyTexture(
            timerTexture
        );
    }

    /*
        Timer bar
    */
    int barX = 20;
    int barY = 62;
    int barWidth = windowWidth - 40;
    int barHeight = 10;

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

    /*
        Status
    */
    const char* statusText;
    SDL_Color statusColor;

    if (game->state == GAME_STATE_GAME_OVER)
    {
        statusText =
            "GAME OVER - R to restart / ENTER for menu";

        statusColor = yellow;
    }
    else
    {
        statusText =
            "WASD / Arrow Keys to move";

        statusColor = gray;
    }

    drawCenteredText(
        sdlRenderer,
        renderer->font,
        statusText,
        statusColor,
        windowWidth,
        88
    );
}


/* ============================================================
   MAIN RENDER FUNCTION
   ============================================================ */

void renderer_render(
    Renderer* renderer,
    const Game* game,
    int mouseX,
    int mouseY
)
{
    SDL_Renderer* sdlRenderer =
        renderer->renderer;

    int windowWidth;
    int windowHeight;

    SDL_GetWindowSize(
        renderer->window,
        &windowWidth,
        &windowHeight
    );

    /*
        MENU
    */
    if (game->state == GAME_STATE_MENU)
    {
        SDL_SetRenderDrawColor(
            sdlRenderer,
            OUTSIDE_R,
            OUTSIDE_G,
            OUTSIDE_B,
            255
        );

        SDL_RenderClear(
            sdlRenderer
        );

        renderMenu(
            renderer,
            windowWidth,
            mouseX,
            mouseY
        );

        SDL_RenderPresent(
            sdlRenderer
        );

        return;
    }

    /*
        GAME WORLD
    */
    renderGameWorld(
        renderer,
        game
    );

    drawGameTexture(
        renderer
    );

    renderHUD(
        renderer,
        game,
        windowWidth
    );

    SDL_RenderPresent(
        sdlRenderer
    );
}


/* ============================================================
   CLEANUP
   ============================================================ */

void renderer_cleanup(
    Renderer* renderer
)
{
    if (renderer->font != NULL)
    {
        TTF_CloseFont(
            renderer->font
        );

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