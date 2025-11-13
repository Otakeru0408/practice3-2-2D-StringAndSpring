#pragma once
#include "IGameState.h"
//#include "SpringManager.h"
//#include "SpringAndGravityManager.h"
#include "StringPointManager.h"

class GameManager;

class InGameState :public IGameState {
public:
	InGameState(GameManager* gameManager) :IGameState(gameManager) {}
	~InGameState()override = default;

	void Init()override;
	SceneTransition* Update(const InputState* input, float deltaTime)override;
	void Draw()override;
	void Terminate()override;

private:
	int m_gameGraphHandle;
	int m_gameFontHandle;
	//std::shared_ptr<SpringManager> m_springManager;
	//std::shared_ptr<SpringAndGravityManager> m_spring_GravityManager;
	std::shared_ptr<StringPointManager> m_stringPointManager;
};