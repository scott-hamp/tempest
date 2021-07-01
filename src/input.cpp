#include "input.h"

InputCommand Input::_commandActive;

void Input::handle(int key)
{
	handleKey(key);

	if (!Map::player()->turnActionRemaining()) Map::nextTurn();
}

void Input::handleKey(int key)
{
	auto direction = Console::getDirection(key);

	if (_commandActive != InputCommand_None)
	{
		// 'Esc'
		if (key == 27)
		{
			_commandActive = InputCommand_None;

			Console::setCursor(1);
			Console::CursorPosition = Map::positionToConsolePosition(Map::player()->position());
			
			UI::setPanelVisible(UIPanel_Inventory, false);
			UI::log("Okay then.");

			return;
		}

		// Examine
		if (_commandActive == InputCommand_Examine)
		{
			auto at = Map::positionFromConsolePosition(Console::CursorPosition);

			// 'Enter' / 'Numpad 5'
			if (key == 10 || key == 53)
			{
				_commandActive = InputCommand_None;
				Console::setCursor(1);
				Console::CursorPosition = Map::positionToConsolePosition(Map::player()->position());
				UI::log(Map::getTileDescription(at, true));

				return;
			}

			auto consoleTo = Point2D::add(Console::CursorPosition, direction);
			auto to = Map::positionFromConsolePosition(consoleTo);
			
			if (Map::contains(to))
			{
				at = to;
				Console::CursorPosition = consoleTo;
			}
				
			UI::log("Examine what? - " + Map::getTileDescription(at) + " - (Direction / Confirm)");

			return;
		}

		// Wear / wield
		if (_commandActive == InputCommand_WearWield)
		{
			if (key < 'a' || key > Map::player()->getInventorySize() + 'a')
				return;

			auto index = key - 'a';
			Map::objectTryInteraction(Map::player(), MapObjectInteraction_WearWield, Map::player()->getInventory(index));

			_commandActive = InputCommand_None;
			UI::setPanelVisible(UIPanel_Inventory, false);

			return;
		}

		return;
	}

	// UI inventory is open
	if (UI::panelIsVisible(UIPanel_Equipment) || UI::panelIsVisible(UIPanel_Inventory))
	{
		// 'Esc'
		if (key == 27)
		{
			Console::setCursor(1);
			UI::setPanelVisible(UIPanel_Equipment, false);
			UI::setPanelVisible(UIPanel_Inventory, false);

			return;
		}

		return;
	}

	// Ascend - '<'
	if (key == '<')
	{
		Map::objectTryInteraction(Map::player(), MapObjectInteraction_Ascend);

		return;
	}

	// Descend - '>'
	if (key == '>')
	{
		Map::objectTryInteraction(Map::player(), MapObjectInteraction_Descend);

		return;
	}

	// Equipment - 'e'
	if (key == 'e')
	{
		Console::setCursor(0);
		UI::setPanelVisible(UIPanel_Equipment, true);

		return;
	}

	// Examine - 'x'
	if (key == 'x')
	{
		_commandActive = InputCommand_Examine;
		Console::setCursor(2);
		UI::log("Examine what? - " + Map::getTileDescription(Map::player()->position()) + " - (Direction / Confirm)");

		return;
	}

	// Inventory - 'i'
	if (key == 'i')
	{
		Console::setCursor(0);
		UI::setPanelVisible(UIPanel_Inventory, true);

		return;
	}

	// Wear / wield - 'w'
	if (key == 'w')
	{
		_commandActive = InputCommand_WearWield;

		auto size = Map::player()->getInventorySize();
		char end = 'a' + (size - 1);

		UI::log("Wear / wield what? - (a - " + std::string(1, end) + ")");
		UI::setPanelVisible(UIPanel_Inventory, true);
		Console::setCursor(0);

		return;
	}

	if (direction.isZero()) return;

	Map::objectTryInteraction(Map::player(), MapObjectInteraction_Move, direction);
}