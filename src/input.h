#pragma once

#ifndef Input_H
#define Input_H

#include <SDL.h>
#include "map.h"
#include "ui.h"

class Input
{
	private:
		static InputCommand _commandActive;
		static Direction2D getDirection(SDL_Keycode);
		static void handleKey(SDL_Keycode);
	public:
		static void handle(SDL_Keycode);
};

#endif
