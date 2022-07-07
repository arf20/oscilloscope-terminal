#include "main.hpp"

#include <SDL2/SDL.h>

#include <iostream>

#define WWIDTH  1280
#define WHEIGHT 720

#define SCALE   10.0f

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

SDL_bool done = SDL_FALSE;

bool graphInit() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Error initializing SDL2: " << SDL_GetError() << std::endl;
    }

    // Create window
    window = SDL_CreateWindow(
        "scope emulator",                     // window title
        SDL_WINDOWPOS_UNDEFINED,            // initial x position
        SDL_WINDOWPOS_UNDEFINED,            // initial y position
        WWIDTH,                              // width, in pixels
        WHEIGHT,                             // height, in pixels
        SDL_WINDOW_OPENGL                   // flags - see below
    );

    if (window == NULL) {
        std::cout << "Error opening window: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    return true;
}


void graphLoop() {
    int lim = 0;


    while (!done) {
        SDL_Event event;

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Draw
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

        for (int i = 0; i < frameWaveSize - 1; i++) {
            SDL_RenderDrawLine(renderer, SCALE * frameWave[i].x, SCALE * frameWave[i].y, SCALE * frameWave[i + 1].x, SCALE * frameWave[i + 1].y);
        }

        lim += 100;
        if (lim > frameWaveSize) lim = 0;
        
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = SDL_TRUE;
            }
        }
    }

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}