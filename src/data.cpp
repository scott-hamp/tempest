#include "data.h"

std::vector<std::wstring> Data::_mapObjectsLines;

std::vector<std::string> Data::getAudio()
{
    std::vector<std::string> lines;
    std::string line;

    std::ifstream audioFile("data/audio.txt");
    if (audioFile.is_open())
    {
        while (getline(audioFile, line))
            lines.push_back(line);

        audioFile.close();
    }

    return lines;
}

std::vector<std::string> Data::getFonts()
{
    std::vector<std::string> lines;
    std::string line;

    std::ifstream fontsFile("data/fonts.txt");
    if (fontsFile.is_open())
    {
        while (getline(fontsFile, line))
            lines.push_back(line);

        fontsFile.close();
    }

    return lines;
}

std::vector<std::wstring> Data::getObject(std::wstring objectID)
{
    bool toBreak = false;
    std::vector<std::wstring> objectLines;

    for (int i = 0; i < _mapObjectsLines.size(); i++)
    {
        if (_mapObjectsLines[i].compare(objectID) != 0) continue;

        for (int j = i; j < _mapObjectsLines.size(); j++)
        {
            if (_mapObjectsLines[j].length() == 0)
            {
                toBreak = true;
                break;
            }

            objectLines.push_back(_mapObjectsLines[j]);
        }

        if (toBreak) break;
    }

    return objectLines;
}

std::vector<std::wstring> Data::getObjectList(std::wstring listName)
{
    std::vector<std::wstring> empty;

    for (int i = 0; i < _mapObjectsLines.size(); i++)
    {
        if (_mapObjectsLines[i].length() == 0) continue;
        if (_mapObjectsLines[i][0] != L'@') continue;

        auto parts = Strings::split(_mapObjectsLines[i].substr(1), L':');
        if (parts[0].compare(listName) != 0) continue;

        return Strings::split(parts[1], L',');
    }

    return empty;
}

std::vector<std::string> Data::getPalette()
{
    std::vector<std::string> lines;
    std::string line;

    std::ifstream paletteFile("data/palette.txt");
    if (paletteFile.is_open())
    {
        while (getline(paletteFile, line))
            lines.push_back(line);

        paletteFile.close();
    }

    return lines;
}

void Data::load()
{
    std::locale::global(std::locale("en_US.utf8"));
    std::wifstream wf("data/map-objects.txt");
    std::wstring wstr;

    /*
    #ifdef _WIN32
        _setmode(_fileno(stdout), _O_WTEXT);
    #else
        std::setlocale(LC_ALL, "");
    #endif 
    */

    for (wchar_t c; wf.get(c); )
    {
        if (c == L'\n')
        {
            _mapObjectsLines.push_back(wstr);
            wstr = L"";

            continue;
        }

        wstr += c;
    }

    if (wstr.length() > 0) _mapObjectsLines.push_back(wstr);
}