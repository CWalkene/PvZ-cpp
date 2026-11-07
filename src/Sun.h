#pragma once

#include <Windows.h>

class IMAGE;

class Sun {
public:
	static constexpr int FRAME_COUNT = 13;
	static constexpr int VALUE = 25; // 阳光点数

	Sun(int x, int y, IMAGE* frames);

	void update();
	void draw();
	void beginCollect(int targetX, int targetY);
	void beginSpawn(int groundX, int groundY);
	void beginFall(int groundY);
	bool isCollected() const;
	bool isExpired() const;
	bool contains(int mouseX, int mouseY) const;

private:
	int x;
	int y;
	IMAGE* frames;
	int frame = 0;
	ULONGLONG animationStart;
	ULONGLONG expireTime = 0;
	static constexpr ULONGLONG GROUND_LIFETIME = 30000; // 过期时间（毫秒）

	// 收集动画
	bool collecting = false;
	int fromX = 0, fromY = 0;
	int targetX = 0, targetY = 0;
	ULONGLONG collectStart = 0;
	static constexpr ULONGLONG COLLECT_DURATION = 1000; // 收集动画速度（毫秒数）

	// 喷出动画
	bool spawning = false;
	int spawnX = 0, spawnY = 0;
	int groundX = 0, groundY = 0;
	int drawSize = 96;
	ULONGLONG spawnStart = 0;
	static constexpr ULONGLONG SPAWN_DURATION = 800;

	// 天空掉落动画
	bool falling = false;
	int fallStartY = 0;
	int fallGroundY = 0;
	ULONGLONG fallStart = 0;
	static constexpr ULONGLONG FALL_DURATION = 4000;
};