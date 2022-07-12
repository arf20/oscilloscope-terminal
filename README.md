# oscilloscope-terminal
Turns your oscilloscope (through a sound card) into a Linux terminal emulator

## Hardware requirements
To be able to see your terminal using real hardware, you need:
 - any old two channel analog oscilloscope capable of doing XY mode
 - any sound card capable of 24-bit 192KHz audio with little noise, for enough clarity
In my case a Philips PM 3350A and a Behringer UMC204HD, but I have a noise issue somewhere.
If you do not have these, you can use any software vectorscope that takes your sound output.

## Audio configuration
Samples per block (i.e. block size) and sample rate are defined in main.hpp. Increasing the sample rate increases the resolution, but you can only go as high as your sound card will accept. Block size should be between 512 and 4096. Blocks per frame defines how many blocks (and sample) will make a whole frame. Increase this to have more resolution, but this decreases the overall vertical frequency. Too high of a value makes the picture flickery.

## Build
Requires SDL2 and a POSIX system
`apt install libsdl2-dev`
Then as any other simple CMake project:
```
mkdir build
cd build
cmake ..
make
```

## Launch
Simply `./oscilloscope-terminal`. Do note, this software disables the terminal canonical mode, so it can process key presses as fast as they are being pressed without waiting for a \n. So after exiting the terminal with `exit`, your terminal might be broken. Fix this by running `reset`.
The project includes a vectorscope emulator with SDL, mostly for debugging.

## Font
Shamefully copied Windows's `Proxy 1` font. It was chosen for being a very easy to replicate vector font, made entirely by straight lines connecting points in a 4x9 grid. It is all manually defined in `font.h`.

## Terminal
This project has a 80x24 character frame buffer, like a VT100. You could try increasing this size, but sound cards do not have infinite sample rate and bandwidth.

Supported ASCII control characters and ANSI escape sequences:
```
BS
HT
LF
CR

ESC [ A
ESC [ B
ESC [ C
ESC [ D
ESC [ E
ESC [ F
ESC [ G
ESC [ H
ESC [ J
ESC [ K
```
