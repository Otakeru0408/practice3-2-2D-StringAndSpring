#pragma once
#include <vector>
#include <cmath>
#include <algorithm> // std::max, std::min用
#include "DxLib.h"   // DxLibの関数を使用
#include "GameData.h"

// 2次元ベクトル/座標構造体 (ご提示いただいたもの)
struct Vec2F {
	float x;
	float y;

	// オペレーターオーバーロード (計算を簡略化)
	Vec2F operator+(const Vec2F& other) const { return { x + other.x, y + other.y }; }
	Vec2F operator-(const Vec2F& other) const { return { x - other.x, y - other.y }; }
	Vec2F operator*(float scalar) const { return { x * scalar, y * scalar }; }

	// **追加:** スカラー除算 (質量やdtの逆数で割る際に便利)
	Vec2F operator/(float scalar) const {
		float inv_s = 1.0f / scalar;
		return { x * inv_s, y * inv_s };
	}

	// **追加:** 複合代入演算子 (p += dp; のように使う)
	Vec2F& operator+=(const Vec2F& other) { x += other.x; y += other.y; return *this; }

	// オペレーターオーバーロード (ベクトルの長さ)
	float lengthSq() const { return x * x + y * y; }
	float length() const { return std::sqrt(lengthSq()); }

	// **追加:** 正規化 (単位ベクトルを求める。制約の法線方向の計算に必須)
	Vec2F normalize() const {
		float len = length();
		return (len > 0) ? *this / len : Vec2F{ 0.0f, 0.0f };
	}
};

// ノード（質点）の構造体
struct Node {
	Vec2F position;        // **PBD必須**：現在の予測位置 (制約ソルバで修正される)
	Vec2F previousPosition; // **PBD必須**：1つ前のタイムステップでの位置 (速度計算の基)
	Vec2F velocity;         // **推奨**：初期速度設定や外部力適用に使用
	float mass;             // 質点の質量
	float invMass;          // **推奨**：質量の逆数 (1/mass)。PBDの計算効率向上に必須。

	// デフォルトコンストラクタ
	Node() : position{ 0, 0 }, previousPosition{ 0, 0 }, velocity{ 0, 0 }, mass(1.0f), invMass(1.0f) {}

	// 質量の設定と逆数の自動計算
	void setMass(float m) {
		mass = m;
		invMass = (m > 0.0f) ? 1.0f / m : 0.0f; // 質量ゼロ（無限大）の場合は逆数をゼロにする
	}
};



class PopRingManager {
private:
	std::vector<Node> nodes; // リングを構成するノードの配列
	int numNodes;            // ノードの数
	float initialRadius;     // リングの初期半径
	Vec2F center;            // リングの初期中心座標

	// PBD パラメータ
	const float TIME_STEP = 1.0f / 60.0f; // 物理計算の固定時間刻み (Dt)
	const int PBD_ITERATIONS = 8;         // 制約を解く反復回数 (高いほど硬く正確になる)

	// 物理パラメータ
	const Vec2F GRAVITY = { 0.0f, 9.8f * 10.0f }; // 重力加速度 (Y軸方向)
	float floorY;            // 床のY座標
	float dampingFactor;     // 速度の減衰率 (空気抵抗など)

	// 制約パラメータ
	float distanceStiffness; // 距離制約の剛性 (弾力性)
	float initialSegmentLength; // 隣接ノード間の初期距離

public:
	/**
	 * @brief コンストラクタ
	 * @param num ノード数
	 * @param r 初期半径
	 * @param center_pos 初期中心座標
	 * @param floor_y 床のY座標
	 */
	PopRingManager(int num, float r, Vec2F center_pos, float floor_y) :
		numNodes(num), initialRadius(r), center(center_pos), floorY(floor_y),
		dampingFactor(0.995f), distanceStiffness(0.5f) // 剛性は0.0～1.0
	{
		// ノードの初期配置
		nodes.reserve(numNodes);
		for (int i = 0; i < numNodes; ++i) {
			float angle = 2.0f * DX_PI_F * (float)i / (float)numNodes;
			Vec2F pos = center + Vec2F{ initialRadius * std::cos(angle), initialRadius * std::sin(angle) };

			Node node;
			node.setMass(1.0f); // 質量を1.0に設定 (invMassが1.0になる)
			node.position = pos;
			node.previousPosition = pos;
			node.velocity = { 0.0f, 0.0f };
			nodes.push_back(node);
		}
		// 隣接ノード間の初期距離を計算
		initialSegmentLength = (nodes[0].position - nodes[1].position).length();
	}

	/**
	 * @brief シミュレーションの更新
	 * @param dt デルタタイム (未使用だが、標準的なアップデート関数として保持)
	 */
	void Update(float dt);

	/**
	 * @brief DXライブラリによる描画
	 */
	void Draw() const {
		// ノード間を線で結んでリングを描画
		for (int i = 0; i < numNodes; ++i) {
			const Node& current = nodes[i];
			const Node& next = nodes[(i + 1) % numNodes]; // 次のノード（リングなので一周する）

			DrawLine(
				(int)current.position.x, (int)current.position.y,
				(int)next.position.x, (int)next.position.y,
				GetColor(255, 255, 255) // 白
			);

			// ノード自体を描画 (デバッグ用)
			// DrawCircle((int)current.position.x, (int)current.position.y, 3, GetColor(0, 255, 0), TRUE);
		}

		// 床の描画
		DrawLine(0, (int)floorY, GameData::windowWidth, (int)floorY, GetColor(0, 0, 255)); // 青
	}

private:
	// ------------------------------------------------------------------------
	// PBD ステップ 1: 外部力の適用と予測位置の計算
	// ------------------------------------------------------------------------
	void ApplyExternalForces(float dt) {
		for (auto& node : nodes) {
			// 現在の位置を保存 (previousPosition)
			node.previousPosition = node.position;

			// 速度に重力を適用 (Euler積分)
			node.velocity += GRAVITY * dt;

			// 予測位置を計算 (速度で移動)
			node.position += node.velocity * dt;
		}
	}

	// ------------------------------------------------------------------------
	// PBD ステップ 2.1: 距離制約 (リングの形状と弾力性を維持)
	// ------------------------------------------------------------------------
	void SolveDistanceConstraints() {
		// 全ての隣接ノードペアに対して制約を適用
		for (int i = 0; i < numNodes; ++i) {
			Node& p1 = nodes[i];
			Node& p2 = nodes[(i + 1) % numNodes]; // リングなので次のノードは循環

			Vec2F delta = p1.position - p2.position;
			float currentDist = delta.length();
			float diff = currentDist - initialSegmentLength;

			// 制約勾配方向 (法線)
			Vec2F n = delta.normalize();

			// ラムダ (位置修正量のスカラー)
			// C = currentDist - initialSegmentLength
			// S = |n|^2 / (m1 + m2) の逆数
			float W = p1.invMass + p2.invMass; // 分母
			if (W == 0.0f) continue;
			float lambda = -diff / W;

			// 剛性 (Stiffness) の適用
			lambda *= distanceStiffness;

			// 位置修正ベクトル
			Vec2F dp = n * lambda;

			// 質量に応じて修正量を分配
			p1.position += (dp * p1.invMass);
			//p2.position -= dp * p2.invMass; // p2はp1と逆方向に修正
		}
	}

	// ------------------------------------------------------------------------
	// PBD ステップ 2.2: 面積制約 (リングが完全に潰れるのを防ぐ)
	// ------------------------------------------------------------------------
	// この制約は、中央のノード（仮想点）と2つのノードで構成される三角形の面積が、
	// 全体の面積を維持するように作用する、比較的簡単な方法を使用します。
	void SolveAreaConstraint() {
		// 簡単化のため、全ノードの重心を仮想的な中心点とします
		Vec2F virtualCenter = { 0.0f, 0.0f };
		for (const auto& node : nodes) {
			virtualCenter += node.position;
		}
		virtualCenter = virtualCenter / (float)numNodes;

		// 全ノードの位置を、仮想中心点から外側へ押し出すように修正します
		// これは完全な面積制約ではありませんが、「潰れにくい」弾力性を表現できます
		float currentRadiusSqSum = 0.0f;
		for (const auto& node : nodes) {
			currentRadiusSqSum += (node.position - virtualCenter).lengthSq();
		}
		float currentRadius = std::sqrt(currentRadiusSqSum / (float)numNodes);

		float targetRadius = initialRadius; // 目標半径

		if (currentRadius < targetRadius) {
			float correctionFactor = 1.0f - (currentRadius / targetRadius);

			// 修正量をスカラーで調整
			float push_strength = 0.05f; // 面積維持の剛性

			for (auto& node : nodes) {
				Vec2F dir = node.position - virtualCenter;
				// 中心から離れる方向に修正
				node.position += dir.normalize() * correctionFactor * push_strength;
			}
		}
	}

	// ------------------------------------------------------------------------
	// PBD ステップ 2.3: 衝突制約 (床との衝突)
	// ------------------------------------------------------------------------
	void ApplyCollisionConstraint() {
		for (auto& node : nodes) {
			// 床の高さ (floorY) より下にめり込んだ場合
			if (node.position.y > floorY) {
				// 1. 位置の修正 (めり込み解除)
				node.position.y = floorY;
			}
		}
	}

	// ------------------------------------------------------------------------
	// PBD ステップ 3: 最終位置の更新と速度の再計算
	// ------------------------------------------------------------------------
	void UpdatePositions(float dt) {
		float inv_dt = 1.0f / dt;
		for (auto& node : nodes) {
			// 新しい速度を計算 (修正後の位置と前の位置の差分)
			// v = (p_new - p_old) / dt
			node.velocity = (node.position - node.previousPosition) * inv_dt;

			// 減衰を適用
			node.velocity.x *= dampingFactor;
			node.velocity.y *= dampingFactor;
		}
	}
};