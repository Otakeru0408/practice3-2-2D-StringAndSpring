#pragma once
#include "DxLib.h"
#include <cmath>
#include <vector>

// 2次元ベクトル/座標構造体
struct Vec2f {
	float x;
	float y;

	// オペレーターオーバーロード (計算を簡略化)
	Vec2f operator+(const Vec2f& other) const { return { x + other.x, y + other.y }; }
	Vec2f operator-(const Vec2f& other) const { return { x - other.x, y - other.y }; }
	Vec2f operator*(float scalar) const { return { x * scalar, y * scalar }; }
	// オペレーターオーバーロード (ベクトルの長さ)
	float lengthSq() const { return x * x + y * y; }
	float length() const { return std::sqrt(lengthSq()); }
};


class StringPointManager {
private:
	std::vector<Vec2f> ropeNodes; // 紐を構成するノード群

	float segmentLength;      // 各セグメント（ノード間）の理想的な長さ
	float maxStretchDistance; // 紐全体の伸びる限界距離 (Node 0 と Node N の間の距離)
	float moveSpeed;          // ノード 0 (操作点) の移動速度
	const int CONSTRAINT_ITERATIONS = 3; // 拘束処理の反復回数 (多いほど安定)

	unsigned int lineColor;
	unsigned int controllableColor;
	unsigned int fixedColor;
	int pointRadius;

public:
	/**
	 * @brief コンストラクタ
	 * @param startX, startY 初期座標
	 * @param nodeCount 紐の節の数 (2点を含む、最小2)
	 * @param segLen 各節の長さ
	 * @param maxDist 紐全体の限界距離
	 * @param speed 操作点の移動速度
	 */
	StringPointManager(float startX, float startY, int nodeCount, float segLen, float maxDist, float speed)
		: segmentLength(segLen), maxStretchDistance(maxDist), moveSpeed(speed)
	{
		if (nodeCount < 2) nodeCount = 2;

		// ノードを初期位置 (垂直に配置を仮定) に設定
		for (int i = 0; i < nodeCount; ++i) {
			ropeNodes.push_back({ startX, startY + (float)i * segLen });
		}

		// 色と描画設定
		lineColor = GetColor(255, 255, 255);
		controllableColor = GetColor(0, 255, 0); // Node 0
		fixedColor = GetColor(255, 0, 0);       // Node N
		pointRadius = 10;
	}

	void Update() {
		handleInputAndUpdateControllablePoint();
		updateRope();
	}

	/**
	 * @brief WASDキー入力によりノード 0 (操作点) の位置を更新
	 */
	void handleInputAndUpdateControllablePoint() {
		// Node 0 はユーザーが操作するポイント
		Vec2f& controllablePoint = ropeNodes[0];

		// 水平方向の移動
		if (CheckHitKey(KEY_INPUT_A)) {
			controllablePoint.x -= moveSpeed;
		}
		if (CheckHitKey(KEY_INPUT_D)) {
			controllablePoint.x += moveSpeed;
		}

		// 垂直方向の移動
		if (CheckHitKey(KEY_INPUT_W)) {
			controllablePoint.y -= moveSpeed;
		}
		if (CheckHitKey(KEY_INPUT_S)) {
			controllablePoint.y += moveSpeed;
		}

		// (画面境界処理は省略)
	}

	/**
	 * @brief 紐全体の動きと拘束条件を更新します
	 */
	void updateRope() {
		if (ropeNodes.empty()) return;

		// Node 0 (操作点) の位置
		Vec2f& controllablePoint = ropeNodes[0];
		// Node N (追従点、末尾) の位置
		Vec2f& lastFixedPoint = ropeNodes.back();
		const int N = ropeNodes.size() - 1;

		// ----------------------------------------------------
		// フェーズ 1: 紐全体のグローバルな拘束チェック (元のロジック)
		// ----------------------------------------------------

		// Node 0 と Node N の間の距離をチェック
		float totalDistance = (controllablePoint - lastFixedPoint).length();
		bool isRopeTaut = (totalDistance > maxStretchDistance);

		if (isRopeTaut) {
			// 紐が伸びる限界を超えた場合、Node N を Node 0 から限界距離の位置に移動させる
			Vec2f direction = normalize(lastFixedPoint - controllablePoint);
			lastFixedPoint = controllablePoint + direction * maxStretchDistance;
		}

		// ----------------------------------------------------
		// フェーズ 2: ノード間のセグメント長拘束 (たるみ/追従のシミュレーション)
		// ----------------------------------------------------

		// 拘束処理を複数回反復することで安定した動きを実現する (Position-Based Dynamicsの簡易版)
		for (int k = 0; k < CONSTRAINT_ITERATIONS; ++k) {

			// 拘束処理は Node 1 から Node N まで行う
			for (int i = 1; i <= N; ++i) {
				// p1 (Node i) と p2 (Node i-1) の間で処理
				Vec2f& p1 = ropeNodes[i];
				Vec2f& p2 = ropeNodes[i - 1];

				Vec2f diff = p1 - p2;
				float currentDist = diff.length();

				// 修正量 (どれだけ動かせばセグメント長になるか)
				float correctionFactor = (currentDist - segmentLength) / currentDist;
				Vec2f correctionVector = diff * correctionFactor;

				// 1. Node i (p1) の移動:
				// Node N がロックされている間は、Node N は動かせないため、
				// 末尾のノード (i=N) は、全体の拘束が解かれていない限り動かしません。
				// ただし、Phase 1 で Node N の位置は既に強制修正されているため、
				// ここでは Node N が動くことを許可し、その動きを Node N-1 に伝播させます。

				// Node 0 はユーザー入力で固定されているため、すべての修正を p1 に与える
				if (i == 1) {
					p1 = p1 - correctionVector;
				}
				// それ以外のノードは、p1 と p2 の両方に修正を分割する
				else {
					p1 = p1 - correctionVector * 0.5f;
					// p2 は Node 0 以外であれば動く
					p2 = p2 + correctionVector * 0.5f;
				}
			}

			// 重要: Node 0 はユーザーによって操作された位置に、
			// Node N はグローバル拘束によって修正された位置に、それぞれリセットする
			ropeNodes[0] = controllablePoint;
			if (isRopeTaut) {
				// Node N がグローバル拘束で動かされた場合、その位置を維持
				ropeNodes[N] = lastFixedPoint;
			}
		}
	}

	/**
	 * @brief 紐とノードを描画します
	 */
	void Draw() const {
		if (ropeNodes.empty()) return;

		// 1. 紐 (線) の描画
		// Node i と Node i+1 を結ぶ線を描画
		for (size_t i = 0; i < ropeNodes.size() - 1; ++i) {
			const Vec2f& p1 = ropeNodes[i];
			const Vec2f& p2 = ropeNodes[i + 1];
			DrawLine(
				(int)p1.x,
				(int)p1.y,
				(int)p2.x,
				(int)p2.y,
				lineColor,
				5
			);
		}

		// 2. ノードの描画
		for (size_t i = 0; i < ropeNodes.size(); ++i) {
			unsigned int color = lineColor;
			int smallRange = 5;
			if (i == 0) {
				color = controllableColor; // Node 0 (操作点)
				smallRange = 0;
			}
			else if (i == ropeNodes.size() - 1) {
				color = fixedColor; // Node N (追従点/固定点)
				smallRange = 0;
			}
			else {
				//continue;
			}

			DrawCircle(
				(int)ropeNodes[i].x,
				(int)ropeNodes[i].y,
				pointRadius - smallRange,
				color,
				TRUE
			);
		}
	}


	// ヘルパー関数: ベクトルを正規化 (長さを1にする)
	Vec2f normalize(const Vec2f& v) {
		float len = v.length();
		if (len == 0.0f) return { 0.0f, 0.0f };
		return { v.x / len, v.y / len };
	}
};

// --- 使用例 (メインループ) ---
/*
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	ChangeWindowMode(TRUE);
	SetGraphMode(640, 480, 32);
	if (DxLib_Init() == -1) return -1;
	SetDrawScreen(DX_SCREEN_BACK);

	// インスタンス化:
	// 始点(320, 100), ノード数 10, セグメント長 15, 限界距離 150, 移動速度 5.0
	StringPointManager manager(320.0f, 100.0f, 10, 15.0f, 150.0f, 5.0f);

	// メインループ
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
		ClearDrawScreen();

		// 1. 入力処理
		manager.handleInputAndUpdateControllablePoint();

		// 2. 紐の更新 (たるみ・追従処理)
		manager.updateRope();

		// 3. 描画
		manager.draw();

		ScreenFlip();
	}

	DxLib_End();
	return 0;
}
*/