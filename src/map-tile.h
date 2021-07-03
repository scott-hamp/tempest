#pragma once

#ifndef MapTile_H
#define MapTile_H

#include <vector>
#include "enums.h"
#include "map-object.h"

class MapTile
{
	private:
		int _light;
		std::vector<MapObject*> _objects;
		MapTileTerrain _terrain;
		MapTileType _type;
	public:
		MapTile(MapTileType, MapTileTerrain, int);
		~MapTile();
		void addObject(MapObject*);
		wchar_t getChr();
		std::string getColorBG();
		std::string getColorFG();
		std::string getDescription(bool = false);
		bool isPassable(PassableType);
		int light();
		MapObject* object(int = 0);
		void removeObject(MapObject*);
		void removeObjects();
		void set(MapTileTerrain);
		void set(MapTileType);
		void set(int);
		MapTileTerrain terrain();
		MapTileType type();
};

#endif