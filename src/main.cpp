#include <conio.h>
#include <iostream>
#include <SDL.h>
#include <stdlib.h>
#include <windows.h>
#include "console.h"
#include "input.h"
#include "map.h"
#include "SDL_syswm.h"

#define MAX_FRAME_RATE	60.0

SDL_Window* _window;
SDL_Renderer* _renderer;

int main(int argc, char* argv[])
{
	try
	{
		SDL_Init(SDL_INIT_EVERYTHING);
		TTF_Init();

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

		SDL_DisplayMode displayMode;

		if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0)
		{
			SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
			return 1;
		}

		printf("Creating window...\n");

		_window = SDL_CreateWindow
		("SDL Console Template",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			displayMode.w * 0.1,
			displayMode.h * 0.1,
			SDL_WINDOW_SHOWN);

		printf("Creating renderer...\n");

		_renderer = SDL_CreateRenderer(_window, -1, 0);

		SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(_renderer, 15, 25, 45, 255);

		printf("Creating fonts...\n");

		auto consoleFont1 = new TTFTextureFont
			("inconsolata",
			_renderer,
			"assets/inconsolata-sb.ttf",
			26);

		/*
		auto consoleFont2 = new CP437TextureFont
			("tiles",
			_renderer,
			"assets/tiles-16x24.png",
			{ 16, 24 },
			1.0);
		*/

		printf("Creating console...\n");

		Console::setup
			(_renderer, 
			{ consoleFont1 }, 
			{ 80, 24 });

		SDL_SetWindowSize(_window, Console::windowSize().Width, Console::windowSize().Height);

		printf("Loading data...\n");

		Data::load();
		Random::setSeed();
		UI::setup();

		printf("Generating map...\n");

		Map::setup({ Console::size().Width, Console::size().Height - 3 }, { 0, 1 });
		Map::generate();
		Map::draw();

		printf("Ready!\n");

		unsigned int ticksA = SDL_GetTicks();
		unsigned int ticksB = SDL_GetTicks();
		double delta = 0.0;
		int frame = 0;

		while (true)
		{
			SDL_Event e;

			if (SDL_PollEvent(&e))
			{
				if (e.type == SDL_WINDOWEVENT)
				{
					if (e.window.event == SDL_WINDOWEVENT_CLOSE)
						break;
				}

				if (e.type == SDL_KEYDOWN)
				{
					UI::nextTurn();
					Input::handle(e.key.keysym.sym);
				}
			}

			ticksA = SDL_GetTicks();
			delta += ticksA - ticksB;

			if (delta > 1000 / MAX_FRAME_RATE)
			{
				ticksB = ticksA;
				frame++;

				Console::update(delta);

				SDL_SetRenderDrawColor(_renderer, 15, 25, 45, 255);
				SDL_RenderClear(_renderer);

				Console::clear();
				Map::draw();
				UI::draw();
				Console::render();

				SDL_RenderPresent(_renderer);

				delta = 0;
			}

			ticksB = SDL_GetTicks();
		}

		Console::end();
		SDL_DestroyRenderer(_renderer);
		SDL_DestroyWindow(_window);
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
		std::cout << "Press any key to exit." << std::endl;

		auto c = _getch();
	}
}