#pragma once
#include "DxLib.h"
#include "InputState.h"

// --- 質点構造体 ---
struct Point {
	double x; // 位置
	double v; // 速度
	double a; // 加速度
};

class SpringManager {
public:
	SpringManager();
	~SpringManager() {}

	void Update(const InputState* input, float deltaTime);
	void Draw();

private:
	// --- 関数プロトタイプ宣言 ---
	void InitializeSpring();
	void ApplyExternalForce(const InputState* input);
	// --- 入力処理と状態の切り替え、位置の強制移動を行う関数 ---
	void HandleInput();

	// --- グローバル変数 ---
	Point rightPoint;

	// --- 定数定義 ---
// 質量の大きさ (kg)
	const float MASS = 1.0f;
	// ばね定数 (N/m)
	const float SPRING_CONSTANT = 10.0f;
	// 減衰定数 (空気抵抗などをシミュレート)
	const float DAMPING_CONSTANT = 1.0f;
	// バネの自然長 (pixels)
	const int NATURAL_LENGTH = 200;

	// 固定ポイントの座標
	const int FIXED_X = 100;
	const int FIXED_Y = 200;

	// 初期位置のオフセット（自然長の位置）
	const int INITIAL_X_OFFSET = 0;

	// ★追加：シミュレーションの状態を管理するフラグ
	bool isSimulationActive = true;

	// 外部の力の強さ（加速度として表現）
	const float EXTERNAL_ACCEL = 1000.0f;
	// 物理演算の刻み時間
	const float DELTA_TIME = 0.01f;
};