#include "map-tile.h"

MapTile::MapTile(MapTileType type, MapTileTerrain terrain, int light) 
{ 
	_type = type; 
	_terrain = terrain;
	_light = light;
}

MapTile::~MapTile() { }

void MapTile::addObject(MapObject* object)
{
	if (std::find(_objects.begin(), _objects.end(), object) != _objects.end())
		return;

	_objects.insert(_objects.begin(), object);
}

wchar_t MapTile::getChr() 
{
	if (_objects.size() > 0) 
		return object()->getBehaviorProperty(L"visual", L"chr")[0];

	if (_type == MapTileType_Floor) return L'.';
	if (_type == MapTileType_Wall) return L'▓';

	return L' ';
}

std::string MapTile::getColorBG()
{
	if (_objects.size() > 0)
		return Strings::from(object()->getBehaviorProperty(L"visual", L"color-bg"));

	if (_type == MapTileType_Wall) return { 0, 0, 0, 0};

	return "background";
}

std::string MapTile::getColorFG()
{
	if (_objects.size() > 0)
		return Strings::from(object()->getBehaviorProperty(L"visual", L"color-fg"));

	if (_terrain == MapTileTerrain_Grass) return "bright-green";

	return "bright-gray";
}

std::string MapTile::getDescription(bool longDescription)
{
	if (object() != nullptr)
	{
		auto bpkey = (longDescription) ? L"long" : L"short";
		auto wstr = object()->getBehaviorProperty(L"description", bpkey);
		std::string str(wstr.begin(), wstr.end());

		return str;
	}

	if (longDescription)
	{
		if (_type == MapTileType_Floor)
		{
			if (_terrain == MapTileTerrain_Grass) return "The ground. It's covered in grass.";
			if (_terrain == MapTileTerrain_Stone) return "The floor. It's made of stone.";
		}

		if (_type == MapTileType_Wall) return "The wall. It's made of stone.";
	}

	if (_type == MapTileType_Floor)
	{
		if (_terrain == MapTileTerrain_Grass) return "The ground.";

		return "The floor";
	}

	if (_type == MapTileType_Wall) return "The wall.";

	return "";
}

bool MapTile::isPassable(PassableType passableType)
{
	if (_objects.size() > 0) return object()->isPassable(passableType);

	return _type != MapTileType_Empty && _type != MapTileType_Wall;
}

int MapTile::light() { return _light; }

MapObject* MapTile::object(int i)
{
	if (_objects.size() <= i) return nullptr;

	return _objects[i];
}

void MapTile::removeObject(MapObject* object)
{
	if (std::find(_objects.begin(), _objects.end(), object) == _objects.end())
		return;

	int ind = -1;
	for (int i = 0; i < _objects.size(); i++)
	{
		if (_objects[i] != object) continue;

		ind = i;
		break;
	}

	_objects.erase(_objects.begin() + ind);
}

void MapTile::removeObjects()
{
	_objects.erase(_objects.begin(), _objects.end());
}

void MapTile::set(MapTileType type) { _type = type; }

void MapTile::set(MapTileTerrain terrain) { _terrain = terrain; }

void MapTile::set(int light) { _light = light; }

MapTileTerrain MapTile::terrain() { return _terrain; }

MapTileType MapTile::type() { return _type; }