//Original source code found at : http://programmersranch.blogspot.kr/2014/02/sdl2-pixel-drawing.html

#ifdef _OS_ANDROID_
#error "Android version does not support SDL2 yet."
#endif

#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
	bool leftMouseButtonDown = false;
	bool quit = false;
	SDL_Event event;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window * window = SDL_CreateWindow("SDL2 Pixel Drawing",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 480);
	Uint32 * pixels = malloc(sizeof(Uint32) * 640 * 480);
	memset(pixels, 255, 640 * 480 * sizeof(Uint32));

	while (!quit)
	{
		SDL_UpdateTexture(texture, NULL, pixels, 640 * sizeof(Uint32));

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		SDL_WaitEvent(&event);
		switch (event.type)
		{
		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_LEFT)
				leftMouseButtonDown = false;
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_LEFT)
				leftMouseButtonDown = true;
		case SDL_MOUSEMOTION:
			if (leftMouseButtonDown)
			{
				int mouseX = event.motion.x;
				int mouseY = event.motion.y;
				pixels[mouseY * 640 + mouseX] = 0;
			}
			break;
		case SDL_QUIT:
			quit = true;
			break;
		}
	}

	free(pixels);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
