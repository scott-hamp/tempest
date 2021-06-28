#include "strings.h"

std::vector<std::string> Strings::split(std::string str, char del)
{
	std::string tmp = "";
	std::vector<std::string> res;

	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == del)
		{
			if (tmp.length() == 0) continue;

			res.push_back(tmp);
			tmp = "";

			continue;
		}

		tmp += str[i];
	}

	if (tmp.length() > 0) res.push_back(tmp);

	return res;
}

std::vector<std::wstring> Strings::split(std::wstring str, wchar_t del)
{
	std::wstring tmp = L"";
	std::vector<std::wstring> res;

	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == del)
		{
			if (tmp.length() == 0) continue;

			res.push_back(tmp);
			tmp = L"";

			continue;
		}

		tmp += str[i];
	}

	if (tmp.length() > 0) res.push_back(tmp);

	return res;
}

std::string Strings::toLower(std::string str)
{
	std::for_each(str.begin(), str.end(), [](char& c) {
		c = ::tolower(c);
	});

	return str;
}

std::string Strings::toTitle(std::string str, bool firstWordOnly)
{
	if (firstWordOnly)
	{
		std::for_each(str.begin(), str.begin() + 1, [](char& c) {
			c = ::toupper(c);
		});
	}

	return str;
}

std::string Strings::toUpper(std::string str)
{
	std::for_each(str.begin(), str.end(), [](char& c) {
		c = ::toupper(c);
	});

	return str;
}

std::string Strings::trim(std::string str)
{
	if (!str.empty() && str[str.length() - 1] == '\n')
		str.erase(str.length() - 1);

	int front = 0; int back = str.length() - 1;

	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == ' ') continue;

		front = i;
		break;
	}

	for (int i = str.length() - 1; i > front; i--)
	{
		if (str[i] == ' ') continue;

		back = i;
		break;
	}

	std::string res = "";
	for (int i = front; i <= back; i++) res += str[i];

	return res;
}