#include "PBDRingSim.h"

PBDRingSim::PBDRingSim()
	:nodeCount(10), pointRadius(50), circleRadius(5)
	, startX(0), startY(0), moveSpeed(5), segmentLength(0)
{
	startX = GameData::windowWidth / 2;
	startY = GameData::windowHeight / 2;
	nodeColor = GetColor(255, 255, 255);

	//ノードを円周上に配置する
	float oneStep = (2 * DX_PI_F) / nodeCount;
	//中心ノード作成
	middleNode = { startX,startY };
	for (int i = 0; i < nodeCount; i++) {
		Vec2f node = { startX + cos(oneStep * i - DX_PI_F / 2) * pointRadius,
						  startY + sin(oneStep * i - DX_PI_F / 2) * pointRadius };
		nodes.push_back(node);
	}
	/*for (int i = 0; i < nodeCount; i++) {
		nodes.push_back({ startX,startY + 10 * i });
	}*/
	Vec2f diff = nodes[0] - nodes[1];
	segmentLength = diff.length();
}

void PBDRingSim::Update(const InputState* input) {
	Vec2f& topNode = middleNode;
	float moveValX = 0;
	float moveValY = 0;
	if (input->IsKeyStay(KEY_INPUT_A)) {
		topNode.x -= moveSpeed;
		moveValX -= moveSpeed;
	}
	if (input->IsKeyStay(KEY_INPUT_D)) {
		topNode.x += moveSpeed;
		moveValX += moveSpeed;
	}
	if (input->IsKeyStay(KEY_INPUT_W)) {
		topNode.y -= moveSpeed;
		moveValY -= moveSpeed;
	}
	if (input->IsKeyStay(KEY_INPUT_S)) {
		topNode.y += moveSpeed;
		moveValY += moveSpeed;
	}

	//各移動を各ノードの理想位置に伝える

	for (int i = 0; i < testCount; i++) {
		UpdateNodesWithRing();
		UpdateNodesAround();
		//UpdateNodesLine();
		BarrierHit();
	}
}

void PBDRingSim::UpdateNodesWithRing() {
	for (int i = 0; i < nodes.size(); i++) {
		Vec2f& p1 = nodes[i];
		Vec2f& p2 = middleNode;

		//node0→node1のベクトルとその長さを取得
		Vec2f diff = p1 - p2;
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

		float correctFactor = (currentDist - pointRadius) / currentDist;
		Vec2f correctVector = diff * correctFactor;

		//p2(node0)からp1(node1)へのベクトルがdiffなので、p1からp2は逆ベクトルになる
		//なので、p1に対してはマイナスになる
		if (!changeVersion) {
			p1 = p1 - correctVector * 0.5f;
			/*if (i == 1) {
				p1 = p1 - correctVector;
			}
			else {
				p1 = p1 - correctVector * 0.5f;
				p2 = p2 + correctVector * 0.5f;
			}*/
		}
		else {
			p1 = p1 - correctVector;
		}

	}
}

void PBDRingSim::UpdateNodesAround() {
	for (int i = 1; i <= nodes.size(); i++) {
		Vec2f& p1 = nodes[i % nodes.size()];
		Vec2f& p2 = nodes[i - 1];

		//node0→node1のベクトルとその長さを取得
		Vec2f diff = p1 - p2;
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
		Vec2f correctVector = diff * correctFactor;

		//p2(node0)からp1(node1)へのベクトルがdiffなので、p1からp2は逆ベクトルになる
		//なので、p1に対してはマイナスになる
		if (!changeVersion) {
			p1 = p1 - correctVector * 0.5f;
			p2 = p2 + correctVector * 0.5f;
			/*if (i == 1) {
				p1 = p1 - correctVector;
			}
			else {
				p1 = p1 - correctVector * 0.5f;
				p2 = p2 + correctVector * 0.5f;
			}*/
		}
		else {
			p1 = p1 - correctVector;
		}

	}
}

void PBDRingSim::UpdateNodeIdealPos() {
	for (int i = 0; i < nodes.size(); i++) {

	}
}

void PBDRingSim::UpdateNodesLine() {
	for (int i = 1; i < nodes.size(); i++) {
		Vec2f& p1 = nodes[i % nodes.size()];
		Vec2f& p2 = nodes[i];

		//node0→node1のベクトルとその長さを取得
		Vec2f diff = p1 - p2;
		float currentDist = diff.length();

		float correctFactor = (currentDist - segmentLength) / currentDist;
		Vec2f correctVector = diff * correctFactor;

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

void PBDRingSim::BarrierHit() {
	if (middleNode.y > 700)middleNode.y = 700;
	for (int i = 0; i < nodes.size(); i++) {
		if (nodes[i].y > 700) {
			nodes[i].y = 700;
		}
	}
}

void PBDRingSim::Draw() {
	int color = GetColor(255, 255, 255);
	if (changeVersion)color = GetColor(0, 255, 0);
	//DrawCircle(startX, startY, 10, GetColor(0, 0, 0), TRUE);

	//中心ノードを描画
	DrawCircle(middleNode.x, middleNode.y, circleRadius, nodeColor, TRUE);

	//各ノードを描画
	for (int i = 0; i < nodes.size(); i++) {
		DrawCircle(nodes[i].x, nodes[i].y, circleRadius, nodeColor, TRUE);
	}
	//各ノードの理想位置を描画
	/*for (int i = 0; i < nodes.size(); i++) {
		DrawCircle(nodes[i].ideal_x, nodes[i].ideal_y, circleRadius, GetColor(0, 255, 0), TRUE);
	}*/
	//ノードをつなぐ線を描画
	for (int i = 0; i < nodes.size(); i++) {
		int next = (i + 1) % nodeCount;
		DrawLine(nodes[i].x, nodes[i].y, nodes[next].x, nodes[next].y,
			GetColor(255, 255, 255), 5);
	}
	/*for (int i = 0; i < nodes.size() - 1; i++) {
		int next = (i + 1) % nodeCount;
		DrawLine(nodes[i].x, nodes[i].y, nodes[next].x, nodes[next].y,
			GetColor(255, 255, 255), 5);
	}*/
}

