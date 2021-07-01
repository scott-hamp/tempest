#include "map-object.h"

MapObjectBehaviorProperty::MapObjectBehaviorProperty(std::wstring key, std::wstring value)
{
	_key = key;
	_value = value;
}

MapObjectBehaviorProperty::~MapObjectBehaviorProperty() { }

std::wstring MapObjectBehaviorProperty::key() { return _key; }

void MapObjectBehaviorProperty::setValue(std::wstring value) 
{ 
	_value = value;
}

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

void MapObjectBehavior::setPropertyValue(std::wstring key, std::wstring value)
{
	for (const auto& prop : _properties)
	{
		if (prop->key().compare(key) != 0) continue;

		prop->setValue(value);
		return;
	}
}


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

void MapObject::addToInventory(MapObject* object)
{
	if (object == nullptr || !hasBehavior(L"has-inventory"))
		return;
	if (!object->hasBehavior(L"item")) return;
	if (std::find(_inventory.begin(), _inventory.end(), object) != _inventory.end())
		return;

	_inventory.push_back(object);
}

void MapObject::clearView()
{
	for (int i = 0; i < _view.size(); i++)
	{
		_view[i]->State = 0;
		_view[i]->Chr = L' ';
	}
}

void MapObject::createEquipmentSlots()
{
	_equipment[L"on head"] = nullptr;
	_equipment[L"on face"] = nullptr;
	_equipment[L"on shoulders"] = nullptr;
	_equipment[L"on chest"] = nullptr;
	_equipment[L"on hands"] = nullptr;
	_equipment[L"in right hand"] = nullptr;
	_equipment[L"in left hand"] = nullptr;
	_equipment[L"on right ring finger"] = nullptr;
	_equipment[L"on left ring finger"] = nullptr;
	_equipment[L"on legs"] = nullptr;
	_equipment[L"on feet"] = nullptr;
	_equipment[L"about body"] = nullptr;
	_equipment[L"as light source"] = nullptr;
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

MapObject* MapObject::getEquipment(std::wstring slot)
{
	if (!hasBehavior(L"has-equipment") || !hasBehavior(L"has-inventory"))
		return nullptr;

	for (std::map<std::wstring, MapObject*>::iterator iter = _equipment.begin(); iter != _equipment.end(); ++iter)
	{
		if (iter->first.compare(slot) != 0) continue;

		return iter->second;
	}

	return nullptr;
}

std::vector<std::wstring> MapObject::getEquipmentSlots()
{
	std::vector<std::wstring> result;

	if (!hasBehavior(L"has-equipment") || !hasBehavior(L"has-inventory")) 
		return result;

	for (std::map<std::wstring, MapObject*>::iterator iter = _equipment.begin(); iter != _equipment.end(); ++iter)
		result.push_back(iter->first);

	return result;
}

std::wstring MapObject::getEquippedSlot(MapObject* item)
{
	if (!hasItem(item) || !hasBehavior(L"has-equipment") || !item->hasBehavior(L"equipment"))
		return L"";

	for (std::map<std::wstring, MapObject*>::iterator iter = _equipment.begin(); iter != _equipment.end(); ++iter)
	{
		auto k = iter->first;
		auto v = iter->second;

		if (v != item) continue;

		return k;
	}

	return L"";
}

std::vector<std::wstring> MapObject::getEquippedEffect(std::wstring effect)
{
	std::vector<std::wstring> result;

	if (!hasBehavior(L"has-equipment") || !hasBehavior(L"has-inventory")) 
		return result;

	for (std::map<std::wstring, MapObject*>::iterator iter = _equipment.begin(); iter != _equipment.end(); ++iter)
	{
		auto item = iter->second;
		if (item == nullptr) continue;

		auto effects = item->getBehaviorProperty(L"equipment", L"equipped-effects");

		if (effects.length() == 0) continue;

		auto effectsSplit = Strings::split(effects, L';');

		for (auto e : effectsSplit)
		{
			auto parts = Strings::split(e, L':');

			if (parts.size() != 2) continue;
			if (parts[0].compare(effect) != 0) continue;

			result.push_back(e);
		}
	}

	return result;
}

MapObject* MapObject::getInventory(int index)
{
	if (index >= _inventory.size()) return nullptr;

	return _inventory[index];
}

int MapObject::getInventorySize()
{
	return _inventory.size();
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

bool MapObject::hasItem(MapObject* item)
{
	if (!hasBehavior(L"has-inventory") || !item->hasBehavior(L"item")) 
		return false;

	return std::find(_inventory.begin(), _inventory.end(), item) != _inventory.end();
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

void MapObject::removeFromInventory(MapObject* object)
{
	if (object == nullptr || !hasBehavior(L"has-inventory")) 
		return;
	if (!object->hasBehavior(L"item")) return;
	if (std::find(_inventory.begin(), _inventory.end(), object) == _inventory.end())
		return;

	auto ind = -1;
	for (int i = 0; i < _inventory.size(); i++)
	{
		if (_inventory[i] != object) continue;

		ind = i;
		break;
	}

	_inventory.erase(_inventory.begin() + ind);
}

void MapObject::tryInteraction(MapObjectInteraction interaction)
{

}

void MapObject::tryInteraction(MapObjectInteraction interaction, MapObject* other)
{
	if (interaction == MapObjectInteraction_WearWield)
	{
		if (!hasBehavior(L"has-equipment") || !other->hasBehavior(L"item"))
			return;

		auto eqat = other->getBehaviorProperty(L"equipment", L"equip-at");
		if (eqat.length() == 0) return;

		takeTurnAction();

		if (getEquippedSlot(other) != L"")
			_equipment[eqat] = nullptr;
		else
			_equipment[eqat] = other;

		updateEquipmentEffects();

		return;
	}
}

void MapObject::setBehaviorProperty(std::wstring behaviorName, std::wstring propertyKey, std::wstring propertyValue)
{
	for (const auto& behavior : _behaviors)
	{
		if (behavior->name().compare(behaviorName) != 0) continue;

		behavior->setPropertyValue(propertyKey, propertyValue);
		return;
	}
}

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

void MapObject::takeDamage(int damage)
{
	auto hp = stoi(getBehaviorProperty(L"hp", L"value"));
	hp -= damage;
	setBehaviorProperty(L"hp", L"value", std::to_wstring(hp));
}

void MapObject::takeTurnAction()
{
	if (turnActionRemaining()) _turnActions--;
}

bool MapObject::turnActionRemaining() { return _turnActions > 0; }

void MapObject::updateEquipmentEffects()
{
	auto viewPower = stoi(getBehaviorProperty(L"view", L"power-default"));

	int viewPowerMod = 0;
	auto effects = getEquippedEffect(L"view-power");
	for (auto e : effects)
		viewPowerMod += stoi(Strings::split(e, L':')[1]);

	viewPower += viewPowerMod;
	setBehaviorProperty(L"view", L"power", std::to_wstring(viewPower));
}