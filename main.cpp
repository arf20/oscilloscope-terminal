#include "main.hpp"
#include "font.hpp"

#include <iostream>
#include <thread>
#include <cstring>

// Frame buffer
constexpr int frameBufferSize = WIDTH * HEIGHT;
char *frameBuffer = new char[frameBufferSize];

// Sound
point *frameWave = new point[frameWaveSize];

// precalc
char *characterLengths = new char[128];

// Functions

float sweepCharLength(char c) {
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

point sweepChar(float t, char c) {
    auto character = font[c];

    // what segment i is at t
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

void render() {
    float length = sweepFrameLength();

    for (int i = 0; i < frameWaveSize; i++) {
        float t = float(i) * (length / frameWaveSize);

        // what character j is at t
        float l = 0.0f;
        int j = -1;
        while (true) {
            if (l > t) break;
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

}

int main() {
    // precalc
    for (int i = 0; i < 128; i++)
        characterLengths[i] = sweepCharLength(i);

    // Scope emulator
    if (!graphInit()) exit(1);
    std::thread graphicThread(graphLoop);
    graphicThread.detach();

    std::fill(frameBuffer, frameBuffer + frameBufferSize, 0);
    /*auto str =  "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    auto str2 = "abcdefghijklmnopqrstuvwxyz";
    auto str3 = " !\"#$%&'()*+,-./1234567890";
    auto str4 = ":;<=>?@[\\]^_`{|}~";
    memcpy(frameBuffer, str, strlen(str));
    memcpy(frameBuffer + (WIDTH), str2, strlen(str2));
    memcpy(frameBuffer + (2 * WIDTH), str3, strlen(str3));
    memcpy(frameBuffer + (3 * WIDTH), str4, strlen(str4));

    render();*/

    createTerminal();
    runTerminal();

    int ae;
    std::cin >> ae;
}