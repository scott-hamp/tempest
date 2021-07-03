#pragma once

#ifndef Data_H
#define Data_H

#include <codecvt>
#include <fcntl.h>
#include <fstream>
#include <io.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <Windows.h>
#include "strings.h"

class Data
{
	private:
		static std::vector<std::wstring> _mapObjectsLines;
	public:
		static std::vector<std::wstring> getObject(std::wstring);
		static std::vector<std::wstring> getObjectList(std::wstring);
		static std::vector<std::string> getPalette();
		static void load();
};

#endif