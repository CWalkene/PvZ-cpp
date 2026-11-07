#pragma once

#include <Windows.h>
#include <array>

class WaveManager {
public:
	void reset(ULONGLONG now);

	// 返回 true 表示需要生成一只僵尸。
	bool update(ULONGLONG now, int aliveCount);
	bool allSpawned() const;
	int getCurrentWave() const { return currentWave + 1; }

private:
	// 每波的数量、波内间隔，以及推进下一波的等待时间
	// { 4, 800, 18000, 30000 } : 
	// 这一波生成 4 只僵尸，每只间隔 800 毫秒。
	// 从本波开始满 18 秒后，若全场最多剩一只就推进；否则等到 30 秒推进。
	struct WaveConfig {
		int zombieCount;
		ULONGLONG spawnInterval;
		ULONGLONG minDuration;
		ULONGLONG maxDuration;
	};

	// 僵尸生成规则
	static constexpr int WAVE_COUNT = 20;
	static constexpr ULONGLONG PREPARATION_TIME = 20000;

	std::array<WaveConfig, WAVE_COUNT> waves{};

	// -1 表示开局准备阶段；0～19 对应第 1～20 波
	int currentWave = -1;
	// 累计生成数量，僵尸死亡后不减少
	int spawnedInWave = 0;

	ULONGLONG preparationStartTime = 0;
	ULONGLONG waveStartTime = 0;
	ULONGLONG lastSpawnTime = 0;

	void startNextWave(ULONGLONG now);
};
