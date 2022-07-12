# oscilloscope-terminal
Turns your oscilloscope (through a sound card) into a Linux terminal emulator

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

## Terminal
This project has a 80x24 character frame buffer, like a VT100. You could try increasing this size, but sound cards do not have infinite sample rate and bandwidth

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
