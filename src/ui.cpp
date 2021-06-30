#include "ui.h"

LogMessage::LogMessage(std::string value, int colorPair)
{
	Value = value;
	ColorPair = ColorPair;
}

LogMessage::~LogMessage() { }


std::vector<LogMessage*>UI::_log;
std::vector<LogMessage*> UI::_logTurn;
std::vector<UIPanel> UI::_panelsVisible;

void UI::draw()
{
	if (panelIsVisible(UIPanel_Equipment))
	{
		auto slots = Map::player()->getEquipmentSlots();

		Console::clearLines(1, Console::size().Height - 3);

		Console::write(2, 1, "Equipment: ");
		Console::write(3, 1, "~~~~~~~~~~ ");

		auto l = 0;
		for (auto slot : slots)
		{
			auto item = Map::player()->getEquipment(slot);
			auto slotStr = slot + L": ";
			std::wstring itemStr = L"(nothing)";

			if (item != nullptr) itemStr = item->getBehaviorProperty(L"description", L"short");
			auto itemStrCP = (item == nullptr) ? 8 : 1;

			Console::write(l + 4, 1, slotStr);
			Console::write(l + 4, 25, itemStr, itemStrCP);
			l++;
		}
	}

	if (panelIsVisible(UIPanel_Inventory))
	{
		auto size = Map::player()->getInventorySize();

		Console::clearLines(1, Console::size().Height - 3);

		Console::write(2, 1, "Inventory: ");
		Console::write(3, 1, "~~~~~~~~~~ ");

		for (int i = 0; i < size; i++)
		{
			auto item = Map::player()->getInventory(i);
			char letter = 'a' + i;
			auto details = item->getBehaviorProperty(L"description", L"short");
			auto eqat = Map::player()->getEquippedSlot(item);

			if (eqat.length() > 0) details += L"(" + eqat + L")";

			Console::write(i + 4, 1, letter);
			Console::write(i + 4, 2, L". " + details);
		}
	}

	if (panelIsVisible(UIPanel_Log))
	{
		auto lm = getLogTurn();
		if (lm != nullptr)
			Console::write(0, 1, lm->Value, lm->ColorPair);
	}

	if (panelIsVisible(UIPanel_Stats))
	{
		Console::write(22, 1, "(Player)  HP: 12/12  STR: 8  DEX: 11  CON: 13  INT: 7  WIS: 9");
		Console::write(23, 1, "LVL: 1  XP: 10  Village");
	}
}

LogMessage* UI::getLogTurn(int i)
{
	if (_logTurn.size() <= i) return nullptr;

	return _logTurn[i];
}

void UI::log(std::string value, int colorPair)
{
	auto lm = new LogMessage(value, colorPair);
	_log.push_back(lm);
	_logTurn.push_back(lm);
}

void UI::logTurnNext()
{
	if (_logTurn.size() == 0) return;

	_logTurn.erase(_logTurn.begin());
}

bool UI::panelIsVisible(UIPanel panel)
{
	return std::find(_panelsVisible.begin(), _panelsVisible.end(), panel) != _panelsVisible.end();
}

void UI::nextTurn()
{
	_logTurn.erase(_logTurn.begin(), _logTurn.end());
}

void UI::setPanelVisible(UIPanel panel, bool visible)
{
	auto isVisible = panelIsVisible(panel);
	if (isVisible == visible) return;

	if (visible)
	{
		_panelsVisible.push_back(panel);
		return;
	}

	auto ind = -1;
	for (int i = 0; i < _panelsVisible.size(); i++)
	{
		if (_panelsVisible[i] != panel) continue;

		ind = i;
		break;
	}

	_panelsVisible.erase(_panelsVisible.begin() + ind);
}

void UI::setup()
{
	_panelsVisible.push_back(UIPanel_Log);
	_panelsVisible.push_back(UIPanel_Stats);
}