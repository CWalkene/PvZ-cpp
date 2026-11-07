#include "Sunflower.h"
#include "Plant.h"
#include "Game.h"

#include <Windows.h>

void Sunflower::update(Game& game) {
	Plant::update(game);
	// 生产阳光
	ULONGLONG now = GetTickCount64();

	if (!productionStarted) {
		productionStarted = true;
		lastProductionTime = now;
	}
	if (now - lastProductionTime >= PRODUCTION_INTERVAL) {
		game.spawnSunFromPlant(getRow(), getCol());
		lastProductionTime = now;
	}
}