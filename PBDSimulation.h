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

class PBDSimulation {
public:
	PBDSimulation();
	void Update(const InputState* input);
	void Draw();
	bool changeVersion = false;
private:
	std::vector<Vec2F> nodes;
	int nodeCount;
	float pointRadius;
	float circleRadius;
	float startX, startY;
	float moveSpeed;
	float segmentLength;

	void UpdateNodes();
};