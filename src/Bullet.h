#pragma once

#include <Windows.h>

class IMAGE;

class Bullet {
public:
	static constexpr int DAMAGE = 20;

	// x、y 是子弹中心，row 用于之后判断同一行碰撞
	Bullet(int row, double x, double y, IMAGE* image);

	int getRow() const { return row; }
	double getX() const { return x; }

	void update();
	void draw();
	bool isOffscreen() const;
private:
	int row;
	double x;
	double y;
	IMAGE* image;

	static constexpr double SPEED = 250.0; // 每秒移动像素
	ULONGLONG lastUpdate;
};