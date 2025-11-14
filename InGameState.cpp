#include "InGameState.h"
#include "GameManager.h"
#include "ResultState.h"
#include "DxLib.h"

void InGameState::Init() {
	SetBackgroundColor(200, 200, 200);
	//ご使用のパソコンに一時的にFontを読み込ませる
	AddFontResourceEx("Data/YDWaosagi.otf", FR_PRIVATE, 0);
	m_gameFontHandle = CreateFontToHandle("YDW あおさぎ R", 25, 3);
	//m_springManager = std::make_shared<SpringManager>();
	//m_spring_GravityManager = std::make_shared<SpringAndGravityManager>();
	//m_stringPointManager = std::make_shared<StringPointManager>(320.0f, 100.0f, 10, 15.0f, 200.0f, 5.0f);
	m_PBDSimulation = std::make_shared<PBDSimulation>();
	//m_PBDSimulation2 = std::make_shared<PBDSimulation>();
	//m_PBDSimulation2->changeVersion = true;
}

SceneTransition* InGameState::Update(const InputState* input, float deltaTime) {
	//Spaceを押したときはゲームシーンへ移行する
	if (input->IsKeyDown(KEY_INPUT_SPACE)) {
		SceneTransition* trans = new SceneTransition{ TransitionType::Change,
			std::make_unique<ResultState>(m_gameManager) };
		return trans;
	}

	//m_springManager->Update(input, deltaTime);
	//m_spring_GravityManager->Update(input, deltaTime);
	//m_stringPointManager->Update();
	m_PBDSimulation->Update(input);
	//m_PBDSimulation2->Update(input);

	SceneTransition* trans = new SceneTransition{ TransitionType::None, nullptr };
	return trans;
}

void InGameState::Draw() {
	GameData::DrawStringWithAnchor(100, GameData::windowHeight / 2, 0, 0.5f,
		GetColor(255, 255, 255), m_gameFontHandle, "Press Space \nto See Result");

	//m_springManager->Draw();
	//m_spring_GravityManager->Draw();
	//m_stringPointManager->Draw();
	//m_PBDSimulation2->Draw();
	m_PBDSimulation->Draw();
}

void InGameState::Terminate() {
	//読み込ませたフォントを開放する
	RemoveFontResourceEx("Data/YDWaosagi.otf", FR_PRIVATE, 0);
}