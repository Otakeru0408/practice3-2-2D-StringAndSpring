#include "PBDSimulation.h"

PBDSimulation::PBDSimulation()
	:nodeCount(10), pointRadius(100), circleRadius(5)
	, startX(0), startY(0), moveSpeed(10), segmentLength(30)
{
	startX = GameData::windowWidth / 2;
	startY = GameData::windowHeight / 2;
	nodeColor = GetColor(255, 255, 255);

	//ノードを円周上に配置する
	float oneStep = (2 * DX_PI_F) / nodeCount;
	for (int i = 0; i < nodeCount; i++) {
		nodes.push_back({ startX + cos(oneStep * i - DX_PI_F / 2) * pointRadius,
						  startY + sin(oneStep * i - DX_PI_F / 2) * pointRadius });
	}
	/*for (int i = 0; i < nodeCount; i++) {
		nodes.push_back({ startX,startY + 10 * i });
	}*/
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

	for (int i = 0; i < testCount; i++) {
		//UpdateNodesWithRing();
		UpdateNodesLine();
	}
}

void PBDSimulation::UpdateNodesLine() {
	for (int i = 1; i < nodes.size(); i++) {
		Vec2F& p1 = nodes[i % nodes.size()];
		Vec2F& p2 = nodes[i - 1];

		//node0→node1のベクトルとその長さを取得
		Vec2F diff = p1 - p2;
		float currentDist = diff.length();

		float correctFactor = (currentDist - segmentLength) / currentDist;
		Vec2F correctVector = diff * correctFactor;

		if (!changeVersion) {
			if (i == 1) {
				p1 = p1 - correctVector;
			}
			else {
				p1 = p1 - correctVector * 0.5f;
				p2 = p2 + correctVector * 0.5f;
			}
		}
		else {
			p1 = p1 - correctVector;
		}

	}
}

void PBDSimulation::UpdateNodesWithRing() {
	for (int i = 1; i <= nodes.size(); i++) {
		Vec2F& p1 = nodes[i % nodes.size()];
		Vec2F& p2 = nodes[i - 1];

		//node0→node1のベクトルとその長さを取得
		Vec2F diff = p1 - p2;
		float currentDist = diff.length();

		//修正量：node0がうごいたらnode1との距離は理想の距離ではなくなるので修正
		/*
		Dを２つのポイントの距離だとする。
		diffは２つのポイント間のベクトル→つまり向き
		diff/Dはベクトルをその大きさで割っているので単位ベクトルになる
		その単位ベクトルdiff/Dに実際に移動させたい距離(D-L)をかける
		Lは２つのポイントの理想的な距離
		diff/D * (D-L)=diff * (D-L)/Dと書けるので、(D-L)/Dの式が必要なのだ
		*/

		float correctFactor = (currentDist - segmentLength) / currentDist;
		Vec2F correctVector = diff * correctFactor;

		//p2(node0)からp1(node1)へのベクトルがdiffなので、p1からp2は逆ベクトルになる
		//なので、p1に対してはマイナスになる
		if (!changeVersion) {
			if (i == 1) {
				p1 = p1 - correctVector;
			}
			else {
				p1 = p1 - correctVector * 0.5f;
				p2 = p2 + correctVector * 0.5f;
			}
		}
		else {
			p1 = p1 - correctVector;
		}

	}
}

void PBDSimulation::Draw() {
	int color = GetColor(255, 255, 255);
	if (changeVersion)color = GetColor(0, 255, 0);
	//DrawCircle(startX, startY, 10, GetColor(0, 0, 0), TRUE);
	for (int i = 0; i < nodes.size(); i++) {
		DrawCircle(nodes[i].x, nodes[i].y, circleRadius, nodeColor, TRUE);
	}
	/*for (int i = 0; i < nodes.size(); i++) {
		int next = (i + 1) % nodeCount;
		DrawLine(nodes[i].x, nodes[i].y, nodes[next].x, nodes[next].y,
			GetColor(255, 255, 255), 5);
	}*/
	for (int i = 0; i < nodes.size() - 1; i++) {
		int next = (i + 1) % nodeCount;
		DrawLine(nodes[i].x, nodes[i].y, nodes[next].x, nodes[next].y,
			GetColor(255, 255, 255), 5);
	}
}

