#include "Bullet.h"
#include "GraphicsUtils.h"

#include <Windows.h>

Bullet::Bullet(int row, double x, double y, IMAGE* image)
	: row(row), x(x), y(y), image(image),
	lastUpdate(GetTickCount64()) {}

void Bullet::update() {
	ULONGLONG now = GetTickCount64();
	double seconds = static_cast<double>(now - lastUpdate) / 1000.0;
	lastUpdate = now;
	x += SPEED * seconds; // 向右移动
}

void Bullet::draw() {
	drawPng(static_cast<int>(x) - 14, static_cast<int>(y) - 14, 28, 28, *image);
}

bool Bullet::isOffscreen() const {
	// 左边缘越过窗口右侧，整颗子弹已经离开画面
	return x - 14 >= 900;
}