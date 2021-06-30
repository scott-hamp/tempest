#pragma once

#ifndef MapObject_H
#define MapObject_H

#include <map>
#include <string>
#include <vector>
#include "console.h"
#include "enums.h"
#include "geometry.h"
#include "strings.h"

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
		std::map<std::wstring, MapObject*> _equipment;
		std::vector<MapObject*> _inventory;
		Point2D _position;
		int _turnActions;
	public:
		MapObject();
		~MapObject();
		void addBehavior(MapObjectBehavior*);
		void addToInventory(MapObject*);
		void clearView();
		void createEquipmentSlots();
		void createView(Size2D);
		std::wstring getBehaviorProperty(std::wstring, std::wstring);
		MapObject* getEquipment(std::wstring);
		std::vector<std::wstring> getEquipmentSlots();
		std::vector<std::wstring> getEquippedEffect(std::wstring);
		std::wstring getEquippedSlot(MapObject*);
		MapObject* getInventory(int);
		int getInventorySize();
		MapObjectView* getView(Point2D, int);
		bool hasBehavior(std::wstring);
		bool hasItem(MapObject*);
		bool isPassable(PassableType);
		void nextTurn();
		Point2D position();
		void removeFromInventory(MapObject*);
		void setPosition(Point2D);
		void setView(Point2D, int, int);
		void setView(Point2D, int, int, wchar_t);
		void takeTurnAction();
		void tryInteraction(MapObjectInteraction);
		void tryInteraction(MapObjectInteraction, MapObject*);
		bool turnActionRemaining();
		std::vector<MapObjectView*> _view;
};

#endif