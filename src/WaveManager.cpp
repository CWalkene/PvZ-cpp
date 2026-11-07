#include "WaveManager.h"

// 波次数量配置
void WaveManager::reset(ULONGLONG now) {
	for (int i = 0; i < WAVE_COUNT; ++i) {
		bool isBigWave = (i + 1) % 10 == 0;
		int count = 2 + i / 4;

		// 旧固定间隔说明保留，当前使用下方波次配置。
		// 第一只等 10 秒，后面的每只间隔 5 秒
		waves[i] = WaveConfig{
			isBigWave ? count * 2 : count,
			isBigWave ? 400ULL : 800ULL,
			isBigWave ? 25000ULL : 18000ULL,
			isBigWave ? 40000ULL : 30000ULL
		};
	}

	currentWave = -1;
	spawnedInWave = 0;
	preparationStartTime = now;
	waveStartTime = 0;
	lastSpawnTime = 0;
}

void WaveManager::startNextWave(ULONGLONG now) {
	// 最后一波之后不再推进，避免配置数组越界
	if (currentWave >= WAVE_COUNT - 1) return;

	++currentWave;
	spawnedInWave = 0;
	waveStartTime = now;
	lastSpawnTime = now;
}

bool WaveManager::update(ULONGLONG now, int aliveCount) {
	// 开局准备
	if (currentWave < 0) {
		if (now - preparationStartTime >= PREPARATION_TIME) {
			startNextWave(now);
		}
		return false;
	}

	// 取出当前波次的配置
	const WaveConfig& config = waves[currentWave];

	if (spawnedInWave < config.zombieCount) {
		if (now - lastSpawnTime >= config.spawnInterval) {
			// 累计数量增加，并重新记录生成时间
			++spawnedInWave;
			lastSpawnTime = now;
			return true;
		}
		return false;
	}

	// 本波次全部生成完毕
	// 判断何时进入下一波
	ULONGLONG elapsed = now - waveStartTime;
	// 达到最小时长 与 存活僵尸不超过一只 -> 可以提前下一波
	bool clearedEnough = elapsed >= config.minDuration && aliveCount <= 1;
	// 或者超过了最大时长 -> 进入下一波
	if (clearedEnough || elapsed >= config.maxDuration) {
		startNextWave(now);
	}
	return false;
}

bool WaveManager::allSpawned() const {
	return currentWave == WAVE_COUNT - 1 &&
		spawnedInWave >= waves.back().zombieCount;
}
