#include "ui.h"

LogMessage::LogMessage(std::string value, int colorPair)
{
	Value = value;
	ColorPair = ColorPair;
}

LogMessage::~LogMessage() { }


std::vector<LogMessage*>UI::_log;
std::vector<LogMessage*> UI::_logTurn;

void UI::draw()
{
	auto lm = getLogTurn();
	if (lm != nullptr)
		Console::write(0, 15, lm->Value, lm->ColorPair);
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

void UI::nextTurn()
{
	_logTurn.erase(_logTurn.begin(), _logTurn.end());
}