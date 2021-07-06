#include "map.h"

MapAnimation::MapAnimation(Point2D position, wchar_t chr, std::string fg, std::string bg, double time)
{
	Position = position;
	Chr = chr;
	ColorFG = fg;
	ColorBG = bg;
	Timer = time;
}

std::vector<MapAnimation*> Map::_animations;
int Map::_depth;
Point2D Map::_drawOffset;
std::vector<MapObject*> Map::_objects;
MapObject* Map::_player;
Size2D Map::_size;
std::vector<MapTile*> Map::_tiles;

void Map::addAnimation(MapAnimation* animation)
{
	_animations.push_back(animation);
}

void Map::animate(double delta)
{
	if (_animations.size() == 0) return;

	_animations[0]->Timer -= delta;
	if (_animations[0]->Timer <= 0.0) 
		_animations.erase(_animations.begin());
}

bool Map::contains(Point2D point)
{
	return point.X >= 0 && point.Y >= 0 && point.X < _size.Width && point.Y < _size.Height;
}

void Map::changeDepth(int by)
{
	_depth = min(max(_depth + by, 0), 50);

	for (int x = 0; x < _size.Width; x++)
	{
		for (int y = 0; y < _size.Height; y++)
			_tiles[(y * _size.Width) + x]->removeObjects();
	}

	_objects.erase(_objects.begin(), _objects.end());
	_objects.push_back(_player);

	_player->clearView();

	generate();
}

MapObject* Map::createObject(std::wstring id)
{
	auto lines = Data::getObject(id);

	if (lines.size() == 0) return nullptr;

	auto object = new MapObject();

	for (int i = 1; i < lines.size(); i++)
	{
		auto parts = Strings::split(lines[i].substr(1), L'#');
		auto behaviorName = parts[0];
		auto behavior = new MapObjectBehavior(behaviorName);

		for (int j = 1; j < parts.size(); j++)
		{
			auto sides = Strings::split(parts[j], L'=');
			std::wstring rightSide = std::wstring(sides[1].begin(), sides[1].end());
			
			auto behaviorProperty = new MapObjectBehaviorProperty(sides[0], rightSide);
			behavior->addProperty(behaviorProperty);
		}

		object->addBehavior(behavior);

		if (behaviorName.compare(L"view") == 0) object->createView(_size);
		
		if (behaviorName.compare(L"has-equipment") == 0)
			object->createEquipmentSlots();

		if (behaviorName.compare(L"has-inventory") == 0)
		{
			auto startingItems = behavior->getPropertyValue(L"starting");

			if (startingItems.length() > 0)
			{
				auto startingItemsIDs = Strings::split(startingItems, ',');

				for (auto siid : startingItemsIDs)
					object->addToInventory(createObject(siid));
			}
		}
	}

	return object;
}

void Map::draw()
{
	Console::setFont("tiles");

	for (int x = 0; x < _size.Width; x++)
	{
		for (int y = 0; y < _size.Height; y++)
		{
			Point2D point = { x, y };
			auto tile = _tiles[(y * _size.Width) + x];
			auto chr = L' ';
			SDL_Color fg = Console::getColor("white");
			SDL_Color bg = Console::getColor("background");

			auto view = _player->getView(point, _size.Width);
			chr = view->Chr;
			fg = (view->State == 2) ? Console::getColor(tile->getColorFG()) : Console::getColor("background");
			bg = (view->State == 2) ? Console::getColor(tile->getColorBG()) : Console::getColor("background");

			if (view->State == 1)
			{
				fg = Console::getColor("dark-grey");
				if (chr == L'@') chr = tile->getChr();
				if (chr == L'.') chr = L' ';
				if (tile->type() == MapTileType_Wall) bg = { 0, 0, 0, 0 };
			}

			if (tile->type() == MapTileType_Wall)
			{
				if (y < _size.Height - 1)
				{
					if (getTileType({ x, y + 1 }) == MapTileType_Floor)
						chr = L'▒';
				}
			}

			auto consolePoint = Point2D::add(_drawOffset, point);
			Console::write(chr, { consolePoint.X, consolePoint.Y }, fg, bg);
		}
	}

	if(_animations.size() > 0)
		Console::write(_animations[0]->Chr, { _animations[0]->Position.X + _drawOffset.X, _animations[0]->Position.Y + _drawOffset.Y }, Console::getColor(_animations[0]->ColorFG), Console::getColor(_animations[0]->ColorBG));
}

bool Map::dumpObject(MapObject* object, Point2D point)
{
	if (getTilePassable(point, PassableType_Solid))
	{
		placeObject(object, point);
		return true;
	}

	for (int x = -1; x < 2; x++)
	{
		for (int y = -1; y < 2; y++)
		{
			Point2D offset = { x, y };
			auto to = Point2D::add(point, offset);

			if (!getTilePassable(to, PassableType_Solid))
				continue;

			placeObject(object, to);
			return true;
		}
	}

	return false;
}

void Map::generate()
{
	std::vector<MapObject*> chests;

	// At the village

	if (_depth == 0)
	{
		for (int x = 0; x < _size.Width; x++)
		{
			for (int y = 0; y < _size.Height; y++)
			{
				setTile({ x, y }, MapTileType_Floor);
				setTile({ x, y }, MapTileTerrain_Grass);
				setTile({ x, y }, 10);
			}
		}

		std::vector<Rect> villageBuildings;

		for (int i = 0; i < 6; i++)
		{
			Rect rect;

			while (true)
			{
				rect.Position = { 3 + Random::nextInt(0, _size.Width - 15), 1 + Random::nextInt(0, _size.Height - 10) };
				rect.Size = { 7 + Random::nextInt(0, 4), 5 + Random::nextInt(0, 2) };

				if (villageBuildings.size() == 0) break;

				bool overlaps = false;
				for (const auto& r : villageBuildings)
				{
					if (!rect.overlaps(r, 2)) continue;

					overlaps = true;
					break;
				}

				if (overlaps) continue;

				break;
			}

			villageBuildings.push_back(rect);
		}

		for (const auto& r : villageBuildings)
		{
			for (int x = 0; x < r.Size.Width; x++)
			{
				for (int y = 0; y < r.Size.Height; y++)
				{
					Point2D point = { r.Position.X + x, r.Position.Y + y };

					if (!contains(point)) continue;

					if(x > 0 && x < r.Size.Width - 1 && y > 0 && y < r.Size.Height - 1)
						setTile(point, MapTileType_Floor);
					else
					{
						if(x == (r.Size.Width / 2) && y == r.Size.Height - 1)
							setTile(point, MapTileType_Floor);
						else
							setTile(point, MapTileType_Wall);
					}

					setTile(point, MapTileTerrain_Stone);
				}
			}
		}

		for (int x = 1; x < _size.Width - 1; x++)
		{
			for (int y = 1; y < _size.Height - 1; y++)
			{
				if (getTileType({ x, y }) != MapTileType_Floor)
					continue;

				if (!(getTileType({ x - 1, y }) == MapTileType_Wall && getTileType({ x + 1, y }) == MapTileType_Wall) || (getTileType({ x, y - 1 }) == MapTileType_Wall && getTileType({ x, y + 1 }) == MapTileType_Wall))
					continue;

				placeObject(createObject(L"door"), { x, y });
			}
		}

		for (int i = 0; i < 125; i++)
		{
			Point2D point;

			while (true)
			{
				point = { Random::nextInt(0, _size.Width), Random::nextInt(0, _size.Height) };

				if (point.X > 3 && point.X < _size.Width - 3 && point.Y > 3 && point.Y < _size.Height - 3)
					continue;

				if (!getTilePassable(point, PassableType_Solid))
					continue;

				bool contains = false;
				for (auto r : villageBuildings)
				{
					if (!r.contains(point)) continue;

					contains = true;
					break;
				}

				if (contains) continue;

				break;
			}

			placeObject(createObject(L"tree"), point);
		}

		std::vector<std::vector<std::wstring>> villagerNamesDescriptions =
			{
				{ L"Bedfre", L"It's Bedfre, the baker." },
				{ L"Moda", L"It's Moda, the midwife." },
				{ L"Ebed", L"It's Ebed, the blacksmith." },
				{ L"Sige", L"It's Sige, the blacksmith's apprentice." },
				{ L"Stephye", L"It's Stephye, the farmer." },
				{ L"Adulf", L"It's Adulf, the apothecary." },
				{ L"Coenwy", L"It's Coenwy, the troubador." },
				{ L"Hearda", L"It's Hearda, the farmer." },
				{ L"Ethes", L"It's Ethes, the bowyer." },
				{ L"Eodhed", L"It's Eodhed, the alchemist." },
				{ L"Aroth", L"It's Aroth, the mage." },
				{ L"Gauwill", L"It's Gauwill, the fletcher." },
				{ L"Piersym", L"It's Piersym, the priest." },
				{ L"Eadwald", L"It's Eadwald, the armorer." },
				{ L"Ceolfre", L"It's Ceolfre, a young boy." },
				{ L"Genla", L"It's Genla, a young girl." }
			};

		for (int i = 0; i < villagerNamesDescriptions.size(); i++)
		{
			Point2D point;

			while (true)
			{
				point = { Random::nextInt(5, _size.Width - 5), Random::nextInt(5, _size.Height - 5) };

				if (!getTilePassable(point, PassableType_Solid))
					continue;

				break;
			}

			auto villager = createObject(L"villager");
			villager->setBehaviorProperty(L"name", L"value", villagerNamesDescriptions[i][0]);
			villager->setBehaviorProperty(L"description", L"long", villagerNamesDescriptions[i][1]);

			placeObject(villager, point);
		}

		for (int i = 0; i < 4; i++)
		{
			Point2D point;

			while (true)
			{
				point = { Random::nextInt(5, _size.Width - 5), Random::nextInt(5, _size.Height - 5) };

				if (!getTilePassable(point, PassableType_Solid))
					continue;

				break;
			}

			placeObject(createObject(L"pet_cat"), point);
		}

		for (int i = 0; i < 5; i++)
		{
			Point2D point;

			while (true)
			{
				point = { Random::nextInt(5, _size.Width - 5), Random::nextInt(5, _size.Height - 5) };

				if (!getTilePassable(point, PassableType_Solid))
					continue;

				bool contains = false;
				for (auto r : villageBuildings)
				{
					if (!r.contains(point)) continue;

					contains = true;
					break;
				}

				if (!contains) continue;

				break;
			}

			auto chest = createObject(L"chest");
			chests.push_back(chest);
			placeObject(chest, point);
		}
	}
	else
	{
		// In the dungeon

		for (int x = 0; x < _size.Width; x++)
		{
			for (int y = 0; y < _size.Height; y++)
			{
				setTile({ x, y }, MapTileType_Wall);
				setTile({ x, y }, MapTileTerrain_Stone);
				setTile({ x, y }, 3);
			}
		}

		std::vector<Rect> rooms;

		for (int i = 0; i < 7 + Random::nextInt(0, 2); i++)
		{
			Rect rect;

			while (true)
			{
				rect.Position = { 3 + Random::nextInt(0, _size.Width - 12), 1 + Random::nextInt(0, _size.Height - 10) };
				rect.Size = { 3 + Random::nextInt(0, 7), 3 + Random::nextInt(0, 5) };

				if (rooms.size() == 0) break;

				bool overlaps = false;
				for (const auto& r : rooms)
				{
					if (!rect.overlaps(r, 2)) continue;

					overlaps = true;
					break;
				}

				if (overlaps) continue;

				break;
			}

			rooms.push_back(rect);
		}

		for (const auto& r : rooms)
		{
			for (int x = 0; x < r.Size.Width; x++)
			{
				for (int y = 0; y < r.Size.Height; y++)
				{
					Point2D point = { r.Position.X + x, r.Position.Y + y };

					if (!contains(point)) continue;

					setTile(point, MapTileType_Floor);
					setTile(point, MapTileTerrain_Stone);
					setTile(point, 6);
				}
			}
		}

		for (int i = 0; i < rooms.size(); i++)
		{
			int j = i + 1;
			if (j == rooms.size()) j = 0;

			auto at = rooms[i].midpoint();
			auto to = rooms[j].midpoint();

			auto path = findPath(at, to);
			if (path->isComplete())
			{
				delete path;
				continue;
			}

			while (!at.equals(to))
			{
				setTile(at, MapTileType_Floor);

				if (at.Y == to.Y)
				{
					if (to.X > at.X) at.X++;
					if (to.X < at.X) at.X--;

					continue;
				}

				if (at.X != to.X && Random::nextInt(0, 10) >= 2)
				{
					if (to.X > at.X) at.X++;
					if (to.X < at.X) at.X--;

					continue;
				}

				if (to.Y > at.Y) at.Y++;
				if (to.Y < at.Y) at.Y--;

				path = findPath(at, to);
				if (path->isComplete())
				{
					delete path;
					break;
				}
			}
		}

		for (const auto& r : rooms)
		{
			for (int x = -1; x < r.Size.Width + 1; x++)
			{
				for (int y = -1; y < r.Size.Height + 1; y++)
				{
					Point2D point = { r.Position.X + x, r.Position.Y + y };

					if (!contains(point)) continue;
					if (getTileType(point) != MapTileType_Floor)
						continue;
					if (!((getTileType({point.X - 1, point.Y }) == MapTileType_Wall && getTileType({ point.X + 1, point.Y }) == MapTileType_Wall) || (getTileType({ point.X, point.Y - 1 }) == MapTileType_Wall && getTileType({ point.X, point.Y + 1 }) == MapTileType_Wall)))
						continue;

					placeObject(createObject(L"door"), point);
				}
			}
		}

		for (int i = 0; i < 4 + Random::nextInt(0, 3); i++)
		{
			Point2D point;

			while (true)
			{
				point = { Random::nextInt(5, _size.Width - 5), Random::nextInt(5, _size.Height - 5) };

				if (!getTilePassable(point, PassableType_Solid))
					continue;

				bool contains = false;
				for (auto r : rooms)
				{
					if (!r.contains(point)) continue;

					contains = true;
					break;
				}

				if (!contains) continue;

				break;
			}

			auto chest = createObject(L"chest");
			chests.push_back(chest);
			placeObject(chest, point);
		}

		auto monsterNames = Data::getObjectList(L"MONSTERS");
		std::vector<std::wstring> monsterNamesSelected;

		for (auto mn : monsterNames)
		{
			auto passes = false;
			auto monsterData = Data::getObject(mn);
			for (int i = 0; i < monsterData.size(); i++)
			{
				auto line = monsterData[i].substr(1);
				auto parts = Strings::split(line, L'#');

				if (parts[0].compare(L"spawn") != 0) continue;

				parts = Strings::split(parts[1], L'=');
				parts = Strings::split(parts[1], L'-');

				auto min = stoi(parts[0]);
				auto max = stoi(parts[1]);

				if (_depth < min || _depth > max) continue;

				passes = true;
				break;
			}

			if (!passes) continue;

			monsterNamesSelected.push_back(mn);
		}

		if (monsterNamesSelected.size() > 0)
		{
			for (int i = 0; i < 8 + Random::nextInt(0, 2); i++)
			{
				Point2D point = { 0, 0 };

				while (true)
				{
					point = { Random::nextInt(0, _size.Width), Random::nextInt(0, _size.Height) };

					if (!getTilePassable(point, PassableType_Solid) || getTileObject(point) != nullptr)
						continue;

					break;
				}

				auto monster = createObject(monsterNamesSelected[Random::nextInt(0, monsterNamesSelected.size())]);
				placeObject(monster, point);
			}
		}
	}

	auto itemNames = Data::getObjectList(L"ITEMS");
	std::vector<std::wstring> itemNamesSelected;

	for (auto itn : itemNames)
	{
		auto passes = false;
		auto itemData = Data::getObject(itn);
		for (int i = 0; i < itemData.size(); i++)
		{
			auto line = itemData[i].substr(1);
			auto parts = Strings::split(line, L'#');

			if (parts[0].compare(L"spawn") != 0) continue;

			parts = Strings::split(parts[1], L'=');
			parts = Strings::split(parts[1], L'-');

			auto min = stoi(parts[0]);
			auto max = stoi(parts[1]);

			if (_depth < min || _depth > max) continue;

			passes = true;
			break;
		}

		if (!passes) continue;

		itemNamesSelected.push_back(itn);
	}

	if (itemNamesSelected.size() > 0)
	{
		auto itemsBaseCount = (_depth == 0) ? 15 : 5;
		for (int i = 0; i < itemsBaseCount + Random::nextInt(0, 5); i++)
		{
			auto item = createObject(itemNamesSelected[Random::nextInt(0, itemNamesSelected.size())]);

			if (_depth == 0)
			{
				_objects.push_back(item);
				chests[Random::nextInt(0, chests.size())]->addToInventory(item);
			}
			else
			{
				if (Random::nextInt(0, 10) > 3 && chests.size() > 0)
				{
					_objects.push_back(item);
					chests[Random::nextInt(0, chests.size())]->addToInventory(item);
				}
				else
				{
					Point2D point = { 0, 0 };

					while (true)
					{
						point = { Random::nextInt(0, _size.Width), Random::nextInt(0, _size.Height) };

						if (!getTilePassable(point, PassableType_Solid) || getTileObject(point) != nullptr)
							continue;

						break;
					}

					placeObject(item, point);
				}
			}
		}
	}

	while (true && _depth > 0)
	{
		Point2D point = { Random::nextInt(0, _size.Width), Random::nextInt(0, _size.Height) };

		if (!getTilePassable(point, PassableType_Solid) || getTileObject(point) != nullptr)
			continue;

		placeObject(createObject(L"stairs_up"), point);

		break;
	}

	while (true && _depth < 50)
	{
		Point2D point = { Random::nextInt(0, _size.Width), Random::nextInt(0, _size.Height) };

		if (!getTilePassable(point, PassableType_Solid) || getTileObject(point) != nullptr)
			continue;

		placeObject(createObject(L"stairs_down"), point);

		break;
	}

	while (true)
	{
		Point2D point = { Random::nextInt(0, _size.Width), Random::nextInt(0, _size.Height) };

		if (!getTilePassable(point, PassableType_Solid))
			continue;

		if (_player == nullptr) _player = createObject(L"player");
		placeObject(_player, point);

		Console::CursorPosition = Point2D::add(point, _drawOffset);

		updateObjectView(_player);

		break;
	}

	if (_depth == 0) 
		Audio::switchMusic("village");
	else
		Audio::switchMusic("dungeon");
}

bool Map::isAnimating(bool interruptingOnly)
{
	return _animations.size() > 0;
}

Path* Map::findPath(Point2D from, Point2D to)
{
	auto path = new Path();

	if (from.equals(to)) return path;

	Point2D at = { from.X, from.Y };
	int attempts = 1000;
	Direction2D dirs[8] =
	{
		{ 0, -1 },
		{ 1, -1 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 },
		{ -1, 1 },
		{ -1, 0 },
		{ -1, -1 }
	};

	while (attempts > 0)
	{
		path->add({ at.X, at.Y });

		attempts--;

		Point2D todir = { 0, 0 };
		if (to.X > at.X) todir.X = 1;
		if (to.X < at.X) todir.X = -1;
		if (to.Y > at.Y) todir.Y = 1;
		if (to.Y < at.Y) todir.Y = -1;

		auto todiri = 0;
		for (auto d : dirs)
		{
			if (todir.X != d.X || todir.Y != d.Y)
			{
				todiri++;
				continue;
			}

			break;
		}

		int todiris[2] = { todiri, todiri };
		int counts[2] = { 0, 0 };
		Point2D nexts[2] = { at, at };

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				auto d = dirs[todiris[i]];
				Point2D next = Point2D::add(at, d);
				bool pass = true;

				if (!contains(next))
					pass = false;
				else
				{
					if (next.X != to.X || next.Y != to.Y)
					{
						if (!getTilePassable(next, PassableType_Solid))
							pass = false;
					}
				}

				if (path->contains(next)) pass = false;

				if (!pass)
				{
					counts[i]++;
					todiris[i] += (i == 0) ? 1 : -1;

					if (todiris[i] > 7) todiris[i] = 0;
					if (todiris[i] < 0) todiris[i] = 7;

					continue;
				}

				nexts[i] = next;
				break;
			}
		}

		if (nexts[0].X == at.X && nexts[0].Y == at.Y && nexts[1].X == at.X && nexts[1].Y == at.Y)
			return path;

		if (nexts[0].X == at.X && nexts[0].Y == at.Y)
			at = nexts[1];

		if (nexts[1].X == at.X && nexts[1].Y == at.Y)
			at = nexts[0];

		if (counts[0] <= counts[1])
			at = nexts[0];
		else
			at = nexts[1];

		if (at.X == to.X && at.Y == to.Y)
		{
			path->add({ at.X, at.Y });
			path->complete();

			return path;
		}
	}

	return path;
}

std::string Map::getTileDescription(Point2D point, bool longDescription)
{
	if (!contains(point)) return "";

	return _tiles[(point.Y * _size.Width) + point.X]->getDescription(longDescription);
}

int Map::getTileLight(Point2D point)
{
	if (!contains(point)) return 0;

	return _tiles[(point.Y * _size.Width) + point.X]->light();
}

MapObject* Map::getTileObject(Point2D point, int i)
{
	if (!contains(point)) return nullptr;

	return _tiles[(point.Y * _size.Width) + point.X]->object(i);
}

bool Map::getTilePassable(Point2D point, PassableType passableType)
{
	if (!contains(point)) return false;

	return _tiles[(point.Y * _size.Width) + point.X]->isPassable(passableType);
}

MapTileTerrain Map::getTileTerrain(Point2D point)
{
	if (!contains(point)) return MapTileTerrain_Stone;

	return _tiles[(point.Y * _size.Width) + point.X]->terrain();
}

MapTileType Map::getTileType(Point2D point)
{
	if (!contains(point)) return MapTileType_Empty;

	return _tiles[(point.Y * _size.Width) + point.X]->type();
}

void Map::moveObject(MapObject* object, Point2D to)
{
	if (std::find(_objects.begin(), _objects.end(), object) == _objects.end())
		return;

	auto op = object->position();
	_tiles[(op.Y * _size.Width) + op.X]->removeObject(object);
	placeObject(object, to);
}

void Map::nextTurn()
{
	for (auto const& obj : _objects)
	{
		if (obj == _player) continue;

		obj->nextTurn();
		updateObjectView(obj);
		obj->resetTurnActions();

		while(obj->turnActionsRemaining()) updateObject(obj);
	}

	_player->nextTurn();
	updateObjectView(_player);
}

void Map::objectTryInteraction(MapObject* object, MapObjectInteraction interaction)
{
	// Ascend
	if (interaction == MapObjectInteraction_Ascend)
	{
		if (object != _player) return;

		auto other = getTileObject(_player->position(), 1);
		if (other == nullptr) return;
		if (other->getBehaviorProperty(L"stairs", L"dir").compare(L"up") != 0)
			return;

		changeDepth(-1);

		return;
	}

	// Descend
	if (interaction == MapObjectInteraction_Descend)
	{
		if (object != _player) return;

		auto other = getTileObject(_player->position(), 1);
		if (other == nullptr) return;
		if (other->getBehaviorProperty(L"stairs", L"dir").compare(L"down") != 0)
			return;

		changeDepth(1);

		return;
	}

	// Dump inventory
	if (interaction == MapObjectInteraction_DumpInventory)
	{
		if (!object->hasBehavior(L"has-inventory")) return;

		for (int i = 0; i < object->getInventorySize(); i++)
		{
			auto ito = object->getInventory(i);
			if (!dumpObject(ito, object->position())) 
				return;

			object->removeFromInventory(ito);
		}

		return;
	}

	// Pick up
	if (interaction == MapObjectInteraction_PickUp)
	{
		if (!object->hasBehavior(L"has-inventory")) return;

		auto other = getTileObject(object->position(), 1);

		if (other == nullptr) return;

		auto op = object->position();
		object->takeTurnAction();

		object->addToInventory(other);
		_tiles[(op.Y * _size.Width) + op.X]->removeObject(other);

		if (object == _player)
		{
			auto otherName = Strings::toLower(Strings::from(other->getBehaviorProperty(L"name", L"value")));
			UI::log("You pick up the " + otherName + ".");
		}

		return;
	}
}

void Map::objectTryInteraction(MapObject* object, MapObjectInteraction interaction, Direction2D direction)
{
	// Move
	if (interaction == MapObjectInteraction_Move)
	{
		if (!object->hasBehavior(L"move")) return;

		auto to = Point2D::add(object->position(), direction);

		if (!getTilePassable(to, PassableType_Solid))
		{
			if (object == _player && !contains(to))
				UI::log("You couldn't bear to leave your village.");
			else
			{
				auto other = getTileObject(to);

				if (other == nullptr) return;

				if (other->hasBehavior(L"open-close"))
				{
					if (other->getBehaviorProperty(L"open-close", L"is-open").compare(L"false") == 0)
					{
						objectTryInteraction(object, MapObjectInteraction_OpenCloseOther, other);
						return;
					}
				}

				if (!object->hasBehavior(L"attack") || !other->hasBehavior(L"attack") || object->getBehaviorProperty(L"faction", L"value").compare(other->getBehaviorProperty(L"faction", L"value")) == 0) 
					return;

				objectTryInteraction(object, MapObjectInteraction_Attack, other);
			}

			return;
		}

		moveObject(object, to);

		if(object == _player)
			Console::CursorPosition = Point2D::add(to, _drawOffset);

		object->takeTurnAction();

		return;
	}
}

void Map::objectTryInteraction(MapObject* object, MapObjectInteraction interaction, MapObject* other)
{
	auto objectName = Strings::toLower(Strings::from(object->getBehaviorProperty(L"name", L"value")));
	auto otherName = Strings::toLower(Strings::from(other->getBehaviorProperty(L"name", L"value")));

	// Attack
	if (interaction == MapObjectInteraction_Attack)
	{
		object->takeTurnAction();

		std::string toHitRollExpression = "1d20";
		auto toHitStr = Strings::from(object->getBehaviorProperty(L"attack", L"to-hit"));
		auto toHitInt = stoi(toHitStr);
		toHitRollExpression += (toHitInt >= 0) ? "+" : "";
		toHitRollExpression += toHitStr;
		auto toHitRoll = new DiceRoll(toHitRollExpression);
		auto ac = stoi(other->getBehaviorProperty(L"ac", L"value"));

		if (toHitRoll->roll() <= ac)
		{
			if (object == _player)
				UI::log("You miss the " + otherName + ".");
			if (other == _player)
				UI::log("The " + otherName + " misses you.");

			addAnimation(new MapAnimation(other->position(), L'/', "white", "background", 80));

			delete toHitRoll;
			return;
		}

		auto damageRoll = new DiceRoll(Strings::from(object->getBehaviorProperty(L"attack", L"dmg-roll")));
		auto damage = damageRoll->roll();

		other->takeDamage(damage);

		if (object == _player)
			UI::log("You " + Strings::from(object->getBehaviorProperty(L"attack", L"verb")) + " the " + otherName + ".");
		if (other == _player)
			UI::log("The " + objectName + " " + Strings::from(object->getBehaviorProperty(L"attack", L"verb")) + " you.");

		auto animationChr = L'X';

		if (stoi(other->getBehaviorProperty(L"hp", L"value")) <= 0)
		{
			if (other != _player)
			{
				removeObject(other);
				UI::log("You kill the " + otherName + "!");
				animationChr = L'♥';
			}
		}

		addAnimation(new MapAnimation(other->position(), animationChr, "bright-red", "background", 80));

		auto attackSoundName = object->getBehaviorProperty(L"attack", L"sound");
		if(attackSoundName.compare(L"none") != 0) 
			Audio::playSound(Strings::from(attackSoundName));

		delete toHitRoll; delete damageRoll;

		return;
	}

	// Open / close other
	if (interaction == MapObjectInteraction_OpenCloseOther)
	{
		if (!object->hasBehavior(L"open-close-other"))
		{
			if (object == _player)
				UI::log("You can't open or close anything.");

			return;
		}

		auto isOpen1 = other->getBehaviorProperty(L"open-close", L"is-open");
		other->tryInteraction(MapObjectInteraction_OpenClose);
		auto isOpen2 = other->getBehaviorProperty(L"open-close", L"is-open");

		if (isOpen1.compare(isOpen2) == 0)
		{
			if (object == _player)
				UI::log("That can't be opened or closed.");

			return;
		}

		object->takeTurnAction();

		if (isOpen2.compare(L"true") == 0)
		{
			auto effects = Strings::split(other->getBehaviorProperty(L"open-close", L"opened-effects"), L';');

			for (auto eff : effects)
			{
				if (eff.compare(L"dump-inventory") == 0)
					objectTryInteraction(other, MapObjectInteraction_DumpInventory);
			}
		}

		if (object == _player)
		{
			if(isOpen2.compare(L"true") == 0)
				UI::log("You open the " + otherName + ".");
			else
				UI::log("You close the " + otherName + ".");
		}

		if (isOpen2.compare(L"true") == 0)
		{
			auto soundName = other->getBehaviorProperty(L"open-close", L"open-sound");
			if (soundName.compare(L"none") != 0) 
				Audio::playSound(Strings::from(soundName));
		}
		else
		{
			auto soundName = other->getBehaviorProperty(L"open-close", L"close-sound");
			if (soundName.compare(L"none") != 0)
				Audio::playSound(Strings::from(soundName));
		}

		return;
	}

	// Wear / wield
	if (interaction == MapObjectInteraction_WearWield)
	{
		auto eqat1 = object->getEquippedSlot(other);
		object->tryInteraction(MapObjectInteraction_WearWield, other);
		auto eqat2 = object->getEquippedSlot(other);

		if (eqat1.compare(eqat2) == 0)
		{
			if (object == _player)
			{
				if(eqat1.length() == 0)
					UI::log("You can't equip that.");
				else
					UI::log("You can't take that off!");
			}

			return;
		}

		auto otherName = Strings::toLower(Strings::from(other->getBehaviorProperty(L"name", L"value")));

		if (eqat2.compare(L"") != 0)
		{
			if (object == _player)
				UI::log("You ready the " + otherName + ".");

			return;
		}

		if (object == _player)
			UI::log("You put away the " + otherName + ".");

		return;
	}
}

void Map::placeObject(MapObject* object, Point2D point)
{
	if (object == nullptr || !contains(point)) return;

	if (std::find(_objects.begin(), _objects.end(), object) == _objects.end())
		_objects.push_back(object);

	_tiles[(point.Y * _size.Width) + point.X]->addObject(object);
	object->setPosition(point);
}

Point2D Map::positionFromConsolePosition(Point2D point)
{
	return Point2D::subtract(point, _drawOffset);
}

Point2D Map::positionToConsolePosition(Point2D point)
{
	return Point2D::add(point, _drawOffset);
}

MapObject* Map::player()
{
	return _player;
}

void Map::removeObject(MapObject* object)
{
	for (int i = 0; i < _size.Width * _size.Height; i++)
		_tiles[i]->removeObject(object);

	auto ind = -1;
	for (int i = 0; i < _objects.size(); i++)
	{
		if (_objects[i] != object) continue;

		ind = i;
		break;
	}

	_objects.erase(_objects.begin() + ind);
}

void Map::setTile(Point2D point, MapTileType type)
{
	if (!contains(point)) return;

	_tiles[(point.Y * _size.Width) + point.X]->set(type);
}

void Map::setTile(Point2D point, MapTileTerrain terrain)
{
	if (!contains(point)) return;

	_tiles[(point.Y * _size.Width) + point.X]->set(terrain);
}

void Map::setTile(Point2D point, int light)
{
	_tiles[(point.Y * _size.Width) + point.X]->set(light);
}

void Map::setup(Size2D size, Point2D drawOffset)
{
	_size = size;
	_drawOffset = drawOffset;

	for (int i = 0; i < _size.Width * _size.Height; i++)
		_tiles.push_back(new MapTile(MapTileType_Empty, MapTileTerrain_Stone, 0));

	_player = nullptr;
}

Size2D Map::size() { return _size; }

void Map::updateObject(MapObject* object)
{
	if (object == _player) return;

	object->takeTurnAction();

	if (object->hasBehavior(L"attack"))
	{
		if (object->getBehaviorProperty(L"faction", L"value").compare(_player->getBehaviorProperty(L"faction", L"value")) != 0)
		{
			if (object->getView(_player->position(), _size.Width)->State == 2)
			{
				auto path = findPath(object->position(), _player->position());

				if (!path->isComplete()) return;
				if (path->length() < 2) return;

				auto direction = object->position().towards(path->get(1));
				objectTryInteraction(object, MapObjectInteraction_Move, direction);

				return;
			}
		}
	}

	if (object->hasBehavior(L"wander"))
	{
		if (Random::next() >= 0.5)
		{
			Direction2D direction = { Random::nextInt(-1, 2), Random::nextInt(-1, 2) };
			auto to = Point2D::add(object->position(), direction);

			if (!getTilePassable(to, PassableType_Solid)) return;

			objectTryInteraction(object, MapObjectInteraction_Move, direction);

			return;
		}
	}
}

void Map::updateObjectView(MapObject* object)
{
	if (!object->hasBehavior(L"view")) return;

	for (int x = 0; x < _size.Width; x++)
	{
		for (int y = 0; y < _size.Height; y++)
		{
			if (object->getView({ x, y }, _size.Width)->State < 2) continue;

			object->setView({ x, y }, _size.Width, 1);
		}
	}

	auto op = object->position();
	auto power = stoi(object->getBehaviorProperty(L"view", L"power"));
	auto distance = stoi(object->getBehaviorProperty(L"view", L"distance"));

	for (int d = 0; d < 360; d++)
	{
		auto powerd = power;

		for (int l = 0; l < distance; l++)
		{
			Point2D point = { (int)round(op.X + sin(d) * l), (int)round(op.Y + cos(d) * l) };

			if (!contains(point)) break;

			object->setView({ point.X, point.Y }, _size.Width, 2, _tiles[(point.Y * _size.Width) + point.X]->getChr());

			powerd += getTileLight(point) - 10;
			if (powerd <= 0) break;

			if (!getTilePassable(point, PassableType_Light)) break;
		}
	}
}

Path::Path() { _isComplete = false; }

Path::~Path() { }

void Path::add(Point2D p) { _nodes.push_back(p); }

void Path::complete() { _isComplete = true; }

bool Path::contains(Point2D point)
{
	for (auto node : _nodes)
	{
		if (node.equals(point)) return true;
	}

	return false;
}

Point2D Path::get(int i) { return _nodes[i]; }

bool Path::isComplete() { return _isComplete; }

int Path::length() { return _nodes.size(); }