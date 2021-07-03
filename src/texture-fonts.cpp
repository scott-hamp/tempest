#include "texture-fonts.h"

Size2D TextureFont::cellSize() { return _cellSize; }

std::string TextureFont::name() { return _name; }

CP437TextureFont::CP437TextureFont(std::string name, SDL_Renderer* renderer, const char* assetPath, Size2D cellSize, double scale)
{
	_name = name;
	_renderer = renderer;
	_cellSize = cellSize;
	_scale = scale;

	SDL_Surface* surface = IMG_Load(assetPath);
	if (!surface)
		throw std::exception("IMG_Load: The sprite image could not be loaded.");

	_texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	const wchar_t* CP437 = L" ☺☻♥♦♣♠•◘○◙♂♀♪♫☼►◄↕‼¶§▬↨↑↓→←∟↔▲▼ !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~⌂ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥₧ƒáíóúñÑªº¿⌐¬½¼¡«»░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■ ";

	int x = 0;
	int y = 0;
	int c = 0;

	for (int i = 0; i < 256; i++)
	{
		auto chr = CP437[i];
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = _cellSize.Width;
		rect.h = _cellSize.Height;

		_glyphs[chr] = rect;

		c++;

		if (c < 16)
		{
			x += _cellSize.Width;

			continue;
		}

		x = 0;
		y += _cellSize.Height;
		c = 0;
	}
}

CP437TextureFont::~CP437TextureFont()
{
	_renderer = nullptr;

	delete _texture;
}

Size2D CP437TextureFont::cellSizeScaled()
{
	return { (int)(_cellSize.Width * _scale), (int)(_cellSize.Height * _scale) };
}

int CP437TextureFont::getWidth(const char* val)
{
	return (int)_cellSize.Width * strlen(val);
}

void CP437TextureFont::setScale(double s) { _scale = s; }

void CP437TextureFont::write(const char val, SDL_Point point)
{
	SDL_Rect srect = _glyphs[val];
	auto css = cellSizeScaled();

	SDL_Rect drect;
	drect.x = point.x;
	drect.y = point.y;
	drect.w = css.Width;
	drect.h = css.Height;

	SDL_RenderCopy(_renderer, _texture, &srect, &drect);
}

void CP437TextureFont::write(const char val, SDL_Point point, SDL_Color col)
{
	SDL_SetTextureColorMod(_texture, col.r, col.g, col.b);

	write(val, point);

	SDL_SetTextureColorMod(_texture, 255, 255, 255);
}

void CP437TextureFont::write(const char* val, SDL_Point point)
{
	for (int i = 0; i < strlen(val); i++)
	{
		write(val[i], point);
		point.x += cellSizeScaled().Width;
	}
}

void CP437TextureFont::write(const char* val, SDL_Point point, SDL_Color col)
{
	for (int i = 0; i < strlen(val); i++)
	{
		write(val[i], point, col);
		point.x += cellSizeScaled().Width;
	}
}

void CP437TextureFont::write(const wchar_t val, SDL_Point point)
{
	SDL_Rect srect = _glyphs[val];
	auto css = cellSizeScaled();

	SDL_Rect drect;
	drect.x = point.x;
	drect.y = point.y;
	drect.w = css.Width;
	drect.h = css.Height;

	SDL_RenderCopy(_renderer, _texture, &srect, &drect);
}

void CP437TextureFont::write(const wchar_t val, SDL_Point point, SDL_Color col)
{
	SDL_SetTextureColorMod(_texture, col.r, col.g, col.b);

	write(val, point);

	SDL_SetTextureColorMod(_texture, 255, 255, 255);
}

void CP437TextureFont::write(const wchar_t* val, SDL_Point point)
{
	for (int i = 0; i < wcslen(val); i++)
	{
		write(val[i], point);
		point.x += cellSizeScaled().Width;
	}
}

void CP437TextureFont::write(const wchar_t* val, SDL_Point point, SDL_Color col)
{
	for (int i = 0; i < wcslen(val); i++)
	{
		write(val[i], point, col);
		point.x += cellSizeScaled().Width;
	}
}

TTFTextureFont::TTFTextureFont(std::string name, SDL_Renderer* renderer, const char* assetPath, unsigned int fontSize)
{
	_name = name;
	_renderer = renderer;

	const wchar_t* CP437 = L" ☺☻♥♦♣♠•◘○◙♂♀♪♫☼►◄↕‼¶§▬↨↑↓→←∟↔▲▼ !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~⌂ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥₧ƒáíóúñÑªº¿⌐¬½¼¡«»░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■ ";

	_font = TTF_OpenFont(assetPath, fontSize);
	if (!_font)
		throw std::exception("TTF_OpenFont: The font asset could not be loaded.");

	_fontSize = fontSize;
	
	for (int i = 0; i < 256; i++)
	{
		Uint16 str[] = { CP437[i], '\0' };

		SDL_Surface* surface = nullptr;

		surface = TTF_RenderUNICODE_Blended(_font, str, { 255, 255, 255 });

		if (!surface)
			throw std::exception("TTF_RenderText...: The text could not be rendered.");

		//delete[] str;

		SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, surface);
		SDL_FreeSurface(surface);

		_glyphs[CP437[i]] = texture;

		int w, h;
		SDL_QueryTexture(texture, NULL, NULL, &w, &h);

		_glyphSizes[CP437[i]] = { w, h };

		if (i == 0) _cellSize = _glyphSizes[CP437[i]];
	}
}

TTFTextureFont::~TTFTextureFont()
{
	//delete _font;

	for (auto it = _glyphs.begin(); it != _glyphs.end(); it++)
		SDL_DestroyTexture(it->second);

	_renderer = nullptr;
}

int TTFTextureFont::fontSize()
{
	return _fontSize;
}

std::vector<std::string> TTFTextureFont::getLines(const char* val, int width)
{
	std::vector<std::string> lines;
	std::string vals = val;
	std::string line = "";
	int lineWidth = 0;

	for (int i = 0; i < vals.size(); i++)
	{
		line += vals[i];
		lineWidth += getWidth(vals.substr(i, 1).c_str());

		if (vals[i] != ' ') continue;

		int lineWidthExtra = 0;
		for (int j = i + 1; j < vals.size(); j++)
		{
			lineWidthExtra += getWidth(vals.substr(j, 1).c_str());

			if (vals[j] != ' ' && j < vals.size() - 1)
				continue;

			if (lineWidth + lineWidthExtra > width)
			{
				if (line.size() > 0)
				{
					lineWidth = 0;
					lines.push_back(line);
					line = "";
				}
			}

			break;
		}
	}

	if (line.size() > 0)
		lines.push_back(line);

	return lines;
}

std::vector<std::wstring> TTFTextureFont::getLines(const wchar_t* val, int width)
{
	std::vector<std::wstring> lines;
	std::wstring vals = val;
	std::wstring line = L"";
	int lineWidth = 0;

	for (int i = 0; i < vals.size(); i++)
	{
		line += vals[i];
		lineWidth += getWidth(vals.substr(i, 1).c_str());

		if (vals[i] != ' ') continue;

		int lineWidthExtra = 0;
		for (int j = i + 1; j < vals.size(); j++)
		{
			lineWidthExtra += getWidth(vals.substr(j, 1).c_str());

			if (vals[j] != ' ' && j < vals.size() - 1)
				continue;

			if (lineWidth + lineWidthExtra > width)
			{
				if (line.size() > 0)
				{
					lineWidth = 0;
					lines.push_back(line);
					line = L"";
				}
			}

			break;
		}
	}

	if (line.size() > 0)
		lines.push_back(line);

	return lines;
}

int TTFTextureFont::getHeight(const char* val, int width)
{
	return getLines(val, width).size() * _fontSize;
}

int TTFTextureFont::getWidth(const char val)
{
	return _glyphSizes[val].Width;
}

int TTFTextureFont::getWidth(const char* val)
{
	int result = 0;

	for (int i = 0; i < strlen(val); i++)
		result += getWidth(val[i]);

	return result;
}

int TTFTextureFont::getWidth(const char* val, int width)
{
	auto lines = getLines(val, width);
	int m = 0;
	for (auto l : lines)
		m = std::max(m, getWidth(l.c_str()));

	return m;
}

int TTFTextureFont::getWidth(const wchar_t val)
{
	return _glyphSizes[val].Width;
}

int TTFTextureFont::getWidth(const wchar_t* val)
{
	int result = 0;

	for (int i = 0; i < wcslen(val); i++)
		result += getWidth(val[i]);

	return result;
}

int TTFTextureFont::getWidth(const wchar_t* val, int width)
{
	auto lines = getLines(val, width);
	int m = 0;
	for (auto l : lines)
		m = std::max(m, getWidth(l.c_str()));

	return m;
}

void TTFTextureFont::write(const char val, SDL_Point point)
{
	auto tex = _glyphs[val];
	auto size = _glyphSizes[val];

	SDL_Rect srect;
	srect.x = 0;
	srect.y = 0;
	srect.w = size.Width;
	srect.h = size.Height;

	SDL_Rect drect;
	drect.x = point.x;
	drect.y = point.y;
	drect.w = size.Width;
	drect.h = size.Height;

	SDL_RenderCopy(_renderer, tex, &srect, &drect);
}

void TTFTextureFont::write(const char val, SDL_Point point, SDL_Color col)
{
	auto tex = _glyphs[val];
	auto size = _glyphSizes[val];

	SDL_Rect srect;
	srect.x = 0;
	srect.y = 0;
	srect.w = size.Width;
	srect.h = size.Height;

	SDL_Rect drect;
	drect.x = point.x;
	drect.y = point.y;
	drect.w = size.Width;
	drect.h = size.Height;

	SDL_SetTextureColorMod(tex, col.r, col.g, col.b);

	SDL_RenderCopy(_renderer, tex, &srect, &drect);

	SDL_SetTextureColorMod(tex, 255, 255, 255);
}

void TTFTextureFont::write(const char* val, SDL_Point point)
{
	for (int i = 0; i < strlen(val); i++)
	{
		write(val[i], point);
		point.x += getWidth(val[i]);
	}
}

void TTFTextureFont::write(const char* val, SDL_Point point, SDL_Color col)
{
	for (int i = 0; i < strlen(val); i++)
	{
		write(val[i], point, col);
		point.x += getWidth(val[i]);
	}
}

void TTFTextureFont::write(const char* val, SDL_Point point, int width)
{
	for (auto l : getLines(val, width))
	{
		write(l.c_str(), point);
		point.y += _fontSize;
	}
}

void TTFTextureFont::write(const wchar_t val, SDL_Point point)
{
	auto tex = _glyphs[val];
	auto size = _glyphSizes[val];

	SDL_Rect srect;
	srect.x = 0;
	srect.y = 0;
	srect.w = size.Width;
	srect.h = size.Height;

	SDL_Rect drect;
	drect.x = point.x;
	drect.y = point.y;
	drect.w = size.Width;
	drect.h = size.Height;

	SDL_RenderCopy(_renderer, tex, &srect, &drect);
}

void TTFTextureFont::write(const wchar_t val, SDL_Point point, SDL_Color col)
{
	auto tex = _glyphs[val];
	auto size = _glyphSizes[val];

	SDL_Rect srect;
	srect.x = 0;
	srect.y = 0;
	srect.w = size.Width;
	srect.h = size.Height;

	SDL_Rect drect;
	drect.x = point.x;
	drect.y = point.y;
	drect.w = size.Width;
	drect.h = size.Height;

	SDL_SetTextureColorMod(tex, col.r, col.g, col.b);

	SDL_RenderCopy(_renderer, tex, &srect, &drect);

	SDL_SetTextureColorMod(tex, 255, 255, 255);
}

void TTFTextureFont::write(const wchar_t* val, SDL_Point point)
{
	for (int i = 0; i < wcslen(val); i++)
	{
		write(val[i], point);
		point.x += getWidth(val[i]);
	}
}

void TTFTextureFont::write(const wchar_t* val, SDL_Point point, SDL_Color col)
{
	for (int i = 0; i < wcslen(val); i++)
	{
		write(val[i], point, col);
		point.x += getWidth(val[i]);
	}
}

void TTFTextureFont::write(const wchar_t* val, SDL_Point point, int width)
{
	for (auto l : getLines(val, width))
	{
		write(l.c_str(), point);
		point.y += _fontSize;
	}
}

