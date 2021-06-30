#pragma once

#ifndef UI_H
#define UI_H

#include <vector>
#include "console.h"
#include "enums.h"
#include "map.h"

class LogMessage
{
	public:
		LogMessage(std::string, int);
		~LogMessage();
		std::string Value;
		int ColorPair;
};

class UI
{
	private:
		static std::vector<LogMessage*> _log;
		static std::vector<LogMessage*> _logTurn;
		static std::vector<UIPanel> _panelsVisible;
	public:
		static void draw();
		static LogMessage* getLogTurn(int = 0);
		static void log(std::string, int = 1);
		static void logTurnNext();
		static void nextTurn();
		static bool panelIsVisible(UIPanel);
		static void setPanelVisible(UIPanel, bool);
		static void setup();
};

#endif