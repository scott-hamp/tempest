#include "dice.h"

DiceRoll::DiceRoll(std::string expression)
{
	_mod = 0; _number = 0; _sides = 0;

	if (expression.length() == 0) return;
	if (Strings::split(expression, 'd').size() != 2) return;

	std::string left = "";
	auto ind = 0;
	for (auto const& c : expression)
	{
		if (c == '+' || c == '-') break;

		left += c;
		ind++;
	}

	std::string right = "";
	if (ind < expression.length()) right = expression.substr(ind);

	auto leftParts = Strings::split(left, 'd');
	_number = stoi(leftParts[0]);
	_sides = stoi(leftParts[1]);

	if (right.length() == 0) return;

	std::vector<char> dels = { '+', '-' };
	auto mods = Strings::split(right, dels);
	std::vector<int> signs;

	for (int i = 0; i < right.length(); i++)
	{
		if (right[i] != '+' && right[i] != '-') continue;

		signs.push_back((right[i] == '+') ? 1 : -1);
	}

	if (mods.size() != signs.size()) return;

	for (int i = 0; i < mods.size(); i++)
		_mod += (signs[i] == 1) ? stoi(mods[i]) : -stoi(mods[i]);
}

DiceRoll::~DiceRoll() { }

int DiceRoll::mod() { return _mod; }

int DiceRoll::number() { return _number; }

int DiceRoll::sides() { return _sides; }

int DiceRoll::roll()
{
	auto result = 0;
	for (int i = 0; i < _number; i++)
		result += Random::nextInt(0, _sides) + 1;

	return result + _mod;
}