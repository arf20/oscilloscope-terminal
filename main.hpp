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

#define COMPUTE_DELAY   17

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
#define BLOCKSFRAME 4          // Frame frequency 50Hz
#define BLOCK_SIZE  2048       // Block size
#define SAMP_RATE   192000     // Sound card sample rate

constexpr float framePeriod = float(BLOCKSFRAME * BLOCK_SIZE) / float(SAMP_RATE);
constexpr float frameFreq = 1.0f / framePeriod;

constexpr int frameWaveSize = int(framePeriod * float(SAMP_RATE)); // Samples
extern point *frameWave;

// Main
void render();

// Audio
void initSDLAudio();

// Terminal
extern bool terminalrunning;
void saveTerminal();
void createTerminal();
void pushKey(char v);
void closeTerminal();
void runTerminal();

// Graphics
void graphLoop();