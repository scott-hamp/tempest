#pragma once

#ifndef Data_H
#define Data_H

#include <codecvt>
#include <fcntl.h>
#include <fstream>
#include <io.h>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <Windows.h>
#include "strings.h"

class Data
{
	public:
		static std::vector<std::wstring> getObject(std::wstring);
};

#endif