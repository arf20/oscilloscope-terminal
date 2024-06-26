#include "main.hpp"

#include <SDL2/SDL.h>

#include <iostream>

#define WWIDTH  1280
#define WHEIGHT 720

#define SCALE   3.0f

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;

static SDL_bool done = SDL_FALSE;

void graphInit() {
    // Create window
    window = SDL_CreateWindow(
        "scope emulator",                   // window title
        SDL_WINDOWPOS_UNDEFINED,            // initial x position
        SDL_WINDOWPOS_UNDEFINED,            // initial y position
        WWIDTH,                             // width, in pixels
        WHEIGHT,                            // height, in pixels
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    SDL_Delay(100);

    if (window == NULL) {
        std::cout << "Error opening window: " << SDL_GetError() << std::endl;
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    if (renderer == NULL) {
        std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
        exit(1);
    }
}


void graphLoop() {
    graphInit();

    int lim = 0;

    SDL_Event event;
    while (!done) {
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Draw
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

        for (int i = 0; i < frameWaveSize - 1; i++) {
            //SDL_RenderDrawLine(renderer, SCALE * frameWave[i].x, SCALE * frameWave[i].y, SCALE * frameWave[i + 1].x, SCALE * frameWave[i + 1].y);
            SDL_RenderDrawPoint(renderer,  SCALE * frameWave[i].x, SCALE * frameWave[i].y);
        }

        lim += 10;
        if (lim > frameWaveSize) lim = 0;
        
        SDL_RenderPresent(renderer);
        

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    done = SDL_TRUE;
                    closeTerminal();
                } break;
                case SDL_KEYDOWN: {
                    pushKey(event.key.keysym.sym);
                } break;
            }
        }

        SDL_Delay(COMPUTE_DELAY);
    }

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}