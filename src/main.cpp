#include <conio.h>
#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include <stdlib.h>
#include <windows.h>
#include "audio.h"
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
		Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
		Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);

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
		("Tempest",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			displayMode.w * 0.1,
			displayMode.h * 0.1,
			SDL_WINDOW_SHOWN);

		printf("Creating renderer...\n");

		_renderer = SDL_CreateRenderer(_window, -1, 0);

		SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(_renderer, 15, 25, 45, 255);

		printf("Loading data...\n");

		Data::load();

		printf("Loading audio...\n");

		Audio::load();

		printf("Creating fonts...\n");

		std::vector<TextureFont*> consoleFonts;
		for (auto const& l : Data::getFonts())
		{
			auto parts = Strings::split(l, ':');
			auto fontName = parts[0];
			parts = Strings::split(parts[1], ',');

			TextureFont* font = nullptr;

			if (parts[0].compare("ttf") == 0)
			{
				font = new TTFTextureFont
					(fontName.c_str(),
					_renderer,
					parts[1].c_str(),
					stoi(parts[2]));
			}
			else
			{
				font = new CP437TextureFont
					(fontName.c_str(),
					_renderer,
					parts[1].c_str(),
					{ stoi(parts[2]), stoi(parts[3]) }, 
					stod(parts[4]));
			}

			consoleFonts.push_back(font);

			printf(("   + '" + fontName + "' (" + parts[0] + ", " + parts[1] + ")\n").c_str());
		}

		printf("Creating console...\n");

		Console::setup
			(_renderer, 
			consoleFonts,
			{ 100, 24 });

		SDL_SetWindowSize(_window, Console::windowSize().Width, Console::windowSize().Height);
		SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

		printf("Setting up...\n");

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
					if (!Map::isAnimating())
					{
						UI::nextTurn();
						Input::handle(e.key.keysym.sym);
					}
				}
			}

			ticksA = SDL_GetTicks();
			delta += ticksA - ticksB;

			if (delta > 1000 / MAX_FRAME_RATE)
			{
				ticksB = ticksA;
				frame++;

				Console::update(delta);
				Map::animate(delta);

				auto colorBG = Console::getColor("background");

				SDL_SetRenderDrawColor(_renderer, colorBG.r, colorBG.g, colorBG.b, 255);
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

		TTF_Quit();
		Mix_Quit();
		
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