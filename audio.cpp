#include "main.hpp"

#include <iostream>

#include <SDL2/SDL.h>

typedef uint32_t sample;

static int blockidx = 0;

void SDLCALL audioCallback(void *userdata, Uint8 *stream, int len) {
	for (int i = 0; i < frameWaveSize; i++) {
		if (sizeof(sample) * ((2 * i) + 1) > len) {
			//std::cout << i << " whoops " << ((2 * i) + 1) << std::endl;
			break;
		}
		((sample*)stream)[2 * i] = 			(frameWave[(blockidx * BLOCK_SIZE) + i].x - ((FWIDTH + 1) * WIDTH) / 2.0f) * (INT32_MAX / ((FWIDTH + 1) * WIDTH));
		((sample*)stream)[(2 * i) + 1] = 	-(frameWave[(blockidx * BLOCK_SIZE) + i].y - ((FHEIGHT + 1) * HEIGHT) / 2.0f) * (INT32_MAX / ((FHEIGHT + 1) * HEIGHT));
	}
	blockidx++;
	if (blockidx >= BLOCKSFRAME) blockidx = 0;
}

void initSDLAudio() {
    // Audio driver selection & init
	int drivers = SDL_GetNumAudioDrivers();
	std::cout << "Audio drivers [" << drivers << "]: " << std::endl;
	for (int i = 0; i < drivers; i++)
		std::cout << "\t#" << i << ": " << SDL_GetAudioDriver(i) << std::endl;
	int driveridx = 0;
	std::cout << "> ";
	std::cin >> driveridx;

	if (SDL_AudioInit(SDL_GetAudioDriver(driveridx)) < 0) {
		std::cout << "Cannot initialize driver: " << SDL_GetError() << std::endl;
		exit(1);
	}

    // Audio output
	int outputDevices = SDL_GetNumAudioDevices(SDL_FALSE);
	std::cout << "Output devices [" << outputDevices << "]: " << std::endl;
	for (int i = 0; i < outputDevices; i++)
		std::cout << "\t#" << i << ": " << SDL_GetAudioDeviceName(i, SDL_FALSE) << std::endl;
	int outputDeviceIdx = 0;
	std::cout << "> ";
	std::cin >> outputDeviceIdx;

	// Sample format
	SDL_AudioSpec format = { };
	format.format =	    AUDIO_S32SYS;	// Signed int (32 bits)
	format.channels =	2;		        // Stereo (XY)
	format.freq =		SAMP_RATE;		// 192KHz
	format.samples =	BLOCK_SIZE;		// 2048 
	format.callback = 	audioCallback;
    format.userdata = 	NULL;

	SDL_AudioSpec obteinedFormat = { };

	// Open
	SDL_AudioDeviceID outputDevice = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(outputDeviceIdx, SDL_FALSE), SDL_FALSE, &format, &obteinedFormat, 0);
	if (outputDevice == 0) {
		std::cout << "Cannot open output device: " << SDL_GetError() << std::endl;
		exit(1);
	}

	std::cout << "Obtained format:" << std::endl;
	std::cout << "\tSample format: " << obteinedFormat.format << std::endl;
	std::cout << "\tChannels: " << int(obteinedFormat.channels) << std::endl;
	std::cout << "\tSample rate: " << obteinedFormat.freq << std::endl;
	std::cout << "\tBlock size: " << obteinedFormat.samples << std::endl;

	// Start output
	SDL_PauseAudioDevice(outputDevice, SDL_FALSE);
}