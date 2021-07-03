#pragma once

#ifndef TextureFonts_H
#define TextureFonts_H

#include <exception>
#include <map>
#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "geometry.h"
#include "SDL_ttf.h"

class TextureFont
{
	protected:
		Size2D _cellSize;
		std::string _name;
		SDL_Renderer* _renderer;
	public:
		Size2D cellSize();
		std::string name();
		virtual void write(const char, SDL_Point) = 0;
		virtual void write(const char, SDL_Point, SDL_Color) = 0;
		virtual void write(const char*, SDL_Point) = 0;
		virtual void write(const char*, SDL_Point, SDL_Color) = 0;
		virtual void write(const wchar_t, SDL_Point) = 0;
		virtual void write(const wchar_t, SDL_Point, SDL_Color) = 0;
		virtual void write(const wchar_t*, SDL_Point) = 0;
		virtual void write(const wchar_t*, SDL_Point, SDL_Color) = 0;
};

class CP437TextureFont : public TextureFont
{
	private:
		std::map<wchar_t, SDL_Rect> _glyphs;
		double _scale;
		SDL_Texture* _texture;
	public:
		CP437TextureFont(std::string, SDL_Renderer*, const char*, Size2D, double);
		~CP437TextureFont();
		Size2D cellSizeScaled();
		int getWidth(const char*);
		void setScale(double);
		void write(const char, SDL_Point);
		void write(const char, SDL_Point, SDL_Color);
		void write(const char*, SDL_Point);
		void write(const char*, SDL_Point, SDL_Color);
		void write(const wchar_t, SDL_Point);
		void write(const wchar_t, SDL_Point, SDL_Color);
		void write(const wchar_t*, SDL_Point);
		void write(const wchar_t*, SDL_Point, SDL_Color);
};

class TTFTextureFont : public TextureFont
{
	private:
		TTF_Font* _font;
		int _fontSize;
		std::vector<std::string> getLines(const char*, int);
		std::vector<std::wstring> getLines(const wchar_t*, int);
		std::map<wchar_t, SDL_Texture*> _glyphs;
		std::map<wchar_t, Size2D> _glyphSizes;
	public:
		TTFTextureFont(std::string, SDL_Renderer*, const char*, unsigned int);
		~TTFTextureFont();
		int fontSize();
		int getHeight(const char*, int);
		int getWidth(const char);
		int getWidth(const char*);
		int getWidth(const char*, int);
		int getWidth(const wchar_t);
		int getWidth(const wchar_t*);
		int getWidth(const wchar_t*, int);
		void write(const char, SDL_Point);
		void write(const char, SDL_Point, SDL_Color);
		void write(const char*, SDL_Point);
		void write(const char*, SDL_Point, SDL_Color);
		void write(const char*, SDL_Point, int);
		void write(const wchar_t, SDL_Point);
		void write(const wchar_t, SDL_Point, SDL_Color);
		void write(const wchar_t*, SDL_Point);
		void write(const wchar_t*, SDL_Point, SDL_Color);
		void write(const wchar_t*, SDL_Point, int);
};

#endif