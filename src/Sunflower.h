#pragma once

#include "Plant.h"

#include <Windows.h>

class Game;
class IMAGE;

class Sunflower : public Plant {
public:
	static constexpr int COST = 50;

	explicit Sunflower(IMAGE* frames) : Plant(frames, 25) {}

	void update(Game& game) override;
	int getCost() const override { return COST; }

private:
	bool productionStarted = false;
	ULONGLONG lastProductionTime = 0;
	static constexpr ULONGLONG PRODUCTION_INTERVAL = 10000; // 生产冷却毫秒
};
