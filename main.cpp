#include <iostream>

#include "font.h"

// Frame buffer
#define WIDTH   80
#define HEIGHT  25

char *frameBuffer = new char[WIDTH * HEIGHT];

// Sound
#define FFREQ   50.0f      // Frame frequency 50Hz
#define SRATE   192000  // Sound card sample rate

constexpr float framePeriod = 1 / FFREQ;

struct sample {
    float x, y;
};

int frameWaveSize = int(framePeriod * float(SRATE)); // Samples
sample *frameWave = new sample[frameWaveSize];



// Functions

float sweepCharLength(char c) {
    auto character = font[c];
    float length = 0.0f;
    for (int i = 0; i < character.size(); i++)
        length += character[i].length();
    return length;
}

point sweepChar(float t, char c) {
    auto character = font[c];

    float l = 0.0f;
    int i = -1;
    while (true) {
        if (l > t) break;
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

void render(float T, float sampRate) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char c = frameBuffer[(y * WIDTH) + x];
            if (c == 0) continue;

            // sweep character
            


        }
    }

}

int main() {
    frameBuffer[5] = 'A';
    auto p = sweepChar(4.5f, 'A');
    std::cout << p.x << " " << p.y << std::endl;
}