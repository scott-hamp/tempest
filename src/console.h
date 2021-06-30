#pragma once

#ifndef Console_H
#define Console_H

#include <algorithm>
#include <chrono>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <time.h>
#include <windows.h>
#include "geometry.h"

#define COLOR_GRAY	8

class Console
{
private:
	static const std::wstring CP437;
	static std::string _debug;
	static int _key;
	static Size2D _size;
	static WINDOW* _window;
public:
	static Point2D CursorPosition;
	static void clear();
	static void clearLine(int, int = 0);
	static void clearLines(int);
	static void clearLines(int, int);
	static void clearRect(Rect);
	static void debug(std::string);
	static void end();
	static void fillRandomly();
	static Direction2D getDirection();
	static Direction2D getDirection(int);
	static int getKey();
	static const char getLetter(bool, bool = false);
	static int getNumber();
	static int getNumberSingle();
	static std::string getString();
	static bool getYesNo();
	static void refresh();
	static void refreshKeyERR();
	static void setCursor(int);
	static void setup(int, int);
	static Size2D size();
	static void wait(int);
	static bool windowsSetFont(const wchar_t*);
	static void write(int, int, const char);
	static void write(int, int, const char, int);
	static void write(int, int, const chtype);
	static void write(int, int, const chtype, int);
	static void write(int, int, const char*);
	static void write(int, int, const char*, int);
	static void write(int, int, std::string);
	static void write(int, int, std::string, int);
	static void write(int, int, const wchar_t);
	static void write(int, int, const wchar_t, int);
	static void write(int, int, const wchar_t*);
	static void write(int, int, const wchar_t*, int);
	static void write(int, int, std::wstring);
	static void write(int, int, std::wstring, int);
};

#endif