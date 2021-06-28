#pragma once

#ifndef MapTile_H
#define MapTile_H

#include <vector>
#include "enums.h"
#include "map-object.h"

class MapTile
{
	private:
		std::vector<MapObject*> _objects;
		MapTileTerrain _terrain;
		MapTileType _type;
	public:
		MapTile(MapTileType, MapTileTerrain);
		~MapTile();
		void addObject(MapObject*);
		wchar_t getChr();
		int getColorPair();
		std::string getDescription(bool = false);
		bool isPassable(PassableType);
		MapObject* object(int = 0);
		void removeObject(MapObject*);
		void removeObjects();
		void set(MapTileTerrain);
		void set(MapTileType);
		MapTileTerrain terrain();
		MapTileType type();
};

#endif