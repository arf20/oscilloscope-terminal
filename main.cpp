#include "main.hpp"
#include "font.hpp"

#include <SDL2/SDL.h>

#include <iostream>
#include <thread>
#include <cstring>

// Frame buffer
char *frameBuffer = new char[frameBufferSize];

// Sound
point *frameWave = new point[frameWaveSize];

// precalc
static float *characterLengths = new float[256];

// Functions

float sweepCharLength(unsigned char c) {
    auto character = font[c];
    float length = 0.0f;
    for (int i = 0; i < character.size(); i++)
        length += character[i].length();
    return length;
}

float sweepFrameLength() {
    float l = 0;
    for (int i = 0; i < frameBufferSize; i++) {
        l += characterLengths[frameBuffer[i]];
    }
    return l;
}

point sweepChar(float t, unsigned char c) {
    auto character = font[c];

    if (character.size() == 0) return {0, 0};

    // what segment i is at t
    float l = 0.0f;
    int i = -1;
    while (true) {
        if (l > t || i + 1 > character.size() - 1) break;
        l += character[i + 1].length();
        i++;
    }

    auto t2 = t - (l - character[i].length());

    auto Bx = character[i].a.x;
    auto By = character[i].a.y;
    auto Ax = (character[i].b.x - character[i].a.x) / character[i].length();
    auto Ay = (character[i].b.y - character[i].a.y) / character[i].length();

    return {
        (Ax * t2) + Bx,
        (Ay * t2) + By
    };
}

void render() {
    float frameLength = sweepFrameLength();

    float unitsPerSample = (frameLength + characterLengths[219]) / frameWaveSize;
    float samplesPerUnit = 1.0f / unitsPerSample;

    for (int i = 0; i < frameWaveSize - (characterLengths[219] * samplesPerUnit); i++) {
        float t = float(i) * unitsPerSample;

        // what character j is at t
        float l = 0.0f;
        int j = -1;
        while (true) {
            if (l > t || j + 1 > frameBufferSize - 1) break;
            l += characterLengths[frameBuffer[j + 1]];
            j++;
        }

        float t2 = t - (l - characterLengths[frameBuffer[j]]);

        int y = j / WIDTH;
        int x = j - (y * WIDTH);
       
        frameWave[i] = sweepChar(t2, frameBuffer[j]);

        frameWave[i].x += x * (FWIDTH + 1);
        frameWave[i].y += y * (FHEIGHT + 1);
    }

    for (int i = frameWaveSize - (characterLengths[219] * samplesPerUnit); i < frameWaveSize; i++) {
        float t = float(i) * unitsPerSample;
        float t2 = t - frameLength;

        frameWave[i] = sweepChar(t2, 219);

        frameWave[i].x += cursorX * (FWIDTH + 1);
        frameWave[i].y += cursorY * (FHEIGHT + 1);
    }

}

int main() {
    // save initial terminal state first and foremost
    saveTerminal();

    // precalc
    for (int i = 0; i < 256; i++)
        characterLengths[i] = sweepCharLength(i);

    // Initialize SDL Video and Audio Subsystems
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "Error initializing SDL2: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Scope emulator
    std::thread graphicThread(graphLoop);
    graphicThread.detach();

    std::fill(frameBuffer, frameBuffer + frameBufferSize, 0);

    // Audio stuff
    initSDLAudio();

    createTerminal();

    std::thread terminalThread(runTerminal);
    terminalThread.detach();

    while (true) {
        render();
    }
}