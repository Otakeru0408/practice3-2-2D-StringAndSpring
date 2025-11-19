#pragma once
#include "DxLib.h"
#include "StringPointManager.h"
#include "GameData.h"
#include "InputState.h"
#include <cmath>
#include <vector>

//struct Vec2F {
//	float x;
//	float y;
//
//	Vec2F operator+(const Vec2F& other)const { return { x + other.x,y + other.y }; }
//	Vec2F operator-(const Vec2F& other)const { return { x - other.x,y - other.y }; }
//	Vec2F operator*(const float v)const { return { x * v,y * v }; }
//
//	float lengthSq()const { return x * x + y * y; }
//	float length()const { return std::sqrt(lengthSq()); }
//};

class PBDRingSim {
public:
	PBDRingSim();
	void Update(const InputState* input);
	void Draw();
	bool changeVersion = false;
	int testCount = 3;
	int nodeColor = 0;
private:
	std::vector<Vec2f> nodes;
	Vec2f middleNode;
	int nodeCount;
	float pointRadius;			//中心からの半径
	float circleRadius;			//ノード描画時のcircle半径
	float startX, startY;
	float moveSpeed;
	float segmentLength;		//各ノード間の理想的な距離

	//中心ノードと各ノードが距離を調整する処理
	void UpdateNodesWithRing();
	//全てのノードが線のようにつながっていく処理
	void UpdateNodesLine();
	//中心ノード以外のノードが円状になるように距離を調整する処理
	void UpdateNodesAround();
	//指定した位置より先に進まないようにする処理
	void BarrierHit();
	//各ノードの調整を、各ノードの理想位置を用いた処理
	void UpdateNodeIdealPos();
};