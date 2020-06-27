#pragma once
#ifndef __PLAYER__
#define __PLAYER__

#include "PlayerAnimationState.h"
#include "Sprite.h"
#include "Bullet.h"
#include <vector>

#define GRAV 5.5
#define JUMPFORCE 90.0

class Player final : public Sprite
{
public:
	Player();
	~Player();

	// Life Cycle Methods
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;

	// setters
	void setAnimationState(PlayerAnimationState new_state);
	void setPosition(float x, float y);

	// Movement
	void Stop();
	void StopX();
	void StopY();
	void SetAccelX(double a);
	void SetAccelY(double a);
	bool isJumping();
	void SetJumping(bool j);
	double GetVelX();
	double GetVelY();

	// Shooting
	bool isShooting();
	void SetShooting(bool s);

private:
	void m_buildAnimations();
	PlayerAnimationState m_currentAnimationState;
	SDL_FRect dst;

	bool m_jumping;
	bool m_shooting;
	double m_accelX, m_accelY, m_velX, m_maxVelX, m_velY, m_maxVelY, m_drag, m_grav;
};

#endif /* defined (__PLAYER__) */