#pragma once

#ifndef Enums_H
#define Enums_H

enum InputCommand
{
	InputCommand_None,
	InputCommand_Examine,
	InputCommand_Fire,
	InputCommand_OpenClose,
	InputCommand_WearWield
};

enum MapAnimationType
{
	MapAnimationType_Simple,
	MapAnimationType_Missle
};

enum MapObjectInteraction
{
	MapObjectInteraction_Ascend,
	MapObjectInteraction_Attack,
	MapObjectInteraction_Descend,
	MapObjectInteraction_DumpInventory,
	MapObjectInteraction_Move,
	MapObjectInteraction_OpenClose,
	MapObjectInteraction_OpenCloseOther,
	MapObjectInteraction_PickUp,
	MapObjectInteraction_WearWield
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

enum UIPanel
{
	UIPanel_Equipment,
	UIPanel_Inventory,
	UIPanel_Log,
	UIPanel_Stats
};

#endif