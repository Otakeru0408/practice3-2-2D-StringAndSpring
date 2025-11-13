#pragma once
#include <memory>
#include <stack>
#include "IGameState.h"
#include "InputState.h"

class TitleState;
class InGameState;
class ResultState;

class GameManager {
public:
	GameManager();
	~GameManager();

	void Initialize();
	void Update();
	void Draw();
	void Finalize();

	// ★追加: ゲームが終了状態かを確認するメソッド
	bool IsGameFinished() const
	{
		return m_isGameFinished;
	}
	// ★追加: ゲーム終了フラグをセットするメソッド
	void RequestGameFinish()
	{
		m_isGameFinished = true;
	}

private:
	//std::unique_ptr<IGameState> m_currentState;
	bool m_isGameFinished; // ★追加: ゲーム終了フラグ
	InputState m_inputState;
	float m_prevTime = 0;

	void UpdateInputState();

	std::stack<std::unique_ptr<IGameState>> m_currentState;

	void PushState(std::unique_ptr<IGameState> scene) {
		m_currentState.push(std::move(scene));
	}
	void PopState() {
		if (!m_currentState.empty()) {
			m_currentState.pop();
		}
	}

	IGameState* currentState() {
		return m_currentState.empty() ? nullptr : m_currentState.top().get();
	}
};