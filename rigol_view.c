#include <SDL.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"

static void usage(const char *program)
{
	fprintf(stderr, "Usage: %s [opts]\n"
					"\t[-d device]\n",
					program);
	exit(1);
}

int main(int argc, char * const argv[])
{
	const char *dev_name = NULL;
	int opt;
	int dev = -1;
	int e;
	SDL_Window *window = NULL;
	int quitting = 0;


	while ((opt = getopt(argc, argv, "d:")) != -1) {
		switch (opt) {
			case 'd': dev_name = optarg; break;
			default: usage(argv[0]);
		}
	}

	dev = device_open(dev_name);
	if (dev < 0) {
		fprintf(stderr, "Unable to open Rigol device '%s': %s [%d]\n",
			dev_name, strerror(-dev), -dev);
		return -1;
	}
	device_set_keylock(dev, 0);

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
		return -1;
	}

	window = SDL_CreateWindow("Rigol View",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320, 240, SDL_WINDOW_RESIZABLE);
	if (!window) {
		fprintf(stderr, "Unable to initialise display: %s\n", SDL_GetError());
		return -1;
	}
	SDL_Surface *screen = SDL_GetWindowSurface(window);
	if (!screen) {
		fprintf(stderr, "Unable to get SDL screen surface\n");
		return -1;
	}

	while(!quitting) {
	    SDL_Event event;
	    SDL_Surface *surface;
	    while( SDL_PollEvent(&event) ) {
	        if(event.type == SDL_QUIT) {
	            quitting = 1;
	        }
	    }

		e = device_get_bmp(dev, "temp.bmp");
		if (e < 0) {
			fprintf(stderr, "Unable to get waveform: %s [%d]\n", strerror(e), e);
		}
		surface = SDL_LoadBMP("temp.bmp");
		if (surface) {
			SDL_BlitSurface(surface, NULL, screen, NULL);
			SDL_SetWindowIcon(window, surface);
			SDL_FreeSurface(surface);
			SDL_UpdateWindowSurface(window);
		}
		device_set_keylock(dev, 0);
	    SDL_Delay(2);
	}
	SDL_DestroyWindow(window);


	device_close(dev);

	return 0;
}
