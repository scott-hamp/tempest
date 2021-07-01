#pragma once

#ifndef Dice_H
#define Dice_H

#include "random.h"
#include "strings.h"

class DiceRoll
{
	private:
		int _mod;
		int _number;
		int _sides;
	public:
		DiceRoll(std::string);
		~DiceRoll();
		int mod();
		int number();
		int sides();
		int roll();
};

#endif
