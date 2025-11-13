#pragma once
#include "DxLib.h"
#include "InputState.h"

class SpringAndGravityManager {
public:
	SpringAndGravityManager();
	~SpringAndGravityManager() {}

	void Update(const InputState* input, float deltaTime);
	void Draw();
private:

	// --- 関数プロトタイプ宣言 ---
	void InitializeSpring();
	void ApplyExternalForce(); // W/Sキーの力を適用

	// --- 定数定義 ---
	const double MASS = 1.0;
	const double SPRING_CONSTANT = 5.0;
	const double DAMPING_CONSTANT = 0.5;
	// バネの自然長 (pixels)
	const int NATURAL_LENGTH = 150;
	// 重力加速度 (pixels/s^2) - 描画に合わせて大きめに設定
	const double GRAVITY_ACCEL = 9.8 * 50;

	// 固定ポイントの座標（上側）
	const int FIXED_X = 320;
	const int FIXED_Y = 100;

	// 外部の力の強さ（加速度として表現）
	const double EXTERNAL_ACCEL = 500.0;

	// --- 質点構造体 (Y軸方向の動きに特化) ---
	struct Point {
		double y; // Y位置
		double v; // 速度
		double a; // 加速度
	};

	// --- グローバル変数 ---
	Point lowerPoint;

};
