#pragma once
#ifndef __GAME_OBJECT_TYPE__
#define __GAME_OBJECT_TYPE__
enum GameObjectType {
	NONE = -1,
	SHIP,
	TARGET,
	PLAYER,
	ENEMY,
	PREDATOR,
	P_BULLET,
	P2_BULLET,
	E_BULLET,
	OBSTACLE1,
	OBSTACLE2,
	OBSTACLE3,
	OBSTACLE4,
	OBSTACLE5,
	OBSTACLE6,
	PLATFORM,
	BACKGROUND,
	GROUND,
	START_BUTTON,
	RESTART_BUTTON,
	EXIT_BUTTON,
	LEVEL1_BUTTON,
	LEVEL2_BUTTON,
	LEVEL3_BUTTON,
	PAUSE_BUTTON,
	CONTINUE_BUTTON,
	NEXT_BUTTON,
	NUM_OF_TYPES
};
#endif /* defined (__GAME_OBJECT_TYPE__) */