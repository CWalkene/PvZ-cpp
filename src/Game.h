#pragma once

#include "Plant.h"
#include "Sun.h"
#include "Zombie.h"
#include "Bullet.h"

#include <easyx.h>
#include <memory>
#include <vector>
#include <Windows.h>
#include <random>

class Game {
public:
	void run();
	void spawnSunFromPlant(int row, int col);
	void spawnBulletFromPlant(int row, int col);
	bool hasZombieAhead(int row, int col) const;
	Plant* findPlantToEat(int row, double zombieX);

private:
	enum class GameState { PLAYING, WON, LOST };
	GameState state = GameState::PLAYING;

	void loadAssets();
	void playbgm();
	void loadSoundEffects();
	void processInput();
	void update();
	void draw();
	void drawResult() const ;
	void drawGrid();
	void drawPlants();
	void drawHover();
	void drawUi();
	void drawSelected();
	void spawnSun(int x, int y);
	void spawnSkySun();
	void spawnZombie(int row);
	bool isOnSeed(int x, int y) const;
	bool isOnLawn(int x, int y) const;
	void endGame(GameState result);

	// 随机数引擎初始化
	std::mt19937 randomEngine{ std::random_device{}() };

	ULONGLONG lastSkySunTime = 0;
	static constexpr ULONGLONG SKY_SUN_INTERVAL = 8000;

	static constexpr int rowCount = 5;
	static constexpr int colCount = 9;

	// 植物放置网格参数
	static constexpr int startX = 75;
	static constexpr int startY = 75;
	static constexpr int cellWidth = 80;
	static constexpr int cellHeight = 100;

	// 帧数
	static constexpr int peaFrameCount = 25;
	static constexpr int sunflowerFrameCount = 25;
	static constexpr int wallnutFrameCount = 17;

	// 僵尸生成规则
	static constexpr int TOTAL_ZOMBIES = 20;
	static constexpr ULONGLONG FIRST_ZOMBIE_DELAY = 15000;
	static constexpr ULONGLONG ZOMBIE_SPAWN_INTERVAL = 5000;
	// 累计生成数量，僵尸死亡后不减少
	int spawnedZombieCount = 0;
	ULONGLONG lastZombieSpawnTime = 0;

	IMAGE background;

	// 种子栏
	IMAGE seedBank;
	IMAGE peaCard;
	IMAGE sunflowerCard;
	IMAGE wallnutCard;
	IMAGE shovelBank;
	IMAGE shovel;

	bool shovelSelected = false;

	// 动画帧数组
	IMAGE peashooter[peaFrameCount];
	IMAGE sunflower[sunflowerFrameCount];
	IMAGE wallnut[wallnutFrameCount];
	IMAGE sunFrames[Sun::FRAME_COUNT];
	IMAGE zombieWalkFrames[Zombie::WALK_FRAME_COUNT];
	IMAGE zombieDieFrames[Zombie::DIE_FRAME_COUNT];
	IMAGE zombieEatFrames[Zombie::EAT_FRAME_COUNT];

	// 场上的对象
	std::vector<Sun> suns;
	std::vector<Zombie> zombies;

	// 豌豆图片与场上的子弹
	IMAGE peaImage;
	std::vector<Bullet> bullets;

	// 格子状态
	std::unique_ptr<Plant> plants[rowCount][colCount];

	// 鼠标选择着的植物
	std::unique_ptr<Plant> selectedPlant;

	int mouseX = -1;
	int mouseY = -1;
	bool running = false; // 游戏状态

	int sunCount = 200;
};
