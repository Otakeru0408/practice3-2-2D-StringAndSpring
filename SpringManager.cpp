#include "SpringManager.h"

SpringManager::SpringManager() {
	InitializeSpring();
}

void SpringManager::Update(const InputState* input, float deltaTime) {
	//入力処理と状態の切り替え
	//HandleInput();

	if (!isSimulationActive)return;

	ApplyExternalForce(input);

	// 1. 変位 (自然長からの伸びまたは縮み) を計算
	// 変位 x は、現在の長さ - 自然長
	double currentLength = rightPoint.x - FIXED_X;
	double x = currentLength - NATURAL_LENGTH;

	// 2. バネの力 (フックの法則: F_spring = -k * x)
	double F_spring = -SPRING_CONSTANT * x;

	// 3. 減衰力 (抵抗力: F_damp = -d * v)
	double F_damp = -DAMPING_CONSTANT * rightPoint.v;

	// 4. 合力 (F = F_spring + F_damp + F_external)
	// 外部の力 (F_external) は ApplyExternalForce 関数内で加速度に適用済みとする
	double F_net = F_spring + F_damp;

	// 5. ニュートンの運動方程式 (a = F / m)
	// F_net を用いて加速度を更新
	rightPoint.a += F_net / MASS;

	// 6. 速度と位置の更新 (オイラー法)
	rightPoint.v += rightPoint.a * deltaTime;
	rightPoint.x += rightPoint.v * deltaTime;

	// 7. 加速度のリセット（次のフレームの合力計算に備える）
	rightPoint.a = 0.0;
}

void SpringManager::Draw() {
	// 質点の半径
	const int RADIUS = 10;

	// 左側の固定点（赤色）
	DrawCircle(FIXED_X, FIXED_Y, RADIUS, GetColor(255, 0, 0), TRUE);

	// 右側の動く点（青色）
	DrawCircle((int)rightPoint.x, FIXED_Y, RADIUS, GetColor(0, 0, 255), TRUE);

	// バネ（緑色の直線として簡易的に描画）
	DrawLine(FIXED_X + RADIUS, FIXED_Y, (int)rightPoint.x - RADIUS, FIXED_Y, GetColor(0, 255, 0));

	// 情報を表示
	DrawFormatString(10, 10, GetColor(255, 255, 255), "A/Dキーで力を加える");
	DrawFormatString(10, 30, GetColor(255, 255, 255), "X位置: %.2f", rightPoint.x);
	DrawFormatString(10, 50, GetColor(255, 255, 255), "速度V: %.2f", rightPoint.v);
}

void SpringManager::InitializeSpring() {
	rightPoint.x = (double)FIXED_X + NATURAL_LENGTH + INITIAL_X_OFFSET; // 自然長+初期オフセット
	rightPoint.v = 0.0;
	rightPoint.a = 0.0;
}

void SpringManager::ApplyExternalForce(const InputState* input) {
	// 外部からの力によって加速度を直接加える
	const double EXTERNAL_ACCEL = 500.0; // 外部の力の強さ（加速度として表現）

	// Dキーで右へ力を加える
	if (input->IsKeyStay(KEY_INPUT_D)) {
		rightPoint.a += EXTERNAL_ACCEL;
	}
	// Aキーで左へ力を加える
	if (input->IsKeyStay(KEY_INPUT_A)) {
		rightPoint.a -= EXTERNAL_ACCEL;
	}
}

// --- 入力処理と状態の切り替え、位置の強制移動を行う関数 ---
void SpringManager::HandleInput() {
	int MouseX, MouseY;
	GetMousePoint(&MouseX, &MouseY);
	int mouseInput = GetMouseInput();
	bool isLClick = (mouseInput & MOUSE_INPUT_LEFT) != 0;

	bool isKeyActive = CheckHitKey(KEY_INPUT_D) || CheckHitKey(KEY_INPUT_A);

	// --- 優先度1: マウス操作 ---
	if (isLClick) {
		isSimulationActive = false; // シミュレーション停止

		// 位置をマウスX座標に強制設定
		rightPoint.x = (double)MouseX;

		// 速度と加速度をリセット（手を離した瞬間に静止状態から動き出すため）
		rightPoint.v = 0.0;
		rightPoint.a = 0.0;

		// --- 優先度2: A/Dキー操作 ---
	}
	else if (isKeyActive) {
		isSimulationActive = false; // シミュレーション停止

		// キー制御はUpdateSpringが呼ばれないため、この関数内で直接質点を動かす

		// 速度と加速度をリセット（キーを押した瞬間に前の慣性を消す）
		// ※ 慣性を残したい場合はこのリセットを削除
		rightPoint.v = 0.0;
		rightPoint.a = 0.0;

		// 押されたキーに応じて加速度を適用
		if (CheckHitKey(KEY_INPUT_D)) {
			rightPoint.a += EXTERNAL_ACCEL;
		}
		if (CheckHitKey(KEY_INPUT_A)) {
			rightPoint.a -= EXTERNAL_ACCEL;
		}

		// 加速度による位置と速度の更新
		rightPoint.v += rightPoint.a * DELTA_TIME;
		rightPoint.x += rightPoint.v * DELTA_TIME;

		// --- 優先度3: 操作なし ---
	}
	else {
		// マウスもキーも押されていない場合、シミュレーション再開
		isSimulationActive = true;
	}
}