#include "Zombie.h"
#include "Game.h"
#include "Plant.h"
#include "GraphicsUtils.h"
#include "Audio.h"

#include <Windows.h>
#include <easyx.h>

Zombie::Zombie(int row, IMAGE* walkFrames,
	IMAGE* dieFrames, IMAGE* eatFrames)
	: row(row), walkFrames(walkFrames),
	dieFrames(dieFrames), eatFrames(eatFrames),
	animationStart(GetTickCount64()), lastUpdate(animationStart) {}

void Zombie::update(Game& game) {
	ULONGLONG now = GetTickCount64();

	if (isDead()) {
		ULONGLONG deathFrame = (now - animationStart) * 24 / 1000;
		if (deathFrame >= DIE_FRAME_COUNT) {
			frame = DIE_FRAME_COUNT - 1;
			deathFinished = true;
		}
		else {
			frame = static_cast<int>(deathFrame);
		}
		return;
	}

	double seconds = static_cast<double>(now - lastUpdate) / 1000;
	lastUpdate = now;

	// 吃植物
	Plant* target = game.findPlantToEat(row, x);
	bool shouldEat = target != nullptr;

	// 行走和啃食切换时，动画从头开始
	if (shouldEat != eating) {
		eating = shouldEat;
		animationStart = now;
		lastBite = now;
	}

	if (eating) {
		// 每半秒造成 10 点伤害，并播放一次啃食音效
		if (now - lastBite >= 1000) {
			target->takeDamage(20);
			Audio::playSound(L"pvz_eat");
			lastBite = now;
		}
		frame = static_cast<int>(((now - animationStart) * 24 / 1000) % EAT_FRAME_COUNT);
	}
	else {
		x -= WALK_SPEED * seconds; // 向左移动
		frame = static_cast<int>(
			((now - animationStart) * 16 / 1000) % WALK_FRAME_COUNT);
	}
}

void Zombie::draw() {
	int footY = 75 + row * 100 + 100 - 10;
	IMAGE* frames = isDead() ? dieFrames : (eating ? eatFrames : walkFrames);
	if (GetTickCount64() < flashEnd) {
		drawPngWhite(static_cast<int>(x) - 96, footY - 140, 192, 168, frames[frame]);
	}
	else {
		drawPng(static_cast<int>(x) - 96, footY - 140, 192, 168, frames[frame]);
	}
}


void Zombie::takeDamage(int damage) {
	if (damage <= 0 || isDead()) return;
	flashEnd = GetTickCount64() + 150;
	health -= damage;

	// 血量归零时，重置倒地动画
	if (isDead()) {
		animationStart = GetTickCount64();
		frame = 0;
	}
}
