#include "console.h"

ConsoleCell::ConsoleCell(wchar_t chr, SDL_Color fg, SDL_Color bg, std::string font)
{
	Chr = chr;
	FG = fg;
	BG = bg;
	Font = font;
}

ConsoleCell::~ConsoleCell() { }

std::vector<ConsoleCell*> Console::_cells;
double Console::_cursorTimer;
bool Console::_cursorVisible;
TextureFont* Console::_font;
std::map<std::string, TextureFont*> Console::_fonts;
std::vector<std::string> Console::_fontNames;
std::map<std::string, SDL_Color> Console::_palette;
SDL_Renderer* Console::_renderer;
bool Console::_renderScanLines;
Size2D Console::_size;
Sprite* Console::_vignetteSprite;
Point2D Console::CursorPosition;
double Console::CursorSize;

void Console::clear()
{
	clearLines(0, _size.Height);
}

void Console::clearLine(int y)
{
	for (int x = 0; x < _size.Width; x++)
		write(L' ', { x, y }, _palette["white"], _palette["background"]);
}

void Console::clearLines(int y, int height)
{
	for (int yy = y; yy < y + height; yy++)
		clearLine(yy);
}

bool Console::contains(Point2D point)
{
	return point.X >= 0 && point.Y >= 0 && point.X < _size.Width && point.Y < _size.Height;
}

void Console::end()
{
	delete _vignetteSprite;
}

SDL_Color Console::getColor(std::string colorName)
{
	return _palette[colorName];
}

void Console::render()
{
	_vignetteSprite->setOpacity(0.8);
	_vignetteSprite->render(_renderer, { windowSize().Width / 2, windowSize().Height / 2 });

	auto firstFont = _fonts[_fontNames[0]];

	for (int y = 0; y < _size.Height; y++)
	{
		for (int x = 0; x < _size.Width; x++)
		{
			auto cell = _cells[(y * _size.Width) + x];
			auto cellFont = _fonts[cell->Font];

			SDL_Point point = { x * firstFont->cellSize().Width + ((firstFont->cellSize().Width - cellFont->cellSize().Width) / 2), y * firstFont->cellSize().Height + ((firstFont->cellSize().Height - cellFont->cellSize().Height) / 2) };

			if (cell->BG.a != 0)
			{
				SDL_Rect rect;
				rect.x = point.x;
				rect.y = point.y;
				rect.w = firstFont->cellSize().Width;
				rect.h = firstFont->cellSize().Height;

				SDL_SetRenderDrawColor(_renderer, cell->BG.r, cell->BG.g, cell->BG.b, 255);
				SDL_RenderFillRect(_renderer, &rect);
			}

			cellFont->write(cell->Chr, point, cell->FG);

			if (_cursorVisible && CursorSize > 0.0 && CursorPosition.X == x && CursorPosition.Y == y)
			{
				int h = (int)(CursorSize * (double)firstFont->cellSize().Height);

				SDL_Rect rect;
				rect.x = point.x;
				rect.y = point.y + firstFont->cellSize().Height - h;
				rect.w = firstFont->cellSize().Width;
				rect.h = h;

				SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
				SDL_RenderFillRect(_renderer, &rect);
			}
		}
	}

	if (!_renderScanLines) return;

	SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 5);
	for(int y = 0; y < windowSize().Height; y += 5)
		SDL_RenderDrawLine(_renderer, 0, y, windowSize().Width, y);
}

void Console::resetCursorTimer()
{
	_cursorVisible = true;
	_cursorTimer = 500.0;
}

void Console::setFont(std::string fontName)
{
	_font = _fonts[fontName];
}

void Console::setup(SDL_Renderer* renderer, std::vector<TextureFont*> fonts, Size2D size)
{
	_renderer = renderer;
	_font = fonts[0];
	_size = size;
	_renderScanLines = true;
	CursorSize = 0.2;

	for (auto f : fonts)
	{
		_fonts[f->name()] = f;
		_fontNames.push_back(f->name());
	}

	for (int i = 0; i < _size.Width * _size.Height; i++)
		_cells.push_back(new ConsoleCell(L' ', { 255, 255, 255, 255 }, { 255, 255, 255, 0 }, _font->name()));

	for (auto const &l : Data::getPalette())
	{
		auto parts = Strings::split(l, ':');
		auto colorName = parts[0];
		parts = Strings::split(parts[1], ',');
		SDL_Color color = { std::stoi(parts[0]), std::stoi(parts[1]), std::stoi(parts[2]), std::stoi(parts[3]) };
		_palette[colorName] = color;
	}

	_vignetteSprite = new Sprite(_renderer, "assets/vignette-0.png");
}

Size2D Console::size() { return _size; }

Size2D Console::windowSize()
{
	return { _size.Width * _font->cellSize().Width, _size.Height * _font->cellSize().Height };
}

void Console::write(wchar_t chr, Point2D point)
{
	if (!contains(point)) return;

	auto cell = _cells[(point.Y * _size.Width) + point.X];
	cell->Chr = chr;
	cell->Font = _font->name();
}

void Console::write(wchar_t chr, Point2D point, SDL_Color fg)
{
	if (!contains(point)) return;

	auto cell = _cells[(point.Y * _size.Width) + point.X];
	cell->Chr = chr;
	cell->FG = fg;
	cell->Font = _font->name();
}

void Console::write(wchar_t chr, Point2D point, SDL_Color fg, SDL_Color bg)
{
	if (!contains(point)) return;

	auto cell = _cells[(point.Y * _size.Width) + point.X];
	cell->Chr = chr;
	cell->FG = fg;
	cell->BG = bg;
	cell->Font = _font->name();
}

void Console::write(std::wstring str, Point2D point)
{
	for (int i = 0; i < str.length(); i++)
	{
		write(str[i], point);
		point.X++;
	}
}

void Console::write(std::wstring str, Point2D point, int width)
{
	auto pointX = point.X;
	auto lineLength = 0;

	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == L' ' || str[i] == L'.' || str[i] == L',' || str[i] == L'!' || str[i] == L':' || str[i] == ';')
		{
			auto length = 0;
			for (int j = i + 1; j < str.length(); j++)
			{
				if (str[j] == L' ' || str[j] == L'.' || str[j] == L',' || str[j] == L'!' || str[j] == L':' || str[j] == ';')
					break;

				length++;
			}

			if (length + lineLength > width)
			{
				point.X = pointX;
				point.Y++;
				lineLength = 0;
				continue;
			}
		}

		write(str[i], point);

		point.X++;
		lineLength++;

		if (lineLength > width)
		{
			point.X = pointX;
			point.Y++;
			lineLength = 0;
		}
	}
}

void Console::write(std::wstring str, Point2D point, SDL_Color fg)
{
	for (int i = 0; i < str.length(); i++)
	{
		write(str[i], point, fg);
		point.X++;
	}
}

void Console::write(std::wstring str, Point2D point, SDL_Color fg, SDL_Color bg)
{
	for (int i = 0; i < str.length(); i++)
	{
		write(str[i], point, fg, bg);
		point.X++;
	}
}

void Console::update(double delta)
{
	_cursorTimer -= delta;
	if (_cursorTimer <= 0.0)
	{
		_cursorVisible = (_cursorVisible) ? false : true;
		_cursorTimer = 500.0;
	}
}
