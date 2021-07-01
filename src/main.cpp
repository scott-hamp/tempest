#include "console.h"
#include "data.h"
#include "dice.h"
#include "input.h"
#include "map.h"
#include "random.h"
#include "ui.h"

int main()
{
	Console::setup(24, 80);
	Console::windowsSetFont(L"Consolas");
	
	Data::load();
	Random::setSeed();
	UI::setup();

	Map::setup({ 80, 21 }, { 0, 1 });
	Map::generate();

	Console::write(0, 0, "Press any key to start...");
	Console::refreshKeyERR();

	Console::setCursor(1);
	UI::log("You awake in the village...");

	do
	{
		Console::clear();

		Map::draw();
		UI::draw();

		Console::refresh();

		auto key = Console::getKey();
		UI::nextTurn();
		Input::handle(key);

		//Console::debug(std::to_string(key));

	} while (true);
}