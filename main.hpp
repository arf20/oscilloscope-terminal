#pragma once

#include <cmath>

struct point {
    float x, y;
};

struct line {
    point a, b;

    float length() {
        return sqrt(pow((b.x - a.x), 2) + pow((b.y - a.y), 2));
    }
};

// Frame buffer
#define WIDTH   80
#define HEIGHT  24

#define FWIDTH  4
#define FHEIGHT 9

constexpr int frameBufferSize = WIDTH * HEIGHT;
extern char *frameBuffer;

extern int cursorX;
extern int cursorY;

// Sound
#define FFREQ   20.0f      // Frame frequency 50Hz
#define SRATE   192000  // Sound card sample rate

constexpr float framePeriod = 1 / FFREQ;

constexpr int frameWaveSize = int(framePeriod * float(SRATE)); // Samples
extern point *frameWave;

// Main
void render();

// Terminal
void createTerminal();
void runTerminal();

// Graphics
bool graphInit();
void graphLoop();