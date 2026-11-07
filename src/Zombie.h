#pragma once

#include <Windows.h>

class IMAGE;
class Game;

class Zombie {
public:
	static constexpr int WALK_FRAME_COUNT = 47;
	static constexpr int DIE_FRAME_COUNT = 39;
	static constexpr int EAT_FRAME_COUNT = 40;

	Zombie(int row, IMAGE* walkFrames, IMAGE* dieFrames, IMAGE* eatFrames);

	void update(Game& game);
	void draw();

	void takeDamage(int damage);
	bool isDead() const { return health <= 0; }
	bool canRemove() const { return deathFinished; }

	int getRow() const { return row; }
	double getX() const { return x; }

private:
	static constexpr double WALK_SPEED = 14.0;

	int health = 100;
	int row;
	double x = 850.0;
	bool deathFinished = false;

	IMAGE* walkFrames;
	IMAGE* dieFrames;
	IMAGE* eatFrames;
	int frame = 0; // 当前帧
	ULONGLONG animationStart;
	ULONGLONG lastUpdate;
	ULONGLONG flashEnd = 0;

	bool eating = false;
	ULONGLONG lastBite = 0;
};