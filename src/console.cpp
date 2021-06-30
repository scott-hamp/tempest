#include "console.h"

const std::wstring Console::CP437 = L" ☺☻♥♦♣♠•◘○◙♂♀♪♫☼►◄↕‼¶§▬↨↑↓→←∟↔▲▼ !\"#$ % &\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~⌂ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥₧ƒáíóúñÑªº¿⌐¬½¼¡«»░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■ ";
Point2D Console::CursorPosition;
std::string Console::_debug;
int Console::_key;
Size2D Console::_size;
WINDOW* Console::_window;

void Console::clear()
{
	wclear(_window);
}

void Console::clearLine(int y, int x)
{
	std::string v = "";
	for (int i = x; i < _size.Width; i++) v += " ";

	write(y, x, v);
}

void Console::clearLines(int height)
{
	for (int i = 0; i < height; i++)
		clearLine(i);
}

void Console::clearLines(int y, int height)
{
	for (int i = y; i < y + height; i++)
		clearLine(i);
}

void Console::clearRect(Rect r)
{
	std::string v = "";
	for (int i = r.Position.X; i < r.Position.X + r.Size.Width; i++) v += " ";

	for (int y = r.Position.Y; y < r.Position.Y + r.Size.Height; y++)
		write(y, r.Position.X, v);
}

void Console::debug(std::string v)
{
	_debug = v;
}

void Console::end()
{
	delwin(_window);
	endwin();
}

void Console::fillRandomly()
{
	for (int y = 0; y < _size.Height; y++)
	{
		for (int x = 0; x < _size.Width; x++)
		{
			auto c = CP437[rand() % 256];
			write(y, x, c, rand() % 8);
		}
	}
}

Direction2D Console::getDirection()
{
	do
	{
		getKey();
		Direction2D dir = getDirection(_key);

		if (dir.isZero()) continue;

		return dir;
	} 
	while (true);
}

Direction2D Console::getDirection(int key)
{
	Direction2D dir = { 0, 0 };

	if (key == 104 || key == 260 || key == 52) dir.X = -1;
	if (key == 108 || key == 261 || key == 54) dir.X = 1;
	if (key == 107 || key == 259 || key == 56) dir.Y = -1;
	if (key == 106 || key == 258 || key == 50) dir.Y = 1;
	if (key == 121 || key == 55) { dir.X = -1; dir.Y = -1; }
	if (key == 117 || key == 57) { dir.X = 1; dir.Y = -1; }
	if (key == 98 || key == 49) { dir.X = -1; dir.Y = 1; }
	if (key == 110 || key == 51) { dir.X = 1; dir.Y = 1; }

	return dir;
}

int Console::getKey()
{
	noecho();
	nodelay(stdscr, FALSE);

	do
	{
		if ((_key = wgetch(_window)) == ERR) continue;

		return _key;
	} while (true);
}

const char Console::getLetter(bool lowercase, bool uppercase)
{
	if (!lowercase && !uppercase) return -1;

	do
	{
		getKey();

		if ((uppercase && (_key < 65 || _key > 90)) || (lowercase && (_key < 97 || _key > 122)))
			continue;

		return _key;
	} while (true);
}

int Console::getNumber()
{
	int cy = getcury(_window);
	int cx = getcurx(_window);

	do
	{
		auto str = getString();

		if (!(!str.empty() && std::all_of(str.begin(), str.end(), ::isdigit)))
		{
			std::string clr = "";
			for (int i = 0; i < str.length(); i++) clr += " ";
			write(cy, cx, clr);
			wmove(_window, cy, cx);

			continue;
		}

		return stoi(str);
	} while (true);
}

int Console::getNumberSingle()
{
	do
	{
		getKey();

		if (_key < 48 || _key > 57) continue;

		return _key;
	} while (true);
}

std::string Console::getString()
{
	echo();
	nodelay(stdscr, FALSE);

	std::string val = "";

	do
	{
		char buf[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
		auto res = &buf[0];
		if ((wgetstr(_window, res)) == ERR)
			continue;

		val = res;
		auto len = val.length();
		if (len == 0 || len > 100) continue;

		noecho();
		nodelay(stdscr, TRUE);

		return val;
	} while (true);
}

bool Console::getYesNo()
{
	do
	{
		getKey();

		if (_key != 121 && _key != 110) continue;

		return _key == 121;
	} while (true);
}

void Console::refresh()
{
	if (_debug.length() > 0) write(0, 0, "DEBUG: " + _debug);

	wmove(_window, CursorPosition.Y, CursorPosition.X);
	wrefresh(_window);
}

void Console::refreshKeyERR()
{
	while ((_key = getch()) == ERR) Console::refresh();
}

void Console::setCursor(int i)
{
	curs_set(i);
}

void Console::setup(int h, int w)
{
	_COORD coord;
	coord.X = w;
	coord.Y = h;

	_SMALL_RECT rect;
	rect.Top = 0;
	rect.Left = 0;
	rect.Bottom = h - 1;
	rect.Right = w - 1;

	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleScreenBufferSize(handle, coord);
	SetConsoleWindowInfo(handle, TRUE, &rect);

	srand(time(NULL));

	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);

	start_color();

	init_color(COLOR_WHITE, 999, 999, 999);
	init_color(COLOR_RED, 999, 50, 50);
	init_color(COLOR_GREEN, 50, 999, 50);
	init_color(COLOR_BLUE, 50, 50, 999);
	init_color(COLOR_YELLOW, 999, 999, 50);
	init_color(COLOR_MAGENTA, 999, 50, 999);
	init_color(COLOR_CYAN, 50, 999, 999);
	init_color(COLOR_GRAY, 500, 500, 500);

	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_BLACK);
	init_pair(4, COLOR_BLUE, COLOR_BLACK);
	init_pair(5, COLOR_YELLOW, COLOR_BLACK);
	init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(7, COLOR_CYAN, COLOR_BLACK);
	init_pair(8, COLOR_GRAY, COLOR_BLACK);

	setCursor(0);

	_window = newwin(h, w, 0, 0);
	_size = { w, h };
	_key = -1;
	_debug = "";
	CursorPosition = { 0, 0 };
}

Size2D Console::size() { return _size; }

void Console::wait(int ms)
{
	auto dur = std::chrono::milliseconds(ms);
	std::this_thread::sleep_for(dur);
}

bool Console::windowsSetFont(const wchar_t* fn)
{
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof cfi;
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;
	cfi.dwFontSize.Y = 20;
	cfi.FontFamily = FF_MODERN;
	cfi.FontWeight = FW_NORMAL;
	
	wcscpy_s(cfi.FaceName, fn);
	return SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

void Console::write(int y, int x, const char chr)
{
	mvwaddch(_window, y, x, chr);
}

void Console::write(int y, int x, const char chr, int cp)
{
	wattron(_window, COLOR_PAIR(cp));
	mvwaddch(_window, y, x, chr);
	wattroff(_window, COLOR_PAIR(cp));
}

void Console::write(int y, int x, const chtype ct)
{
	mvwaddch(_window, y, x, ct);
}

void Console::write(int y, int x, const chtype ct, int cp)
{
	wattron(_window, COLOR_PAIR(cp));
	mvwaddch(_window, y, x, ct);
	wattroff(_window, COLOR_PAIR(cp));
}

void Console::write(int y, int x, const char* str)
{
	mvwaddstr(_window, y, x, str);
}

void Console::write(int y, int x, const char* str, int cp)
{
	wattron(_window, COLOR_PAIR(cp));
	mvwaddstr(_window, y, x, str);
	wattroff(_window, COLOR_PAIR(cp));
}

void Console::write(int y, int x, std::string str)
{
	mvwaddstr(_window, y, x, str.c_str());
}

void Console::write(int y, int x, std::string str, int cp)
{
	wattron(_window, COLOR_PAIR(cp));
	mvwaddstr(_window, y, x, str.c_str());
	wattroff(_window, COLOR_PAIR(cp));
}

void Console::write(int y, int x, const wchar_t chr)
{
	wmove(_window, y, x);
	waddrawch(_window, chr);
}

void Console::write(int y, int x, const wchar_t chr, int cp)
{
	wmove(_window, y, x);
	wattron(_window, COLOR_PAIR(cp));
	waddrawch(_window, chr);
	wattroff(_window, COLOR_PAIR(cp));
}

void Console::write(int y, int x, const wchar_t* str)
{
	for (int i = 0; i < wcslen(str); i++)
		write(y, x + i, str[i]);
}

void Console::write(int y, int x, const wchar_t* str, int cp)
{
	for (int i = 0; i < wcslen(str); i++)
		write(y, x + i, str[i], cp);
}

void Console::write(int y, int x, std::wstring str)
{
	for (int i = 0; i < str.length(); i++)
		write(y, x + i, str[i]);
}

void Console::write(int y, int x, std::wstring str, int cp)
{
	for (int i = 0; i < str.length(); i++)
		write(y, x + i, str[i], cp);
}