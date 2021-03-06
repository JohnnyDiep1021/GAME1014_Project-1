#pragma once
#ifndef __END_SCENE__
#define __END_SCENE__

#include "Scene.h"
#include "Label.h"
#include "Button.h"
#include "Background.h"
#include "ScoreBoard.h"
#include "PlayScene.h"


class EndScene final : public Scene
{
public:
	EndScene();
	~EndScene();

	// Inherited via Scene
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

private:
	Label* m_label{};

	Button* m_pRestartButton;
	Button* m_pExitButton;

	Background* m_pBackground;

	ScoreBoard* m_pScoreBoard;
};

#endif /* defined (__END_SCENE__) */