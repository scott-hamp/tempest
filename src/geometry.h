#pragma once

#ifndef Geometry_H
#define Geometry_H

struct Direction2D
{
	int X, Y;

	bool isZero() { return X == 0 && Y == 0; };

	bool equals(Direction2D other)
	{
		return X == other.X && Y == other.Y;
	}
};

struct Point2D
{
	int X, Y;

	static Point2D add(Point2D a, Point2D b)
	{
		return { a.X + b.X, a.Y + b.Y };
	}

	static Point2D add(Point2D a, Direction2D b)
	{
		return { a.X + b.X, a.Y + b.Y };
	}

	bool equals(Point2D other)
	{
		return X == other.X && Y == other.Y;
	}

	static Point2D subtract(Point2D a, Point2D b)
	{
		return { a.X - b.X, a.Y - b.Y };
	}


	Direction2D towards(Point2D other)
	{
		Direction2D result = { 0, 0 };
		if (other.X > X) result.X = 1;
		if (other.X < X) result.X = -1;
		if (other.Y > Y) result.Y = 1;
		if (other.Y < Y) result.Y = -1;

		return result;
	}
};

struct Scale2D
{
	double X, Y;
};

struct Size2D
{
	int Width, Height;
};

struct Rect
{
	Point2D Position;
	Size2D Size;

	bool contains(Point2D point)
	{
		return point.X >= Position.X && point.Y >= Position.Y && point.X < Position.X + Size.Width && point.Y < Position.Y + Size.Height;
	}

	bool overlaps(Rect other, int border = 0)
	{
		for (int x = -border; x < Size.Width + border; x++)
		{
			for (int y = -border; y < Size.Height + border; y++)
			{
				if (other.contains({ Position.X + x, Position.Y + y }))
					return true;
			}
		}

		return false;
	}

	Point2D midpoint()
	{
		return { Position.X + (int)(Size.Width / 2), Position.Y + (int)(Size.Height / 2) };
	}
};

#endif