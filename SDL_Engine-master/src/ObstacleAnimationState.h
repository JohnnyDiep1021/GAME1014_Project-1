#pragma once
#ifndef __ENEMY_ANIMATION_STATE__
#define __ENEMY_ANIMATION_STATE__

enum ObstacleAnimationState
{
	NOT_ENEMY,
	ENEMY_IDLE_RIGHT,
	ENEMY_IDLE_LEFT,
	ENEMY_RUN_RIGHT,
	ENEMY_RUN_LEFT,
	ENEMY_SHOOT_RIGHT,
	ENEMY_SHOOT_LEFT,
	ENEMY_DIE_RIGHT,
	ENEMY_DIE_LEFT,
	NUM_OF_ENEMY_ANIMATION_STATES
};

#endif /* defined (__ENEMY_ANIMATION_STATE__)*/