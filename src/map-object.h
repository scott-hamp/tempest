#pragma once

#ifndef MapObject_H
#define MapObject_H

#include <string>
#include <vector>
#include "enums.h"
#include "geometry.h"

class MapObjectBehaviorProperty
{
	private:
		std::wstring _key;
		std::wstring _value;
	public:
		MapObjectBehaviorProperty(std::wstring, std::wstring);
		~MapObjectBehaviorProperty();
		std::wstring key();
		std::wstring value();
};

class MapObjectBehavior
{
	private:
		std::wstring _name;
		std::vector<MapObjectBehaviorProperty*> _properties;
	public:
		MapObjectBehavior(std::wstring);
		~MapObjectBehavior();
		void addProperty(MapObjectBehaviorProperty*);
		std::wstring getPropertyValue(std::wstring);
		std::wstring name();
};

class MapObjectView
{
	public:
		MapObjectView(int, wchar_t);
		~MapObjectView();
		int State;
		wchar_t Chr;
};

class MapObject
{
	private:
		std::vector<MapObjectBehavior*> _behaviors;
		int _turnActions;
		Point2D _position;
	public:
		MapObject();
		~MapObject();
		void addBehavior(MapObjectBehavior*);
		void clearView();
		void createView(Size2D);
		std::wstring getBehaviorProperty(std::wstring, std::wstring);
		MapObjectView* getView(Point2D, int);
		bool hasBehavior(std::wstring);
		bool isPassable(PassableType);
		void nextTurn();
		Point2D position();
		void setPosition(Point2D);
		void setView(Point2D, int, int);
		void setView(Point2D, int, int, wchar_t);
		void takeTurnAction();
		bool turnActionRemaining();
		std::vector<MapObjectView*> _view;
};

#endif