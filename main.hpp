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
#define HEIGHT  25

#define FWIDTH  4
#define FHEIGHT 9

extern char *frameBuffer;

// Sound
#define FFREQ   50.0f      // Frame frequency 50Hz
#define SRATE   192000  // Sound card sample rate

constexpr float framePeriod = 1 / FFREQ;

constexpr int frameWaveSize = int(framePeriod * float(SRATE)); // Samples
extern point *frameWave;

// Graphics
bool graphInit();
void graphLoop();