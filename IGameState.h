#pragma once
#include "DxLib.h"
#include "GameData.h"
#include <string>

#include <cstdarg>
#include <memory>

class GameManager;
class IGameState;
struct InputState;

enum GameCondition
{
	Playing,
	GameClear,
	GameOver,
};

// シーンの遷移を管理するenum
enum class TransitionType {
	None,       // 変化なし
	Change,     // 新しいシーンに遷移
	Resume,     // 一時停止から再開
	Popup,		//一時的に別のシーンにする
};

// 遷移情報を保持する構造体
struct SceneTransition {
	TransitionType type;
	std::unique_ptr<IGameState> nextState;
};

class IGameState {
public:
	IGameState(GameManager* gameManager) : m_gameManager(gameManager) {}

	virtual ~IGameState() = default;

	virtual void Init() = 0;
	virtual SceneTransition* Update(const InputState* input, float deltaTime) = 0;
	virtual void Draw() = 0;
	virtual void Terminate() = 0;

protected:
	GameManager* m_gameManager;
};
