#include "Sun.h"
#include "GraphicsUtils.h"

#include <windows.h>
#include <easyx.h>

Sun::Sun(int x, int y, IMAGE* frames) 
	: x(x), y(y), frames(frames), 
	animationStart(GetTickCount64()) {}

void Sun::update() {
	// 每秒播放 16 帧
	ULONGLONG elapsed = GetTickCount64() - animationStart;
	frame = static_cast<int>((elapsed * 16 / 1000) % FRAME_COUNT);

	// 收集动画
	if (collecting) {
		ULONGLONG elapsed = GetTickCount64() - collectStart;
		double t = elapsed < COLLECT_DURATION
			? static_cast<double>(elapsed) / COLLECT_DURATION : 1.0;
		// 缓出动画
		double remaining = 1.0 - t;
		double eased = 1.0 - remaining * remaining * remaining;

		x = fromX + static_cast<int>((targetX - fromX) * eased);
		y = fromY + static_cast<int>((targetY - fromY) * eased);


	}

	// 向日葵喷出动画
	if (spawning) { // 边移动边长大，沿弧线落到地面
		ULONGLONG elapsed = GetTickCount64() - spawnStart;
		double t = elapsed < SPAWN_DURATION
			? static_cast<double>(elapsed) / SPAWN_DURATION : 1.0;

		x = spawnX + static_cast<int>((groundX - spawnX) * t);
		y = static_cast<int>(spawnY + (groundY - spawnY) * t
			- 4.0 * 55.0 * t * (1.0 - t));

		drawSize = 24 + static_cast<int>(72 * t);
		if (t >= 1.0) {
			spawning = false;
			expireTime = GetTickCount64() + GROUND_LIFETIME;
		}
	}
		
	// 天空阳光沿竖直方向缓慢落下
	if (falling) {
		ULONGLONG elapsed = GetTickCount64() - fallStart;
		double t = elapsed < FALL_DURATION
			? static_cast<double>(elapsed) / FALL_DURATION : 1.0;

		y = static_cast<int>(
			fallStartY + (fallGroundY - fallStartY) * t);

		if (t >= 1.0) {
			falling = false;
			expireTime = GetTickCount64() + GROUND_LIFETIME;
		}
	}
}

void Sun::draw() {
	// 图片中心对齐阳光中心
	drawPng(x - drawSize / 2, y - drawSize / 2, drawSize, drawSize, frames[frame]);
}

bool Sun::contains(int mouseX, int mouseY) const {
	if (collecting) return false;
	// 用半径 30 像素的圆作为点击区域
	int dx = mouseX - x;
	int dy = mouseY - y;
	return dx * dx + dy * dy <= 30 * 30;
}

void Sun::beginCollect(int targetX, int targetY) {
	if (collecting) return;
	spawning = false;
	falling = false;
	collecting = true;
	fromX = x;
	fromY = y;
	this->targetX = targetX;
	this->targetY = targetY;
	collectStart = GetTickCount64();
}

bool Sun::isCollected() const {
	return collecting &&
		GetTickCount64() - collectStart >= COLLECT_DURATION;
}

bool Sun::isExpired() const {
	return !collecting && expireTime != 0 && GetTickCount64() >= expireTime;
}

void Sun::beginSpawn(int groundX, int groundY) {
	expireTime = 0;
	falling = false;
	spawning = true;
	spawnX = x;
	spawnY = y;
	this->groundX = groundX;
	this->groundY = groundY;
	drawSize = 24;
	spawnStart = GetTickCount64();
}

void Sun::beginFall(int groundY) {
	expireTime = 0;
	spawning = false;
	falling = true;
	fallStartY = y;
	fallGroundY = groundY;
	drawSize = 96;
	fallStart = GetTickCount64();
}