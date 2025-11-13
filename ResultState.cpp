#include "GameManager.h"
#include "DxLib.h"
#include "ResultState.h"

void ResultState::Init() {
	//様々なもののロード
	SetBackgroundColor(255, 255, 255);
	//ご使用のパソコンに一時的にFontを読み込ませる
	AddFontResourceEx("Data/YDWaosagi.otf", FR_PRIVATE, 0);
	m_resultFontHandle = CreateFontToHandle("YDW あおさぎ R", 30, 5);
}

SceneTransition* ResultState::Update(const InputState* input, float deltaTime) {
	//Spaceを押したときはゲームシーンへ移行する
	if (input->IsKeyDown(KEY_INPUT_SPACE)) {
		SceneTransition* trans = new SceneTransition{ TransitionType::Change,
			std::make_unique<InGameState>(m_gameManager) };
		return trans;
	}

	SceneTransition* trans = new SceneTransition{ TransitionType::None, nullptr };
	return trans;
}

void ResultState::Draw() {
	GameData::DrawStringWithAnchor(50, 100, 0, 0.5f,
		GetColor(0, 0, 0), m_resultFontHandle, "Escape : terminate game");
}

void ResultState::Terminate() {
	//様々なもののアンロード
	//読み込ませたフォントを開放する
	RemoveFontResourceEx("Data/YDWaosagi.otf", FR_PRIVATE, 0);
}