#include "input.h"

InputCommand Input::_commandActive;

void Input::handle(int key)
{
	handleKey(key);

	if (!Map::player()->turnActionRemaining()) Map::nextTurn();
}

void Input::handleKey(int key)
{
	auto uilt = UI::getLogTurn(1);
	if (uilt != nullptr)
	{
		UI::logTurnNext();
		return;
	}

	auto direction = Console::getDirection(key);

	if (_commandActive != InputCommand_None)
	{
		// 'Esc'
		if (key == 27)
		{
			_commandActive = InputCommand_None;
			Console::setCursor(1);
			Console::CursorPosition = Map::positionToConsolePosition(Map::player()->position());
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

		return;
	}

	// Ascend - '<'
	if (key == '<')
	{
		Map::objectTryInteraction(Map::player(), MapObjectInteraction_Ascend);

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

	// Descend - '>'
	if (key == '>')
	{
		Map::objectTryInteraction(Map::player(), MapObjectInteraction_Descend);

		return;
	}

	if (direction.isZero()) return;

	Map::objectTryInteraction(Map::player(), MapObjectInteraction_Move, direction);
}