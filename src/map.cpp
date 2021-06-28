#include "map.h"

int Map::_depth;
Point2D Map::_drawOffset;
std::vector<MapObject*> Map::_objects;
MapObject* Map::_player;
Size2D Map::_size;
std::vector<MapTile*> Map::_tiles;

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
	}

	return object;
}

void Map::draw()
{
	for (int x = 0; x < _size.Width; x++)
	{
		for (int y = 0; y < _size.Height; y++)
		{
			Point2D point = { x, y };
			auto tile = _tiles[(y * _size.Width) + x];
			auto chr = L' ';
			auto colorPair = 1;

			if (_depth == 0)
			{
				chr = tile->getChr();
				colorPair = tile->getColorPair();
			}
			else
			{
				auto view = _player->getView(point, _size.Width);
				chr = view->Chr;
				if (view->State == 1)
				{
					if (chr == L'@') chr = tile->getChr();
					if (chr == L'.') chr = L' ';
				}

				colorPair = (view->State == 2) ? colorPair = tile->getColorPair() : 8;
			}

			auto consolePoint = Point2D::add(_drawOffset, point);
			Console::write(consolePoint.Y, consolePoint.X, chr, colorPair);
		}
	}
}

void Map::generate()
{
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

		std::vector<Rect> rects;

		for (int i = 0; i < 5; i++)
		{
			Rect rect;

			while (true)
			{
				rect.Position = { 3 + (rand() % (_size.Width - 10)), 1 + (rand() % (_size.Height - 10)) };
				rect.Size = { 3 + (rand() % 7), 3 + (rand() % 7) };

				if (rects.size() == 0) break;

				bool overlaps = false;
				for (const auto& r : rects)
				{
					if (!rect.overlaps(r, 1)) continue;

					overlaps = true;
					break;
				}

				if (overlaps) continue;

				break;
			}

			rects.push_back(rect);
		}

		for (const auto& r : rects)
		{
			for (int x = 0; x < r.Size.Width; x++)
			{
				for (int y = 0; y < r.Size.Height; y++)
				{
					Point2D point = { r.Position.X + x, r.Position.Y + y };

					if (!contains(point)) continue;

					setTile(point, MapTileType_Wall);
					setTile(point, MapTileTerrain_Stone);
				}
			}
		}

		for (int i = 0; i < 125; i++)
		{
			Point2D point;

			while (true)
			{
				point = { rand() % _size.Width, rand() % _size.Height };

				if (point.X > 3 && point.X < _size.Width - 3 && point.Y > 3 && point.Y < _size.Height - 3)
					continue;

				if (!getTilePassable(point, PassableType_Solid))
					continue;

				break;
			}

			placeObject(createObject(L"tree"), point);
		}
	}
	else
	{
		for (int x = 0; x < _size.Width; x++)
		{
			for (int y = 0; y < _size.Height; y++)
			{
				setTile({ x, y }, MapTileType_Wall);
				setTile({ x, y }, MapTileTerrain_Stone);
				setTile({ x, y }, 3);
			}
		}

		std::vector<Rect> rects;

		for (int i = 0; i < 7 + (rand() % 2); i++)
		{
			Rect rect;

			while (true)
			{
				rect.Position = { 3 + (rand() % (_size.Width - 12)), 1 + (rand() % (_size.Height - 10)) };
				rect.Size = { 3 + (rand() % 7), 3 + (rand() % 5) };

				if (rects.size() == 0) break;

				bool overlaps = false;
				for (const auto& r : rects)
				{
					if (!rect.overlaps(r, 2)) continue;

					overlaps = true;
					break;
				}

				if (overlaps) continue;

				break;
			}

			rects.push_back(rect);
		}

		for (const auto& r : rects)
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

		for (int i = 0; i < rects.size(); i++)
		{
			int j = i + 1;
			if (j == rects.size()) j = 0;

			auto at = rects[i].midpoint();
			auto to = rects[j].midpoint();

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

				if (at.X != to.X && rand() % 10 >= 2)
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
	}

	while (true && _depth > 0)
	{
		Point2D point = { rand() % _size.Width, rand() % _size.Height };

		if (!getTilePassable(point, PassableType_Solid) || getTileObject(point) != nullptr)
			continue;

		placeObject(createObject(L"stairs_up"), point);

		break;
	}

	while (true && _depth < 50)
	{
		Point2D point = { rand() % _size.Width, rand() % _size.Height };

		if (!getTilePassable(point, PassableType_Solid) || getTileObject(point) != nullptr)
			continue;

		placeObject(createObject(L"stairs_down"), point);

		break;
	}

	while (true)
	{
		Point2D point = { rand() % _size.Width, rand() % _size.Height };

		if (!getTilePassable(point, PassableType_Solid))
			continue;

		if (_player == nullptr) _player = createObject(L"player");
		placeObject(_player, point);

		Console::CursorPosition = Point2D::add(point, _drawOffset);

		updateObjectView(_player);

		break;
	}
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
	// ...

	_player->nextTurn();
	if(_depth > 0) updateObjectView(_player);
}

void Map::objectTryInteraction(MapObject* object, MapObjectInteraction interaction)
{
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
}

void Map::objectTryInteraction(MapObject* object, MapObjectInteraction interaction, Direction2D direction)
{
	if (interaction == MapObjectInteraction_Move)
	{
		auto to = Point2D::add(object->position(), direction);

		if (!getTilePassable(to, PassableType_Solid))
		{
			if (object == _player && !contains(to))
				UI::log("You couldn't bear to leave your village.");

			return;
		}

		moveObject(object, to);

		if(object == _player)
			Console::CursorPosition = Point2D::add(to, _drawOffset);

		object->takeTurnAction();

		return;
	}
}

void Map::placeObject(MapObject* object, Point2D point)
{
	if (!contains(point)) return;

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

void Map::updateObjectView(MapObject* object)
{
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

int Path::size() { return _nodes.size(); }