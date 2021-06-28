#include "map-object.h"

MapObjectBehaviorProperty::MapObjectBehaviorProperty(std::wstring key, std::wstring value)
{
	_key = key;
	_value = value;
}

MapObjectBehaviorProperty::~MapObjectBehaviorProperty() { }

std::wstring MapObjectBehaviorProperty::key() { return _key; }

std::wstring MapObjectBehaviorProperty::value() { return _value; }


MapObjectBehavior::MapObjectBehavior(std::wstring name) { _name = name; }

MapObjectBehavior::~MapObjectBehavior() { }

void MapObjectBehavior::addProperty(MapObjectBehaviorProperty* prop)
{
	_properties.push_back(prop);
}

std::wstring MapObjectBehavior::getPropertyValue(std::wstring key)
{
	for (const auto &prop : _properties)
	{
		if (prop->key().compare(key) != 0) continue;

		return prop->value();
	}

	return L"";
}

std::wstring MapObjectBehavior::name() { return _name; }


MapObjectView::MapObjectView(int state, wchar_t chr)
{
	State = state;
	Chr = chr;
}

MapObjectView::~MapObjectView() { }


MapObject::MapObject() { }

MapObject::~MapObject() { }

void MapObject::addBehavior(MapObjectBehavior* behavior)
{
	_behaviors.push_back(behavior);
}

void MapObject::clearView()
{
	for (int i = 0; i < _view.size(); i++)
	{
		_view[i]->State = 0;
		_view[i]->Chr = L' ';
	}
}

void MapObject::createView(Size2D size)
{
	if (_view.size() > 0) return;

	for (int i = 0; i < size.Width * size.Height; i++)
		_view.push_back(new MapObjectView(0, L' '));
}

std::wstring MapObject::getBehaviorProperty(std::wstring behaviorName, std::wstring propertyKey)
{
	for (const auto& behavior : _behaviors)
	{
		if(behavior->name().compare(behaviorName) != 0) continue;

		return behavior->getPropertyValue(propertyKey);
	}

	return L"";
}

MapObjectView* MapObject::getView(Point2D point, int width)
{
	return _view[(point.Y * width) + point.X];
}

bool MapObject::hasBehavior(std::wstring behaviorName)
{
	for (const auto& behavior : _behaviors)
	{
		if (behavior->name().compare(behaviorName) != 0) continue;

		return true;
	}

	return false;
}

bool MapObject::isPassable(PassableType passableType)
{
	if (!hasBehavior(L"passable")) return false;

	if (passableType == PassableType_Solid)
		return getBehaviorProperty(L"passable", L"solid").compare(L"true") == 0;

	if (passableType == PassableType_Light)
		return getBehaviorProperty(L"passable", L"light").compare(L"true") == 0;

	return false;
}

void MapObject::nextTurn()
{
	_turnActions = 1;
}

Point2D MapObject::position() { return _position; }

void MapObject::setPosition(Point2D point) { _position = point; }

void MapObject::setView(Point2D point, int width, int state)
{
	auto v = _view[(point.Y * width) + point.X];
	v->State = state;
}

void MapObject::setView(Point2D point, int width, int state, wchar_t chr)
{
	auto v = _view[(point.Y * width) + point.X];
	v->State = state;
	v->Chr = chr;
}

void MapObject::takeTurnAction()
{
	if (turnActionRemaining()) _turnActions--;
}

bool MapObject::turnActionRemaining() { return _turnActions > 0; }