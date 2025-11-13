#include "PBDSimulation.h"

PBDSimulation::PBDSimulation()
	:nodeCount(5), pointRadius(100), circleRadius(5)
	, startX(0), startY(0), moveSpeed(10)
{
	startX = GameData::windowWidth / 2;
	startY = GameData::windowHeight / 2;

	//ノードを円周上に配置する
	//360°=2pi (2*DX_PI_F)/nodecount;
	float oneStep = (2 * DX_PI_F) / nodeCount;
	for (int i = 0; i < nodeCount; i++) {
		nodes.push_back({ startX + cos(oneStep * i - DX_PI_F / 2) * pointRadius,
						  startY + sin(oneStep * i - DX_PI_F / 2) * pointRadius });
	}
}

void PBDSimulation::Update(const InputState* input) {
	Vec2F& topNode = nodes[0];
	if (input->IsKeyStay(KEY_INPUT_A)) {
		topNode.x -= moveSpeed;
	}
	if (input->IsKeyStay(KEY_INPUT_D)) {
		topNode.x += moveSpeed;
	}
	if (input->IsKeyStay(KEY_INPUT_W)) {
		topNode.y -= moveSpeed;
	}
	if (input->IsKeyStay(KEY_INPUT_S)) {
		topNode.y += moveSpeed;
	}
	//// Node 0 はユーザーが操作するポイント
	//Vec2F& controllablePoint = ropeNodes[0];

	//// 水平方向の移動
	//if (CheckHitKey(KEY_INPUT_A)) {
	//	controllablePoint.x -= moveSpeed;
	//}
	//if (CheckHitKey(KEY_INPUT_D)) {
	//	controllablePoint.x += moveSpeed;
	//}

	//// 垂直方向の移動
	//if (CheckHitKey(KEY_INPUT_W)) {
	//	controllablePoint.y -= moveSpeed;
	//}
	//if (CheckHitKey(KEY_INPUT_S)) {
	//	controllablePoint.y += moveSpeed;
	//}
}

void PBDSimulation::Draw() {
	//DrawCircle(startX, startY, 10, GetColor(0, 0, 0), TRUE);
	for (int i = 0; i < nodes.size(); i++) {
		DrawCircle(nodes[i].x, nodes[i].y, circleRadius, GetColor(255, 255, 255), TRUE);
	}
	for (int i = 0; i < nodes.size(); i++) {
		int next = (i + 1) % nodeCount;
		DrawLine(nodes[i].x, nodes[i].y, nodes[next].x, nodes[next].y,
			GetColor(255, 255, 255), 5);
	}
}