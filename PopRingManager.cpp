#include "PopRingManager.h"

void PopRingManager::Update(float de) {
	// PBDは通常、固定タイムステップで実行
	// 1. 外部力の適用と予測位置の計算
	ApplyExternalForces(TIME_STEP);

	// 2. 制約の反復的な解決
	for (int i = 0; i < PBD_ITERATIONS; ++i) {
		SolveDistanceConstraints();
		SolveAreaConstraint();      // (面積維持)
		ApplyCollisionConstraint(); // (床との衝突)
	}

	// 3. 最終位置と速度の更新
	UpdatePositions(TIME_STEP);
}