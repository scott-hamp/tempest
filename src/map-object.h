#pragma once

#ifndef MapObject_H
#define MapObject_H

#include <map>
#include <string>
#include <vector>
#include "enums.h"
#include "geometry.h"
#include "random.h"
#include "strings.h"

class MapObjectBehaviorProperty
{
	private:
		std::wstring _key;
		std::wstring _value;
	public:
		MapObjectBehaviorProperty(std::wstring, std::wstring);
		~MapObjectBehaviorProperty();
		std::wstring getEffectValue(std::wstring);
		std::wstring key();
		void setEffectValue(std::wstring, std::wstring);
		void setValue(std::wstring);
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
		std::wstring getPropertyEffectValue(std::wstring, std::wstring);
		std::wstring getPropertyValue(std::wstring);
		std::wstring name();
		void setPropertyEffectValue(std::wstring, std::wstring, std::wstring);
		void setPropertyValue(std::wstring, std::wstring);
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
		std::vector<MapObjectView*> _view;
	public:
		MapObject();
		~MapObject();
		void addBehavior(MapObjectBehavior*);
		void addToInventory(MapObject*);
		void clearView();
		void createEquipmentSlots();
		void createView(Size2D);
		std::wstring getBehaviorProperty(std::wstring, std::wstring);
		std::wstring getBehaviorPropertyEffectValue(std::wstring, std::wstring, std::wstring);
		MapObject* getEquipment(std::wstring);
		std::vector<std::wstring> getEquipmentSlots();
		std::vector<std::wstring> getEquippedEffect(std::wstring);
		std::wstring getEquippedSlot(MapObject*);
		MapObject* getInventory(int);
		int getInventorySize();
		std::wstring getStatsSummary();
		MapObjectView* getView(Point2D, int);
		bool hasBehavior(std::wstring);
		bool hasItem(MapObject*);
		bool isPassable(PassableType);
		void nextTurn();
		Point2D position();
		void removeFromInventory(MapObject*);
		void resetTurnActions();
		void setPosition(Point2D);
		void setBehaviorProperty(std::wstring, std::wstring, std::wstring);
		void setBehaviorPropertyEffectValue(std::wstring, std::wstring, std::wstring, std::wstring);
		void setView(Point2D, int, int);
		void setView(Point2D, int, int, wchar_t);
		void takeDamage(int);
		void takeTurnAction();
		void tryInteraction(MapObjectInteraction);
		void tryInteraction(MapObjectInteraction, MapObject*);
		bool turnActionsRemaining();
		void updateEquipmentEffects();
};

#endif