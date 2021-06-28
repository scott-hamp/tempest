#pragma once

#ifndef Enums_H
#define Enums_H

enum InputCommand
{
	InputCommand_None,
	InputCommand_Examine
};

enum MapObjectInteraction
{
	MapObjectInteraction_Ascend,
	MapObjectInteraction_Attack,
	MapObjectInteraction_Descend,
	MapObjectInteraction_Move
};

enum MapTileTerrain
{
	MapTileTerrain_Grass,
	MapTileTerrain_Stone
};

enum MapTileType
{
	MapTileType_Empty,
	MapTileType_Floor,
	MapTileType_Wall
};

enum PassableType
{
	PassableType_Solid,
	PassableType_Light
};

#endif