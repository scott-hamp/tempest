#pragma once

#ifndef Strings_H
#define Strings_H

#include <algorithm>
#include <string>
#include <vector>

class Strings
{
	public:
		static std::string from(std::wstring);
		static std::vector<std::string> split(std::string, char);
		static std::vector<std::wstring> split(std::wstring, wchar_t);
		static std::string toLower(std::string);
		static std::string toTitle(std::string, bool = false);
		static std::string toUpper(std::string);
		static std::string trim(std::string);
};

#endif