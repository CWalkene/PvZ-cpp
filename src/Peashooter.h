#pragma once

#include "Plant.h"

#include <Windows.h>

class IMAGE;
class Game;

class Peashooter : public Plant {
public:
	static constexpr int COST = 100;
	explicit Peashooter(IMAGE* frames) : Plant(frames, 25) {}

	void update(Game& game) override;

	int getCost() const override { return COST; }

private:
	bool attackStarted = false;
	ULONGLONG lastShotTime = 0;
	static constexpr ULONGLONG ATTACK_INTERVAL = 2000; // 攻击间隔

};
