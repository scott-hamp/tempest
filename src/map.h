#pragma once

#ifndef Map_H
#define Map_H

#include <vector>
#include "console.h"
#include "data.h"
#include "dice.h"
#include "enums.h"
#include "map-tile.h"
#include "map-object.h"
#include "ui.h"

class Path;

class Map
{
	private:
		static int _depth;
		static Point2D _drawOffset;
		static std::vector<MapObject*> _objects;
		static MapObject* _player;
		static Size2D _size;
		static std::vector<MapTile*> _tiles;
	public:
		static void changeDepth(int);
		static bool contains(Point2D);
		static MapObject* createObject(std::wstring);
		static Point2D positionFromConsolePosition(Point2D);
		static Point2D positionToConsolePosition(Point2D);
		static void draw();
		static Path* findPath(Point2D, Point2D);
		static void generate();
		static std::string getTileDescription(Point2D, bool = false);
		static int getTileLight(Point2D);
		static MapObject* getTileObject(Point2D, int = 0);
		static bool getTilePassable(Point2D, PassableType);
		static void moveObject(MapObject*, Point2D);
		static void nextTurn();
		static void objectTryInteraction(MapObject*, MapObjectInteraction);
		static void objectTryInteraction(MapObject*, MapObjectInteraction, Direction2D);
		static void objectTryInteraction(MapObject*, MapObjectInteraction, MapObject*);
		static void placeObject(MapObject*, Point2D);
		static MapObject* player();
		static void removeObject(MapObject*);
		static void setTile(Point2D, MapTileType);
		static void setTile(Point2D, MapTileTerrain);
		static void setTile(Point2D, int);
		static void setup(Size2D, Point2D);
		static void updateObjectView(MapObject*);
};

class Path
{
	private:
		bool _isComplete;
		std::vector<Point2D> _nodes;
	public:
		Path();
		~Path();
		void add(Point2D);
		void complete();
		bool contains(Point2D);
		Point2D get(int);
		bool isComplete();
		int size();
};

#endif