#include "console.h"
#include "input.h"
#include "map.h"
#include "ui.h"

int main()
{
	Console::setup(24, 80);
	Console::windowsSetFont(L"Consolas");
	
	Map::setup({ 65, 23 }, { 15, 1 });
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

		Console::debug(std::to_string(key));

	} while (true);
}