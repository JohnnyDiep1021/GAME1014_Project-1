#include "PlayScene.h"
#include <algorithm>
#include "Obstacle.h"
#include "Game.h"
#include "EventManager.h"
#include "TextureManager.h"

#include <iostream>
#include <fstream>
#include <string>

#define ENEMYSIGHT 320
#define FPS 60
#define BGSCROLL 2
#define KILLENEMYSCORE 10

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
{
	SoundManager::Instance().stopMusic();
}

void PlayScene::draw()
{
	drawDisplayList();
	m_objPool->DrawActiveSprites();
}

void PlayScene::update()
{
	if (PlayerHealth <= 0)
	{
		gameOver();
		return;
	}

	m_objPool->UpdateActiveSprites();
	updateDisplayList();
	MakeObstacles();
	ScrollBgGround();
	checkCollision();
	EnemyShoot();
	UpdateScoreBoard();
}

void PlayScene::clean()
{
	// clean GameObject
	delete m_pBackground;
	m_pBackground = nullptr;

	delete m_pScoreBoard;
	m_pScoreBoard = nullptr;

	delete m_pPlayer;
	m_pPlayer = nullptr;

	delete m_ground;
	m_ground = nullptr;

	delete m_objPool;
	m_objPool = nullptr;

	for (int i = 0; i < m_vec.size(); i++)
	{
		delete m_vec[i];
		m_vec[i] = nullptr;
	}

	for (int i = 0; i < m_pObstacles.size(); i++)
	{
		// delete m_pObstacles[i];
		m_pObstacles[i] = nullptr;
	}

	for (int i = 0; i < m_pPlayerBulletVec.size(); i++)
	{
		if (!m_pPlayerBulletVec.empty())
		{
			delete m_pPlayerBulletVec[i];
			m_pPlayerBulletVec[i] = nullptr;
		}
	}

	for (int i = 0; i < m_pEnemyBulletVec.size(); i++)
	{
		if (!m_pEnemyBulletVec.empty())
		{
			delete m_pEnemyBulletVec[i];
			m_pEnemyBulletVec[i] = nullptr;
		}
	}

	// clean button
	delete m_pPauseButton;
	m_pPauseButton = nullptr;

	delete m_pContinueButton;
	m_pContinueButton = nullptr;

	removeAllChildren();
}

void PlayScene::handleEvents()
{
	EventManager::Instance().update();

	// handle player movement with GameController
	if (SDL_NumJoysticks() > 0)
	{
		if (EventManager::Instance().getGameController(0) != nullptr)
		{
			const auto deadZone = 10000;
			if (EventManager::Instance().getGameController(0)->LEFT_STICK_X > deadZone)
			{
				m_pPlayer->setAnimationState(PLAYER_RUN_RIGHT);
				m_playerFacingRight = true;
			}
			else if (EventManager::Instance().getGameController(0)->LEFT_STICK_X < -deadZone)
			{
				m_pPlayer->setAnimationState(PLAYER_RUN_LEFT);
				m_playerFacingRight = false;
			}
			else
			{
				if (m_playerFacingRight)
				{
					m_pPlayer->setAnimationState(PLAYER_IDLE_RIGHT);
				}
				else
				{
					m_pPlayer->setAnimationState(PLAYER_IDLE_LEFT);
				}
			}
		}
	}

	// handle player movement if no Game Controllers found
	if (SDL_NumJoysticks() < 1)
	{
		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_A))
		{
			moveX = -1;
			m_pPlayer->getTransform()->position.x += (moveX * moveSpeed);
			m_pPlayer->setAnimationState(PLAYER_RUN_LEFT);
			m_playerFacingRight = false;
		}
		else if (EventManager::Instance().isKeyDown(SDL_SCANCODE_D))
		{
			moveX = 1;
			m_pPlayer->getTransform()->position.x += (moveX * moveSpeed);
			m_pPlayer->setAnimationState(PLAYER_RUN_RIGHT);
			m_playerFacingRight = true;
		}
		else
		{
			if (m_playerFacingRight)
			{
				m_pPlayer->setAnimationState(PLAYER_IDLE_RIGHT);
			}
			else
			{
				m_pPlayer->setAnimationState(PLAYER_IDLE_LEFT);
			}
		}

		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_SPACE) && m_pPlayer->isJumping())
		{
			//m_pPlayer->SetAccelY(-JUMPFORCE);
			if (playerJumping == false)
			{

				playerJumping = true;
				SoundManager::Instance().playSound("jump");
			}

		}

		if (EventManager::Instance().isKeyDown(SDL_SCANCODE_X) && !m_pPlayer->isShooting())
		{
			m_pPlayer->SetShooting(true);
			if (cooldown_specialskill < 3)
			{
				PlayerShoot(PLAYER_BULLET);
				cooldown_specialskill++;
			}
			else
			{
				PlayerShoot(PLAYER_BULLET2);
				cooldown_specialskill = 0;
			}
		}
		else if (EventManager::Instance().isKeyUp(SDL_SCANCODE_X) && m_pPlayer->isShooting())
		{
			m_pPlayer->SetShooting(false);
		}

	}

	m_pPlayer->update();

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_ESCAPE))
	{
		TheGame::Instance()->quit();
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_1))
	{
		TheGame::Instance()->changeSceneState(START_SCENE);
	}

	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_2))
	{
		TheGame::Instance()->changeSceneState(END_SCENE);
	}
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_F))
	{
		if (!Shoted)
		{
			PlayerShoot(PLAYER_BULLET);
			Shoted = true;
		}

	}
	if (EventManager::Instance().isKeyDown(SDL_SCANCODE_G))
	{
		if (!Shoted)
		{
			PlayerShoot(PLAYER_BULLET2);
			Shoted = true;
		}
	}
	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_G))
	{
		//Shoted = false;
	}
	if (EventManager::Instance().isKeyUp(SDL_SCANCODE_F))
	{

		//Shoted = false;
	}

}

void PlayScene::start()
{
	m_level = TheGame::Instance()->getLevel();
	std::cout << "Level: " << m_level << std::endl;
	TheGame::Instance()->setWin(false);

	//Differentiate between levels
	if (m_level == 0)
	{
		m_pBackground = new Background("../Assets/backgrounds/playscene.png", "playscene-background", BACKGROUND, glm::vec2(0, y), false);
		SoundManager::Instance().playMusic("playScreen");
		ReadObstacleFile();
	}
	else if (m_level == 1)
	{
		m_pBackground = new Background("../Assets/backgrounds/playscene2.png", "playscene-background", BACKGROUND, glm::vec2(0, y), false);
		SoundManager::Instance().playMusic("playScreen");
		ReadObstacleFile();
	}
	else
	{
		m_pBackground = new Background("../Assets/backgrounds/playscene2.png", "playscene-background", BACKGROUND, glm::vec2(0, y), false);
		SoundManager::Instance().playMusic("playScreen");
	}

	int windowHeight = TheGame::Instance()->getWindowHeight();
	int windowWidth = TheGame::Instance()->getWindowWidth();

	// Background 
	addChild(m_pBackground);
	
	// Object Pool
	m_objPool = new ObjectPool();

	//Health
	PlayerHealth = 100;
	const SDL_Color yellow1 = { 255, 255, 0, 255 };
	m_pHealth = new ScoreBoard("Health:" + std::to_string(PlayerHealth), "Playbill", 60, yellow1, glm::vec2(100.0f, 80.0f));;
	addChild(m_pHealth);

	//Score Board
	m_pScoreBoard = new ScoreBoard("Score:" + std::to_string(0), "Playbill", 60, yellow1, glm::vec2(windowWidth - 100, 80.0f));;
	m_pScoreBoard->setParent(this);
	addChild(m_pScoreBoard);

	//High Score
	GetHighscore();
	m_pHighscore = new ScoreBoard("High Score:" + std::to_string(m_highscore), "Playbill", 60, yellow1, glm::vec2(windowWidth / 2, 80.0f));;
	m_pHighscore->setParent(this);
	addChild(m_pHighscore);

	// Player Sprite
	m_pPlayer = new Player();
	addChild(m_pPlayer);
	m_playerFacingRight = true;

	// Obstacle Creation 
	m_pObstacles.reserve(4);
	m_vec.reserve(10);
	m_numSpaces = 3;
	for (int i = 0; i < 10; i++)
	{
		m_vec.push_back(new Box(128 * i, windowHeight - 180));
	}
	m_pPlayer->SetJumping(false);

	//Ground
	m_ground = new ground(0, windowHeight - 120);
	addChild(m_ground);

	// Bullets
	m_pPlayerBulletVec.reserve(30);
	m_pEnemyBulletVec.reserve(30);

	// Reset Score for New Level
	TheGame::Instance()->setScore(0);
}

void PlayScene::checkCollision()
{
	if (Shoted)
	{
		CanShoot();
	}
	if (playerJumping)
	{
		PlayerJump();
	}
	int score = TheGame::Instance()->getScore();

	int playerX = m_pPlayer->getTransform()->position.x;
	int playerY = m_pPlayer->getTransform()->position.y;
	int halfPlayerWidth = m_pPlayer->getWidth() * 0.5;
	int halfPlayerHeight = m_pPlayer->getHeight() * 0.5;
	int groundY = m_ground->getTransform()->position.y;

#pragma region // Ground check
	if (playerY > groundY - halfPlayerHeight - 20)
	{
		m_pPlayer->SetJumping(true);
		m_pPlayer->StopY();
		m_pPlayer->setPosition(playerX, groundY - halfPlayerHeight - 15);
		//std::cout << playerY << std::endl;

	}
#pragma endregion

#pragma region // Obstacles check
	for (int i = 0; i < m_pObstacles.size(); i++)
	{
		switch (m_pObstacles[i]->getType())
		{
		case PLATFORM:
		{
			int platformX = m_pObstacles[i]->getTransform()->position.x;
			int platformY = m_pObstacles[i]->getTransform()->position.y;
			int halfPlatformWidth = m_pObstacles[i]->getWidth() * 0.5;
			int halfPlatformHeight = m_pObstacles[i]->getHeight() * 0.5;

			if (playerY < platformY - halfPlatformHeight)
			{
				if ((playerX + halfPlayerWidth < platformX + halfPlatformWidth || playerX - halfPlayerWidth < platformX + halfPlatformWidth)
					&& (playerX + halfPlayerWidth > platformX - halfPlatformWidth || playerX - halfPlayerWidth > platformX - halfPlatformWidth))
				{
					m_pPlayer->SetJumping(true);
					m_pPlayer->StopY();
					m_pPlayer->setPosition(playerX, platformY - halfPlayerHeight - 30);
				}
				else if (playerX > platformX + halfPlatformWidth || playerX < platformX - halfPlatformWidth)
				{
					m_pPlayer->SetJumping(false);
				}
			}
			break;
		}
		case PREDATOR:
		case ENEMY:
		case OBSTACLE4:
		case OBSTACLE5:
		{
			if (COMA::squaredRadiusCheck(m_pPlayer, m_pObstacles[i]) && m_pObstacles[i]->getActive()) // Player and enemy collide
			{
				std::cout << "Player and enemy collide" << std::endl;
				if (PlayerHealth > 0)
				{
					PlayerHealth -= 100 / 4;
				}
				else if (PlayerHealth == 0)
				{
					gameOver();
				}
			}

			BulletCheck(i, score);
			break;
		}
		case OBSTACLE6:
			if (COMA::squaredRadiusCheck(m_pPlayer, m_pObstacles[i]) && m_pObstacles[i]->getActive()) // Player and enemy collide
			{
				std::cout << "Player and enemy collide" << std::endl;
				if (PlayerHealth > 0)
				{
					PlayerHealth -= 100 / 4;
				}
				else if (PlayerHealth == 0)
				{
					gameOver();
				}
			}

			BulletCheck(i, score);
			break;
		case OBSTACLE1:
		{
			if (COMA::squaredRadiusCheck(m_pPlayer, m_pObstacles[i]) && m_pObstacles[i]->getActive()) // Player and enemy collide
			{
				std::cout << "Player and enemy collide" << std::endl;
				if (PlayerHealth > 0)
				{
					PlayerHealth -= 100 / 4;
				}
				else if (PlayerHealth == 0)
				{
					gameOver();
				}
				//PlayerDeath();
			}

			break;
		}
		}
	}
#pragma endregion 

#pragma region // Player Bullet Check
	for (int i = 0; i < m_pPlayerBulletVec.size(); i++)
	{
		// Player bullet leaves screen
		if (m_pPlayerBulletVec[i]->getTransform()->position.x <= -50 ||
			m_pPlayerBulletVec[i]->getTransform()->position.x >= TheGame::Instance()->getWindowWidth())
		{
			m_pPlayerBulletVec[i]->DeactivateSprite();
			m_pPlayerBulletVec.erase(m_pPlayerBulletVec.begin() + i);
			i--;
		}
	}
#pragma endregion

#pragma region // Enemy Bullet Check
	for (int i = 0; i < m_pEnemyBulletVec.size(); i++)
	{
		if (COMA::squaredRadiusCheck(m_pPlayer, m_pEnemyBulletVec[i]))
		{
			std::cout << "Enemy shot player" << std::endl;
			if (PlayerHealth > 0)
			{
				PlayerHealth -= 100 / 4;
			}
			// Remove bullet
			m_pEnemyBulletVec[i]->setActive(false);
			m_pEnemyBulletVec[i] = nullptr;
			m_pEnemyBulletVec.erase(m_pEnemyBulletVec.begin() + i);
		}
		// Enemy bullet leaves screen
		else if (m_pEnemyBulletVec[i]->getTransform()->position.x <= -50 ||
			m_pEnemyBulletVec[i]->getTransform()->position.x >= TheGame::Instance()->getWindowWidth())
		{
			m_pEnemyBulletVec[i]->setActive(false);
			m_pEnemyBulletVec.erase(m_pEnemyBulletVec.begin() + i);
			i--;
		}
	}
#pragma endregion
}

void PlayScene::ScrollBgGround()
{
	m_pBackground->getTransform()->position.x = m_pBackground->getTransform()->position.x - .5f;
	if (m_pBackground->getTransform()->position.x < -1600.f)
	{
		m_pBackground->getTransform()->position.x = 0;
	}


	m_ground->getTransform()->position.x = m_ground->getTransform()->position.x - .5f;

	if (m_ground->getTransform()->position.x < -1600.f)
	{
		m_ground->getTransform()->position.x = 0.f;
	}

	// check left-right edge
	if(m_pPlayer->getTransform()->position.x < 0 + m_pPlayer->getWidth() * 0.7)
	{
		m_pPlayer->setPosition(0 + m_pPlayer->getWidth() * 0.7, m_pPlayer->getTransform()->position.y);
	}

}

void PlayScene::PlayerShoot(BulletType bulletType)
{
	if (Shoted == false)
	{
		float x;
		float y = m_pPlayer->getTransform()->position.y + 24;

		switch (bulletType)
		{
		case PLAYER_BULLET:
		{
			BulletAnimationState bState;

			if (m_playerFacingRight)
			{
				bState = P_BULLET_MOVE_RIGHT;
				x = m_pPlayer->getTransform()->position.x + 80;
			}
			else
			{
				bState = P_BULLET_MOVE_LEFT;
				x = m_pPlayer->getTransform()->position.x - 80;
			}

			m_pPlayerBulletVec.push_back(m_objPool->GetBullet(PLAYER_BULLET));

			int bulletPos = m_pPlayerBulletVec.size() - 1;
			m_pPlayerBulletVec[bulletPos]->setType(P_BULLET);
			m_pPlayerBulletVec[bulletPos]->setPosition(x, y);
			m_pPlayerBulletVec[bulletPos]->setAnimationState(bState);

			SoundManager::Instance().playSound("shot");
		}
		break;
		case PLAYER_BULLET2:
		{
			BulletAnimationState bState;

			if (m_playerFacingRight)
			{
				bState = P2_BULLET_MOVE_RIGHT;
				x = m_pPlayer->getTransform()->position.x + 30;
			}
			else
			{
				bState = P2_BULLET_MOVE_LEFT;
				x = m_pPlayer->getTransform()->position.x - 90;
			}

			m_pPlayerBulletVec.push_back(m_objPool->GetBullet(PLAYER_BULLET2));

			int bulletPos = m_pPlayerBulletVec.size() - 1;
			m_pPlayerBulletVec[bulletPos]->setType(P2_BULLET);
			m_pPlayerBulletVec[bulletPos]->setPosition(x, y);
			m_pPlayerBulletVec[bulletPos]->setAnimationState(bState);
			SoundManager::Instance().playSound("fire");
		}
		break;
		default:
			break;
		}
	}
#pragma endregion 
}

void PlayScene::UpdateScoreBoard()
{
	int score = TheGame::Instance()->getScore();
	m_pHealth->setText("Health:" + std::to_string(PlayerHealth));
	m_pScoreBoard->setText("Score:" + std::to_string(score));

	if (score > m_highscore)
	{
		SetHighscore(score);
		m_pHighscore->setText("High Score:" + std::to_string(m_highscore));
	}
}

void PlayScene::MakeObstacles()
{
	// Check for out of bounds.
	if ((m_vec[0])->GetX() <= -128) // Fully off-screen.
	{
		m_numSpaces++;

		// Add new box.
		if (m_numSpaces >= 4) // Add new sprite if there has been enough spaces.
		{
			m_vec.push_back(new Box(128 * (m_vec.size() + 1), 536));
			
			if ((TheGame::Instance()->getLevel() < 2 && m_numObstacles < m_pReadObstacles.size()) || TheGame::Instance()->getLevel() == 2)
			{
				m_pObstacles.push_back(m_vec.back()->GetObstacle(m_objPool, m_vec.back()->GetX(), 536, m_level, m_numObstacles, m_pReadObstacles));
				m_numObstacles++;
			}
			else
			{
				TheGame::Instance()->setWin(true);
				TheGame::Instance()->changeSceneState(END_SCENE);
			}
			

			if (m_pObstacles.size() > 4 && m_pObstacles[0] != nullptr)
			{
				std::cout << "Remove obstacle" << std::endl;
				m_pObstacles[0]->DeactivateSprite();
				m_pObstacles[0] = nullptr;
				m_pObstacles.erase(m_pObstacles.begin());

				m_objPool->DisplayActiveList();
			}

			m_numSpaces = 0;
		}
		else // Create another empty space
		{
			m_vec.push_back(new Box(128 * m_vec.size(), 536));
		}

		// Remove front element.
		delete m_vec[0];
		m_vec[0] = nullptr;
		m_vec.erase(m_vec.begin());
	}

	// Scroll the boxes.
	for (int col = 0; col < m_vec.size(); col++)
	{
		m_vec[col]->Update();
	}
}

void PlayScene::EnemyShoot()
{
	for (int i = 0; i < m_pObstacles.size(); i++)
	{
		GameObjectType enemyType = m_pObstacles[i]->getType();
		BulletAnimationState bulletAnim;

		if (enemyType == PREDATOR || enemyType == ENEMY && m_pObstacles[i]->getActive())
		{
			Obstacle* enemy = m_pObstacles[i];
			float enemyX = enemy->getTransform()->position.x;
			float enemyY = enemy->getTransform()->position.y;
			float playerX = m_pPlayer->getTransform()->position.x;
			float playerY = m_pPlayer->getTransform()->position.y;
			if ((playerX > enemyX - ENEMYSIGHT - m_pPlayer->getWidth() && playerX < enemyX && playerY >= enemyY) ||
				(playerX < enemyX + ENEMYSIGHT + m_pPlayer->getWidth() && playerX > enemyX && playerX >= enemyY))
			{
				if (playerX > enemyX - ENEMYSIGHT - m_pPlayer->getWidth() && playerX < enemyX && playerY >= enemyY)
				{
					enemyX = m_pObstacles[i]->getTransform()->position.x - 85.0f;
					enemyY = m_pObstacles[i]->getTransform()->position.y + 10.0f;
					//bulletAnim = E_BULLET_MOVE_LEFT;

					if (enemyType == PREDATOR)
					{
						enemy->setAnimationState(PREDATOR_IDLE_LEFT);
						bulletAnim = E_BULLET_MOVE_LEFT;
					}
					else if (enemyType == ENEMY)
					{
						enemy->setAnimationState(ENEMY_IDLE_LEFT);
						bulletAnim = E2_BULLET_MOVE_LEFT;
					}

					if (m_bulletTimer++ == m_timerMax)
					{
						//m_pEnemyBulletVec.push_back(new Bullet(enemyX, enemyY, /*false*/ ENEMY_BULLET, bulletAnim));

						if (enemyType == PREDATOR)
						{
							m_pEnemyBulletVec.push_back(m_objPool->GetBullet(ENEMY_BULLET));
						}
						else if (enemyType == ENEMY)
						{
							m_pEnemyBulletVec.push_back(m_objPool->GetBullet(ENEMY_BULLET2));
						}
						
			

						int bulletPos = m_pEnemyBulletVec.size() - 1;
						
						if (enemyType == PREDATOR)
						{
							m_pEnemyBulletVec[bulletPos]->setType(E_BULLET);
						}
						else if (enemyType == ENEMY)
						{
							m_pEnemyBulletVec[bulletPos]->setType(E2_BULLET);
						}
						
						//m_pEnemyBulletVec[bulletPos]->setType(E_BULLET);
						m_pEnemyBulletVec[bulletPos]->setPosition(enemyX, enemyY);
						m_pEnemyBulletVec[bulletPos]->setAnimationState(bulletAnim);
						//addChild(m_pEnemyBulletVec[m_pEnemyBulletVec.size() - 1]);
						m_bulletTimer = 0;
					}
				}
				else if (playerX < enemyX + ENEMYSIGHT + m_pPlayer->getWidth() && playerX > enemyX && playerX >= enemyY)
				{
					enemyX = m_pObstacles[i]->getTransform()->position.x + 85.0f;
					enemyY = m_pObstacles[i]->getTransform()->position.y + 10.0f;
					//bulletAnim = E_BULLET_MOVE_RIGHT;

					if (enemyType == PREDATOR)
					{
						enemy->setAnimationState(PREDATOR_IDLE_RIGHT);
						bulletAnim = E_BULLET_MOVE_RIGHT;
					}
					else if (enemyType == ENEMY)
					{
						enemy->setAnimationState(ENEMY_IDLE_RIGHT);
						bulletAnim = E2_BULLET_MOVE_RIGHT;
					}

					if (m_bulletTimer++ == m_timerMax)
					{
						//m_pEnemyBulletVec.push_back(new Bullet(enemyX, enemyY, /*false*/ ENEMY_BULLET, bulletAnim));

						if (enemyType == PREDATOR)
						{
							m_pEnemyBulletVec.push_back(m_objPool->GetBullet(ENEMY_BULLET));
						}
						else if (enemyType == ENEMY)
						{
							m_pEnemyBulletVec.push_back(m_objPool->GetBullet(ENEMY_BULLET2));
						}

						int bulletPos = m_pEnemyBulletVec.size() - 1;

						if (enemyType == PREDATOR)
						{
							m_pEnemyBulletVec[bulletPos]->setType(E_BULLET);
						}
						else if (enemyType == ENEMY)
						{
							m_pEnemyBulletVec[bulletPos]->setType(E2_BULLET);
						}
						
						//m_pEnemyBulletVec[bulletPos]->setType(E_BULLET);
						m_pEnemyBulletVec[bulletPos]->setPosition(enemyX, enemyY);
						m_pEnemyBulletVec[bulletPos]->setAnimationState(bulletAnim);
						//addChild(m_pEnemyBulletVec[m_pEnemyBulletVec.size() - 1]);
						m_bulletTimer = 0;
					}
				}

			}
		}
	}

}

void PlayScene::gameOver()
{
	m_pPlayer->setAnimationState(PLAYER_DEATH);
	const SDL_Color yellow1 = { 255, 255, 0, 255 };
	m_pGameOverText = new ScoreBoard("Game Over", "Playbill", 90, yellow1, glm::vec2(500.0f, 300.0f));;
	addChild(m_pGameOverText);
	GameOver = true;

	TheGame::Instance()->changeSceneState(END_SCENE);
}

void PlayScene::ReadObstacleFile()
{
	int i = 0;
	int test;
	if (m_level == 0)
	{
		std::fstream FileOne("../Assets/obstacles1.txt", std::fstream::in);

		if (!FileOne.good())
		{
			std::cout << "Open file has been errored!" << std::endl;
		}
		else
		{
			while (!FileOne.eof())
			{
				FileOne >> test;
				m_pReadObstacles.push_back(test);

				i++;				
			}
		}

		FileOne.close();
	}
	else
	{
		std::ifstream FileTwo("../Assets/obstacles2.txt");

		if (!FileTwo.good())
		{
			std::cout << "Open file has been errored!" << std::endl;
		}
		else
		{
			while (!FileTwo.eof())
			{
				FileTwo >> test;
				m_pReadObstacles.push_back(test);

				i++;
			}
		}
		FileTwo.close();
	}
}

	
void PlayScene::BulletCheck(int i, int score)
{
	// Check for bullet with enemy
	for (int j = 0; j < m_pPlayerBulletVec.size(); j++)
	{

		if (COMA::squaredRadiusCheck(m_pObstacles[i], m_pPlayerBulletVec[j]) && m_pObstacles[i]->getActive())
		{
			// Score
			TheGame::Instance()->setScore(score + KILLENEMYSCORE);
			if (m_pPlayerBulletVec[j]->getType() == P_BULLET)
			{
				if (m_pObstacles[i]->Health < 1)
				{
					m_pObstacles[i]->DeactivateSprite();
					m_pObstacles[i] = nullptr;
					m_pObstacles.erase(m_pObstacles.begin() + i);

					// Remove bullet
					m_pPlayerBulletVec[j]->DeactivateSprite();
					m_pPlayerBulletVec[j] = nullptr;
					m_pPlayerBulletVec.erase(m_pPlayerBulletVec.begin() + j);

				}

				else
				{
					m_pObstacles[i]->Health -= 15;
					m_pPlayerBulletVec[j]->DeactivateSprite();
					m_pPlayerBulletVec[j] = nullptr;
					m_pPlayerBulletVec.erase(m_pPlayerBulletVec.begin() + j);
				}
			}
			else if (m_pPlayerBulletVec[j]->getType() == P2_BULLET)
			{
				m_pObstacles[i]->DeactivateSprite();
				m_pObstacles[i] = nullptr;
				m_pObstacles.erase(m_pObstacles.begin() + i);

				// Remove bullet
				m_pPlayerBulletVec[j]->DeactivateSprite();
				m_pPlayerBulletVec[j] = nullptr;
				m_pPlayerBulletVec.erase(m_pPlayerBulletVec.begin() + j);
			}
		}
	}
}

void PlayScene::GetHighscore()
{
	std::fstream Highscore("../Assets/highscore.txt", std::fstream::in);

	if (!Highscore.good())
	{
		std::cout << "Open file has been errored!" << std::endl;
	}
	else
	{
		Highscore >> m_highscore;
	}

	std::cout << m_highscore << std::endl;

	Highscore.close();
}

void PlayScene::SetHighscore(int score)
{
	std::ofstream NewHighScore("../Assets/highscore.txt.");

	m_highscore = score;
	NewHighScore << m_highscore;

	std::cout << m_highscore << std::endl;

	NewHighScore.close();
}

void PlayScene::CanShoot()
{
	ShootTimer += .1f;
	if (ShootTimer > 5.f)
	{
		Shoted = false;
		ShootTimer = 0;
	}
}

void PlayScene::PlayerJump()
{
	playerJumpTimer();
	//m_pPlayer->SetAccelY(Jumpforce);
	float y = m_pPlayer->getTransform()->position.y;
	m_pPlayer->SetY(y - Jumpforce);

}

void PlayScene::playerJumpTimer()
{
	Jumpforce += 1.f;
	if (Jumpforce > 28.f)
	{
		playerJumping = false;
		Jumpforce = 0;
		m_pPlayer->SetJumping(false);
	}
}
