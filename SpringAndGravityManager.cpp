#include "SpringAndGravityManager.h"

SpringAndGravityManager::SpringAndGravityManager() {
	InitializeSpring();
}

void SpringAndGravityManager::Update(const InputState* input, float deltaTime) {
	// 外部からの力の適用 (キー入力)
	ApplyExternalForce();

	// 1. 変位 (自然長からの伸びまたは縮み) を計算
	// 現在の長さ: lowerPoint.y - FIXED_Y
	// 伸び x は、現在の長さ - 自然長。下方向（Y+）が伸びの正方向。
	double currentLength = lowerPoint.y - FIXED_Y;
	double x = currentLength - NATURAL_LENGTH;

	// 2. バネの力 (フックの法則: F_spring = -k * x)
	// 伸び(x>0)の場合、F_springは上向き（Y-）になるため、符号はマイナス。
	double F_spring = -SPRING_CONSTANT * x;

	// 3. 減衰力 (抵抗力: F_damp = -d * v)
	// 速度と逆向き
	double F_damp = -DAMPING_CONSTANT * lowerPoint.v;

	// 4. 重力 (F_gravity = M * G)
	// 重力は常に下向き（Y軸正方向）に働く
	double F_gravity = MASS * GRAVITY_ACCEL;

	// 5. 合力 (F = F_spring + F_damp + F_gravity + F_external)
	// F_external (キー入力) は ApplyExternalForce 関数内で加速度に適用済みとする
	double F_net = F_spring + F_damp + F_gravity;

	// 6. ニュートンの運動方程式 (a = F / m)
	// F_net を用いて加速度を更新
	lowerPoint.a += F_net / MASS;

	// 7. 速度と位置の更新 (オイラー法)
	lowerPoint.v += lowerPoint.a * deltaTime;
	lowerPoint.y += lowerPoint.v * deltaTime;

	// 8. 加速度のリセット（次のフレームの合力計算に備える）
	lowerPoint.a = 0.0;
}

void SpringAndGravityManager::Draw() {
	const int RADIUS = 10;

	// 上側の固定点（赤色）
	DrawCircle(FIXED_X, FIXED_Y, RADIUS, GetColor(255, 0, 0), TRUE);

	// 下側の動く点（青色）
	DrawCircle(FIXED_X, (int)lowerPoint.y, RADIUS, GetColor(0, 0, 255), TRUE);

	// バネ（緑色の直線として簡易的に描画）
	DrawLine(FIXED_X, FIXED_Y + RADIUS, FIXED_X, (int)lowerPoint.y - RADIUS, GetColor(0, 255, 0));

	// 情報を表示
	DrawFormatString(10, 10, GetColor(255, 255, 255), "W/Sキーで上下に力を加える");
	DrawFormatString(10, 30, GetColor(255, 255, 255), "Y位置: %.2f", lowerPoint.y);
	DrawFormatString(10, 50, GetColor(255, 255, 255), "速度V: %.2f", lowerPoint.v);
}

void SpringAndGravityManager::InitializeSpring() {
	// 初期位置を自然長の位置に設定
	lowerPoint.y = (double)FIXED_Y + NATURAL_LENGTH;
	lowerPoint.v = 0.0;
	lowerPoint.a = 0.0;
}

void SpringAndGravityManager::ApplyExternalForce() {
	// Wキーで上へ力を加える（Y軸負方向）
	if (CheckHitKey(KEY_INPUT_W)) {
		lowerPoint.a -= EXTERNAL_ACCEL;
	}
	// Sキーで下へ力を加える（Y軸正方向）
	if (CheckHitKey(KEY_INPUT_S)) {
		lowerPoint.a += EXTERNAL_ACCEL;
	}
	// D/Aキーの処理は削除
}