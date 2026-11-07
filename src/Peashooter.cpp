#include "Peashooter.h"
#include "Plant.h"
#include "Game.h"

#include <Windows.h>

void Peashooter::update(Game& game) {
	Plant::update(game);
	ULONGLONG now = GetTickCount64();

	if (!game.hasZombieAhead(getRow(), getCol())) {
		attackStarted = false;
		return;
	}
	if (!attackStarted) {
		attackStarted = true;
		lastShotTime = now;
	}
	if (now - lastShotTime >= ATTACK_INTERVAL) {
		game.spawnBulletFromPlant(getRow(), getCol());
		lastShotTime = now;
	}
}

