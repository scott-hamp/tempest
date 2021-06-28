#include "data.h"

std::vector<std::wstring> Data::getObject(std::wstring objectID)
{
    std::locale::global(std::locale("en_US.utf8"));
    std::wifstream wf("data/map-objects.txt");
    std::wstring wstr;

    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_WTEXT);
    #else
        std::setlocale(LC_ALL, "");
    #endif 

    std::vector<std::wstring> lines;
    for (wchar_t c; wf.get(c); )
    {
        if (c == L'\n')
        {
            lines.push_back(wstr);
            wstr = L"";

            continue;
        }

        wstr += c;
    }

    if (wstr.length() > 0) lines.push_back(wstr);
    
    bool toBreak = false;
    std::vector<std::wstring> objectLines;

    for (int i = 0; i < lines.size(); i++)
    {
        if (lines[i].compare(objectID) != 0) continue;

        for (int j = i; j < lines.size(); j++)
        {
            if (lines[j].length() == 0)
            {
                toBreak = true;
                break;
            }

            objectLines.push_back(lines[j]);
        }

        if (toBreak) break;
    }

    return objectLines;
}