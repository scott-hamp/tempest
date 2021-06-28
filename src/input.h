#pragma once

#ifndef Input_H
#define Input_H

#include "map.h"
#include "ui.h"

class Input
{
	private:
		static InputCommand _commandActive;
		static void handleKey(int);
	public:
		static void handle(int);
};

#endif
