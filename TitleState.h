#pragma once
#include "IGameState.h"

class GameManager;

class TitleState : public IGameState {
public:
	TitleState(GameManager* gameManager) :IGameState(gameManager) {}
	~TitleState() override = default;

	void Init()override;
	SceneTransition* Update(const InputState* input, float deltaTime)override;
	void Draw()override;
	void Terminate()override;

private:
	int m_titleFontHandle;
};