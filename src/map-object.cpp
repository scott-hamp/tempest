#include "map-object.h"

MapObjectBehaviorProperty::MapObjectBehaviorProperty(std::wstring key, std::wstring value)
{
	_key = key;
	_value = value;
}

MapObjectBehaviorProperty::~MapObjectBehaviorProperty() { }

std::wstring MapObjectBehaviorProperty::getEffectValue(std::wstring effectName)
{
	auto effects = Strings::split(_value, L';');

	for (auto eff : effects)
	{
		auto parts = Strings::split(eff, L':');
		if (parts.size() != 2) continue;
		if (parts[0].compare(effectName) != 0) continue;

		return parts[1];
	}

	return L"";
}

void MapObjectBehaviorProperty::setEffectValue(std::wstring effectName, std::wstring effectValue)
{
	auto effects = Strings::split(_value, L';');
	std::wstring valueNew = L"";

	for (int i = 0; i < effects.size(); i++)
	{
		auto parts = Strings::split(effects[i], L':');
		if (parts.size() != 2) return;
		if (parts[0].compare(effectName) != 0)
		{
			std::wstring end = (i < effects.size() - 1) ? L";" : L"";
			valueNew += parts[0] + L":" + parts[1] + end;
			continue;
		}

		std::wstring end = (i < effects.size() - 1) ? L";" : L"";
		valueNew += parts[0] + L":" + effectValue + end;
	}

	_value = valueNew;
}

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

std::wstring MapObjectBehavior::getPropertyEffectValue(std::wstring key, std::wstring effectName)
{
	for (const auto& prop : _properties)
	{
		if (prop->key().compare(key) != 0) continue;

		return prop->getEffectValue(effectName);
	}

	return L"";
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

void MapObjectBehavior::setPropertyEffectValue(std::wstring key, std::wstring effectName, std::wstring effectValue)
{
	for (const auto& prop : _properties)
	{
		if (prop->key().compare(key) != 0) continue;

		prop->setEffectValue(effectName, effectValue);
		return;
	}
}

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
	_equipment[L"as cloak"] = nullptr;
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

std::wstring MapObject::getBehaviorPropertyEffectValue(std::wstring behaviorName, std::wstring propertyKey, std::wstring effectName)
{
	for (const auto& behavior : _behaviors)
	{
		if (behavior->name().compare(behaviorName) != 0) continue;

		return behavior->getPropertyEffectValue(propertyKey, effectName);
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

std::wstring MapObject::getStatsSummary()
{
	std::wstring result = L"";

	if (hasBehavior(L"equipment"))
	{
		auto effects = Strings::split(getBehaviorProperty(L"equipment", L"equipped-effects"), L';');

		for (auto eff : effects)
		{
			auto parts = Strings::split(eff, L':');
			if (result.length() > 0) result += L", ";

			if (parts[0].compare(L"ac") == 0) result += L"♦";
			if (parts[0].compare(L"to-hit") == 0) result += L"→";
			if (parts[0].compare(L"view-power") == 0) result += L"light: ";

			result += parts[1];

			if (parts[0].compare(L"duration") == 0) result += L" turns";
		}
	}

	return result;
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
	auto speedStr = getBehaviorProperty(L"speed", L"value");
	if (speedStr.length() > 0)
	{
		auto speed = stoi(speedStr);
		_turnActions = 1 + Random::nextInt(0, speed);
	}
	else
		_turnActions = 1;

	if (hasBehavior(L"has-inventory"))
	{
		for (auto const& item : _inventory)
		{
			if (!item->hasBehavior(L"equipment")) continue;
			auto equippedAt = getEquippedSlot(item);
			if (equippedAt.length() == 0) continue;

			auto durationStr = item->getBehaviorPropertyEffectValue(L"equipment", L"equipped-effects", L"duration");
			if (durationStr.length() == 0) continue;

			auto duration = stoi(durationStr) - 1;
			item->setBehaviorPropertyEffectValue(L"equipment", L"equipped-effects", L"duration", std::to_wstring(duration));

			if (duration <= 0)
				item->setBehaviorPropertyEffectValue(L"equipment", L"equipped-effects", L"view-power", L"0");
		}
	}
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

void MapObject::resetTurnActions() { _turnActions = 1; }

void MapObject::tryInteraction(MapObjectInteraction interaction)
{
	// Open / close
	if (interaction == MapObjectInteraction_OpenClose)
	{
		if (!hasBehavior(L"open-close")) return;

		auto isOpen = getBehaviorProperty(L"open-close", L"is-open").compare(L"true") == 0;
		isOpen = (isOpen) ? false : true;
		setBehaviorProperty(L"open-close", L"is-open", (isOpen) ? L"true" : L"false");
		if (isOpen)
			setBehaviorProperty(L"visual", L"chr", getBehaviorProperty(L"open-close", L"open-chr"));
		else
			setBehaviorProperty(L"visual", L"chr", getBehaviorProperty(L"open-close", L"closed-chr"));

		if (hasBehavior(L"storage")) return;

		setBehaviorProperty(L"passable", L"solid", (isOpen) ? L"true" : L"false");
		setBehaviorProperty(L"passable", L"light", (isOpen) ? L"true" : L"false");

		return;
	}
}

void MapObject::tryInteraction(MapObjectInteraction interaction, MapObject* other)
{
	// Wear / wield
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

void MapObject::setBehaviorPropertyEffectValue(std::wstring behaviorName, std::wstring propertyKey, std::wstring effectName, std::wstring effectValue)
{
	for (const auto& behavior : _behaviors)
	{
		if (behavior->name().compare(behaviorName) != 0) continue;

		behavior->setPropertyEffectValue(propertyKey, effectName, effectValue);
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
	if (turnActionsRemaining()) _turnActions--;
}

bool MapObject::turnActionsRemaining() { return _turnActions > 0; }

void MapObject::updateEquipmentEffects()
{
	auto ac = stoi(getBehaviorProperty(L"ac", L"default-value"));
	int acMod = 0;
	auto effects = getEquippedEffect(L"ac");
	for (auto e : effects)
		acMod += stoi(Strings::split(e, L':')[1]);
	ac += acMod;
	setBehaviorProperty(L"ac", L"value", std::to_wstring(ac));

	std::wstring attackDmgRoll = getBehaviorProperty(L"attack", L"dmg-roll-default");
	effects = getEquippedEffect(L"dmg-roll");
	if (effects.size() > 0) attackDmgRoll = Strings::split(effects[0], L':')[1];
	setBehaviorProperty(L"attack", L"dmg-roll", attackDmgRoll);

	auto viewPower = stoi(getBehaviorProperty(L"view", L"power-default"));
	int viewPowerMod = 0;
	effects = getEquippedEffect(L"view-power");
	for (auto e : effects)
		viewPowerMod += stoi(Strings::split(e, L':')[1]);
	viewPower += viewPowerMod;
	setBehaviorProperty(L"view", L"power", std::to_wstring(viewPower));
}