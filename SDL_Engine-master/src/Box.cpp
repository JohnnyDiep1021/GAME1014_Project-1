#include "Box.h"
#include "Obstacle.h"
#include"Background.h"
#include "Game.h"

#include <iostream>

#define SCROLLSPD 2.6

#define PLATFORMY 600
#define ENEMYY 715
#define SPIKEY 725

Box::Box(float x, float y)
{
	
	m_x = x;
	m_y = y;
	m_obstacle = nullptr;
}

Box::~Box()
{
	//if (m_obstacle != nullptr)
	//{
	//	m_obstacle->DeactivateSprite();
	//	m_obstacle = nullptr;
	//}
}

void Box::Update()
{
	m_x -= SCROLLSPD;
	int windowHeight = TheGame::Instance()->getWindowHeight();
	if (m_obstacle != nullptr)
	{
		m_obstacle->getTransform()->position = glm::vec2(m_x, windowHeight / 2 + 208);
	}
}

void Box::Render()
{
}

int Box::GetX()
{
	return m_x;
}

Obstacle* Box::GetSprite()
{
	return m_obstacle;
}

Obstacle* Box::GetRandomObstacle(ObjectPool* objPool, int x, int y)
{
	int randNumber = (rand() % 8) + 1;

	Obstacle* sprite = nullptr;

	switch (randNumber)
	{
	case 1:
		std::cout << "Get Random Obstacle - Platform" << std::endl;
		sprite = objPool->GetObstacle(PLATFORM);
		sprite->getTransform()->position = glm::vec2(x, y);
		m_y = PLATFORMY;
		break;
	case 2:
		std::cout << "Get Random Obstacle - Predator" << std::endl;
		sprite = objPool->GetObstacle(PREDATOR);
		sprite->getTransform()->position = glm::vec2(x, y);
		m_y = ENEMYY;
		break;
	case 3:
		std::cout << "Get Random Obstacle - Alien" << std::endl;
		sprite = objPool->GetObstacle(ENEMY);
		sprite->getTransform()->position = glm::vec2(x, y);
		m_y = ENEMYY;
		break;
	case 4:
	case 8:
		std::cout << "Get Random Obstacle - Obstacle1" << std::endl;
		sprite = objPool->GetObstacle(OBSTACLE1);
		sprite->getTransform()->position = glm::vec2(x, y);
		m_y = SPIKEY;
		break;
	case 5:
		std::cout << "Get Random Obstacle - Obstacle4" << std::endl;
		sprite = objPool->GetObstacle(OBSTACLE4);
		sprite->getTransform()->position = glm::vec2(x, y);
		m_y = SPIKEY;
		break;
	case 6:
		std::cout << "Get Random Obstacle - Obstacle5" << std::endl;
		sprite = objPool->GetObstacle(OBSTACLE5);
		sprite->getTransform()->position = glm::vec2(x, y);
		m_y = SPIKEY;
		break;
	case 7:
		std::cout << "Get Random Obstacle - Obstacle6" << std::endl;
		sprite = objPool->GetObstacle(OBSTACLE6);
		sprite->getTransform()->position = glm::vec2(x, y);
		m_y = SPIKEY;
		break;
	}
	m_obstacle = sprite;
	return sprite;
}