#pragma once

#ifndef Console_H
#define Console_H

#include <vector>
#include "data.h"
#include "sprites.h"
#include "texture-fonts.h"

class ConsoleCell
{
	public:
		wchar_t Chr;
		SDL_Color FG;
		SDL_Color BG;
		std::string Font;
		ConsoleCell(wchar_t, SDL_Color, SDL_Color, std::string);
		~ConsoleCell();
};

class Console
{
	private:
		static std::vector<ConsoleCell*> _cells;
		static double _cursorTimer;
		static bool _cursorVisible;
		static TextureFont* _font;
		static std::map<std::string, TextureFont*> _fonts;
		static std::vector<std::string> _fontNames;
		static std::map<std::string, SDL_Color> _palette;
		static SDL_Renderer* _renderer;
		static bool _renderScanLines;
		static Size2D _size;
		static Sprite* _vignetteSprite;
	public:
		static Point2D CursorPosition;
		static double CursorSize;
		static void clear();
		static void clearLine(int);
		static void clearLines(int, int);
		static bool contains(Point2D);
		static void end();
		static SDL_Color getColor(std::string);
		static void render();
		static void resetCursorTimer();
		static void setFont(std::string);
		static void setup(SDL_Renderer*, std::vector<TextureFont*>, Size2D);
		static Size2D size();
		static Size2D windowSize();
		static void write(wchar_t, Point2D);
		static void write(wchar_t, Point2D, SDL_Color);
		static void write(wchar_t, Point2D, SDL_Color, SDL_Color);
		static void write(std::wstring, Point2D);
		static void write(std::wstring, Point2D, int);
		static void write(std::wstring, Point2D, SDL_Color);
		static void write(std::wstring, Point2D, SDL_Color, SDL_Color);
		static void update(double);
};

#endif
