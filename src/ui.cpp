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
	Console::setFont("text");

	if (panelIsVisible(UIPanel_Equipment))
	{
		auto slots = Map::player()->getEquipmentSlots();

		Console::clearLines(1, Console::size().Height - 3);

		Console::write(L"Equipment: ", { 1, 2 });
		Console::write(L"────────── ", { 1, 3 });

		auto l = 0;
		for (auto slot : slots)
		{
			auto item = Map::player()->getEquipment(slot);
			auto slotStr = slot + L": ";
			std::wstring itemStr = L"(nothing)";

			if (item != nullptr) itemStr = item->getBehaviorProperty(L"description", L"short");
			auto itemStrColor = (item == nullptr) ? "dark-grey" : "yellow";

			Console::write(slotStr, { 1, l + 4 });
			Console::write(itemStr, { 25, l + 4 }, Console::getColor(itemStrColor));
			l++;
		}
	}

	if (panelIsVisible(UIPanel_Inventory))
	{
		auto size = Map::player()->getInventorySize();

		Console::clearLines(1, Console::size().Height - 3);

		Console::write(L"Inventory: ", { 1, 2 });
		Console::write(L"────────── ", { 1, 3 });

		for (int i = 0; i < size; i++)
		{
			auto item = Map::player()->getInventory(i);
			char letter = 'a' + i;
			auto details = item->getBehaviorProperty(L"description", L"short");
			auto eqat = Map::player()->getEquippedSlot(item);

			if (eqat.length() > 0) details += L"(" + eqat + L")";

			Console::write(letter, { 1, i + 4 });
			Console::write(L". " + details, { 2, i + 4 });
		}
	}

	if (panelIsVisible(UIPanel_Log))
	{
		int y = 0;
		for (auto lm : _logTurn)
		{
			Console::clearLine(y);
			auto str = lm->Value;
			std::wstring wstr(str.begin(), str.end());
			Console::write(wstr, { 1, y });
			y++;
		}
	}

	if (panelIsVisible(UIPanel_Stats))
	{
		auto attributes = Strings::split(Map::player()->getBehaviorProperty(L"attributes", L"value"), L',');

		Console::write(L"(Player)  HP: 12/12  STR: " + attributes[0] + L"  DEX: " + attributes[1] + L"  CON: " + attributes[2] + L"  INT: " + attributes[3] + L"  WIS: " + attributes[4] + L"  CHA: " + attributes[5], { 1, 22 });
		Console::write(L"LVL: 1  XP: 10  Village", { 1, 23 });
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