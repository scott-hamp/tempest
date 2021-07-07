#include "input.h"

InputCommand Input::_commandActive;

Direction2D Input::getDirection(SDL_Keycode key)
{
	Direction2D dir = { 0, 0 };

	if (key == SDLK_LEFT || key == SDLK_h || key == SDLK_KP_4) dir.X = -1;
	if (key == SDLK_RIGHT || key == SDLK_l || key == SDLK_KP_6) dir.X = 1;
	if (key == SDLK_UP || key == SDLK_k || key == SDLK_KP_8) dir.Y = -1;
	if (key == SDLK_DOWN|| key == SDLK_j || key == SDLK_KP_2) dir.Y = 1;
	if (key == SDLK_y || key == SDLK_KP_7) { dir.X = -1; dir.Y = -1; }
	if (key == SDLK_u || key == SDLK_KP_9) { dir.X = 1; dir.Y = -1; }
	if (key == SDLK_b || key == SDLK_KP_1) { dir.X = -1; dir.Y = 1; }
	if (key == SDLK_n || key == SDLK_KP_3) { dir.X = 1; dir.Y = 1; }

	return dir;
}

void Input::handle(SDL_Keycode key)
{
	handleKey(key);

	if (!Map::player()->turnActionsRemaining()) Map::nextTurn();
}

void Input::handleKey(SDL_Keycode key)
{
	auto direction = getDirection(key);

	if (_commandActive != InputCommand_None)
	{
		// 'Esc'
		if (key == SDLK_ESCAPE)
		{
			_commandActive = InputCommand_None;

			Console::CursorSize = 0.2;
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
			if (key == SDLK_RETURN || key == SDLK_KP_5)
			{
				_commandActive = InputCommand_None;
				Console::CursorSize = 0.2;
				Console::CursorPosition = Map::positionToConsolePosition(Map::player()->position());
				UI::log(Map::getTileDescription(at, true));

				return;
			}

			auto consoleTo = Point2D::add(Console::CursorPosition, direction);
			auto to = Map::positionFromConsolePosition(consoleTo);
			
			if (Map::contains(to) && Map::player()->getView(to, Map::size().Width)->State == 2)
			{
				at = to;
				Console::CursorPosition = consoleTo;
			}
				
			UI::log("Examine what? - " + Map::getTileDescription(at) + " - (Direction / Confirm)");

			if (!direction.isZero()) Console::resetCursorTimer();

			return;
		}

		// Fire
		if (_commandActive == InputCommand_Fire)
		{
			auto at = Map::positionFromConsolePosition(Console::CursorPosition);

			// 'Enter' / 'Numpad 5'
			if (key == SDLK_RETURN || key == SDLK_KP_5)
			{
				auto other = Map::getTileObject(Map::positionFromConsolePosition(Console::CursorPosition));

				if (other == nullptr)
					UI::log("There's nothing to fire at there.");
				else
				{
					if (other == Map::player())
						UI::log("You can't fire at yourself.");
					else
					{
						if (!other->hasBehavior(L"ac") || !other->hasBehavior(L"hp"))
							UI::log("That can't be attacked.");
						else
						{
							if (Map::player()->getBehaviorProperty(L"faction", L"value").compare(other->getBehaviorProperty(L"faction", L"value")) == 0)
								UI::log("You wouldn't think of it!");
							else
								Map::objectTryInteraction(Map::player(), MapObjectInteraction_Attack, other);
						}
					}
				}

				_commandActive = InputCommand_None;
				Console::CursorSize = 0.2;
				Console::CursorPosition = Map::positionToConsolePosition(Map::player()->position());

				return;
			}

			auto consoleTo = Point2D::add(Console::CursorPosition, direction);
			auto to = Map::positionFromConsolePosition(consoleTo);

			if (Map::contains(to) && Map::player()->getView(to, Map::size().Width)->State == 2)
			{
				at = to;
				Console::CursorPosition = consoleTo;
			}

			UI::log("Fire at what? - " + Map::getTileDescription(at) + " - (Direction / Confirm)");

			if (!direction.isZero()) Console::resetCursorTimer();

			return;
		}

		// Open / close
		if (_commandActive == InputCommand_OpenClose)
		{
			if (direction.isZero())
			{
				UI::log("Open / close what? - (Direction)");
				return;
			}

			auto to = Point2D::add(Map::player()->position(), direction);
			auto other = Map::getTileObject(to);
			
			if (other == nullptr)
				UI::log("There's nothing there to open or close.");
			else
				Map::objectTryInteraction(Map::player(), MapObjectInteraction_OpenCloseOther, other);

			_commandActive = InputCommand_None;

			return;
		}

		// Wear / wield
		if (_commandActive == InputCommand_WearWield)
		{
			if (key < SDLK_a || key > Map::player()->getInventorySize() + SDLK_a)
			{
				auto size = Map::player()->getInventorySize();
				char end = 'a' + (size - 1);
				UI::log("Wear / wield what? - (a - " + std::string(1, end) + ")");

				return;
			}

			auto index = key - SDLK_a;
			Map::objectTryInteraction(Map::player(), MapObjectInteraction_WearWield, Map::player()->getInventory(index));

			_commandActive = InputCommand_None;
			UI::setPanelVisible(UIPanel_Inventory, false);
			Console::CursorSize = 0.2;

			return;
		}

		return;
	}

	// UI inventory is open
	if (UI::panelIsVisible(UIPanel_Equipment) || UI::panelIsVisible(UIPanel_Inventory))
	{
		// 'Esc'
		if (key == SDLK_ESCAPE)
		{
			Console::CursorSize = 0.2;
			UI::setPanelVisible(UIPanel_Equipment, false);
			UI::setPanelVisible(UIPanel_Inventory, false);

			return;
		}

		return;
	}

	// Ascend - '<'
	if (key == SDLK_COMMA && (SDL_GetModState() & KMOD_SHIFT))
	{
		Map::objectTryInteraction(Map::player(), MapObjectInteraction_Ascend);

		return;
	}

	// Descend - '>'
	if (key == SDLK_PERIOD && (SDL_GetModState() & KMOD_SHIFT))
	{
		Map::objectTryInteraction(Map::player(), MapObjectInteraction_Descend);

		return;
	}

	// Equipment - 'e'
	if (key == SDLK_e)
	{
		Console::CursorSize = 0.0;
		UI::setPanelVisible(UIPanel_Equipment, true);

		return;
	}

	// Examine - 'x'
	if (key ==SDLK_x)
	{
		_commandActive = InputCommand_Examine;
		UI::log("Examine what? - " + Map::getTileDescription(Map::player()->position()) + " - (Direction / Confirm)");
		Console::CursorSize = 1.0;
		Console::resetCursorTimer();

		return;
	}

	// Fire - 'f'
	if (key == SDLK_f)
	{
		auto rangedWeapon = Map::player()->getEquipment(L"in right hand");

		if (rangedWeapon == nullptr)
		{
			UI::log("You have no ranged weapon ready.");
			return;
		}

		auto uses = rangedWeapon->getBehaviorPropertyEffectValue(L"equipment", L"used-effects", L"uses");

		if (uses.length() > 0)
		{
			auto missles = Map::player()->getEquipment(L"as missles");

			if (missles == nullptr)
			{
				UI::log("You have no " + Strings::from(uses) + " ready.");
				return;
			}

			if (missles->getBehaviorProperty(L"equipment", L"missle-type").compare(uses) != 0)
			{
				UI::log("You need have " + Strings::from(uses) + " ready.");
				return;
			}
		}

		_commandActive = InputCommand_Fire;
		UI::log("Fire at what? - " + Map::getTileDescription(Map::player()->position()) + " - (Direction / Confirm)");
		Console::CursorSize = 1.0;
		Console::resetCursorTimer();

		return;
	}

	// Inventory - 'i'
	if (key == SDLK_i)
	{
		Console::CursorSize = 0.0;
		UI::setPanelVisible(UIPanel_Inventory, true);

		return;
	}

	// Open / close - 'c'
	if (key == SDLK_c)
	{
		_commandActive = InputCommand_OpenClose;
		UI::log("Open / close what? - (Direction)");

		return;
	}

	// Pick up - 'g' / ','
	if (key == SDLK_g || key == SDLK_COMMA)
	{
		Map::objectTryInteraction(Map::player(), MapObjectInteraction_PickUp);
		return;
	}

	// Wait - '.' / numpad 5
	if (key == SDLK_PERIOD || key == SDLK_KP_5)
	{
		Map::player()->takeTurnAction();

		return;
	}

	// Wear / wield - 'w'
	if (key == SDLK_w)
	{
		_commandActive = InputCommand_WearWield;

		auto size = Map::player()->getInventorySize();
		char end = 'a' + (size - 1);
		UI::log("Wear / wield what? - (a - " + std::string(1, end) + ")");
		UI::setPanelVisible(UIPanel_Inventory, true);
		Console::CursorSize = 0.0;

		return;
	}

	if (direction.isZero()) return;

	Map::objectTryInteraction(Map::player(), MapObjectInteraction_Move, direction);
}