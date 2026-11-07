#include "Plant.h"
#include "GraphicsUtils.h"

#include <Windows.h>
#include <easyx.h>

// 保存共享动画数组，并记录动画开始时间
Plant::Plant(IMAGE* frames, int frameCount, int health)
	: health(health),
	frames(frames),
	frameCount(frameCount),
	animationStart(GetTickCount64()) {}

// 成功种植时设置行列，并从第一帧开始播放
void Plant::setCell(int row, int col) {
	this->row = row;
	this->col = col;
	animationStart = GetTickCount64();
	frame = 0;
}

int Plant::getRow() const {
	return row;
}

int Plant::getCol() const {
	return col;
}

void Plant::drawFrame(IMAGE& image) const {
	const int startX = 75;
	const int startY = 75;
	const int cellWidth = 80;
	const int cellHeight = 100;

	// 植物脚底放在格子的水平中心、距离底部 10 像素处
	int footX = startX + col * cellWidth + cellWidth / 2;
	int footY = startY + row * cellHeight + cellHeight - 10;

	// 原图为 192×192，按原图约 0.6 倍显示
	drawPng(footX + 5 - 58, footY - 106, 115, 115, image);
}

void Plant::draw() {
	drawFrame(frames[frame]);
}

void Plant::update(Game&) {
	// 每株植物独立计算帧号，每秒播放 20 帧
	ULONGLONG elapsed = GetTickCount64() - animationStart;
	frame = static_cast<int>((elapsed * 20 / 1000) % frameCount);
}

void Plant::drawPreview(int mouseX, int mouseY) {
	drawPng(mouseX - 58, mouseY - 58, 115, 115, frames[0]);
}

void Plant::drawPlacementPreview(int footX, int footY) const {
	// 与种下后的植物相符
	drawPng(footX + 5 - 58, footY - 106, 115, 115, frames[0], 77);
}

void Plant::takeDamage(int damage) {
	if (damage <= 0 || isDead()) return;

	health -= damage;
	if (health < 0) health = 0;
}
