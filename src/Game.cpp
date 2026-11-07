#include "Game.h"
#include "GraphicsUtils.h"
#include "Audio.h"
#include "Peashooter.h"
#include "Sunflower.h"
#include "Wallnut.h"

#include <Windows.h>
#include <easyx.h>
#include <memory>
#include <string>
#include <tchar.h>
#include <utility>
#include <random>

void Game::run() {
	initgraph(900, 600);
	SetWindowText(GetHWnd(), _T("Plants vs Zombies"));
	loadAssets();
	loadSoundEffects();
	playbgm();
	spawnSkySun();
	lastSkySunTime = GetTickCount64();
	waveManager.reset(GetTickCount64());
	previousSpawnRow = -1;
	running = true;

	BeginBatchDraw();
	while (running) {
		Audio::beginFrame();
		processInput();
		if (!running) {
			break;
		}
		// 结算发生的这一帧绘制结果，之后保留画面，不再更新或重绘。
		if (state == GameState::PLAYING) {
			update();
			draw();
			FlushBatchDraw();
		}
		Sleep(16);
	}
	EndBatchDraw();
	Audio::closeSounds();
	Audio::stopMusic();
	closegraph();
}

void Game::spawnSunFromPlant(int row, int col) {
	int footX = startX + col * cellWidth + cellWidth / 2 + 5;
	int footY = startY + row * cellHeight + cellHeight - 10;
	// 从花盘附近喷出，落点沿用现有喷出动画
	spawnSun(footX, footY - 65);
}

void Game::spawnBulletFromPlant(int row, int col) {
	int footX = startX + col * cellWidth + cellWidth / 2 + 5;
	int footY = startY + row * cellHeight + cellHeight - 10;

	bullets.emplace_back(row, footX + 35, footY - 65, &peaImage);
	Audio::playSound(L"pvz_shoot");
}


Plant* Game::findPlantToEat(int row, double zombieX) {
	if (row < 0 || row >= rowCount) return nullptr;
	// 从右向左检查
	for (int col = colCount - 1; col >= 0; --col) {
		// 格子无植物或植物已死亡则跳过
		if (!plants[row][col] || plants[row][col]->isDead()) continue;

		int plantX = startX + col * cellWidth + cellWidth / 2 + 5;
		double distance = zombieX - plantX;

		if (distance >= -15.0 && distance <= 55.0) {
			return plants[row][col].get();
		}
	}
	return nullptr;
}

void Game::loadAssets() {
	// 加载背景
	loadimage(&background, _T("assets/backgrounds/day_crop.png"), 900, 600);

	// 加载植物动画
	for (int i = 0; i < peaFrameCount; ++i) {
		std::wstring path = L"assets/plants/peashooter/idle/";
		path += (i < 10 ? L"0" : L"");
		path += std::to_wstring(i) + L".png";
		loadimage(&peashooter[i], path.c_str());
		resizePng(peashooter[i], 115, 115);
	}
	for (int i = 0; i < sunflowerFrameCount; ++i) {
		std::wstring path = L"assets/plants/sunflower/idle/";
		path += (i < 10 ? L"0" : L"");
		path += std::to_wstring(i) + L".png";
		loadimage(&sunflower[i], path.c_str());
		resizePng(sunflower[i], 115, 115);
	}
	for (int i = 0; i < wallnutFrameCount; ++i) {
		std::wstring path = L"assets/plants/wallnut/idle/";
		path += (i < 10 ? L"0" : L"");
		path += std::to_wstring(i) + L".png";
		loadimage(&wallnut[i], path.c_str());
	}

	// 加载阳光动画
	for (int i = 0; i < Sun::FRAME_COUNT; ++i) {
		std::wstring path = L"assets/effects/sun/spin/";
		path += (i < 10 ? L"0" : L"");
		path += std::to_wstring(i) + L".png";
		loadimage(&sunFrames[i], path.c_str());
	}

	// 加载僵尸行走动画
	for (int i = 0; i < Zombie::WALK_FRAME_COUNT; ++i) {
		std::wstring path = L"assets/zombies/normal/walk/";
		path += (i < 10 ? L"0" : L"");
		path += std::to_wstring(i) + L".png";
		loadimage(&zombieWalkFrames[i], path.c_str());
	}

	// 加载僵尸倒地动画
	for (int i = 0; i < Zombie::DIE_FRAME_COUNT; ++i) {
		std::wstring path = L"assets/zombies/normal/die/";
		path += (i < 10 ? L"0" : L"");
		path += std::to_wstring(i) + L".png";
		loadimage(&zombieDieFrames[i], path.c_str());
	}

	// 加载僵尸啃食动画
	for (int i = 0; i < Zombie::EAT_FRAME_COUNT; ++i) {
		std::wstring path = L"assets/zombies/normal/eat/";
		path += (i < 10 ? L"0" : L"");
		path += std::to_wstring(i) + L".png";
		loadimage(&zombieEatFrames[i], path.c_str());
	}

	// 加载植物选卡
	loadimage(&seedBank, _T("assets/ui/seed_bank.png"));
	loadimage(&peaCard, _T("assets/ui/cards/peashooter.png"));
	loadimage(&sunflowerCard, _T("assets/ui/cards/sunflower.png"));
	loadimage(&wallnutCard, _T("assets/ui/cards/wallnut.png"));

	// 加载豌豆子弹
	loadimage(&peaImage, L"assets/effects/pea.png");
	resizePng(peaImage, 28, 28);

	// 加载铲子
	loadimage(&shovelBank, L"assets/ui/shovel_bank.png");
	loadimage(&shovel, L"assets/ui/shovel.png");

	// 生成受击闪白图像缓存
	for (IMAGE& image : zombieWalkFrames) prepareWhiteImage(image);
	for (IMAGE& image : zombieEatFrames) prepareWhiteImage(image);
	for (IMAGE& image : zombieDieFrames) prepareWhiteImage(image);
}

void Game::playbgm() {
	// 开始播放背景音乐，失败时给出提示
	if (!Audio::playMusic(L"assets/music/day_grasswalk.mp3")) {
		MessageBoxW(GetHWnd(), L"背景音乐播放失败，请检查文件路径。",
			L"音频提示", MB_OK | MB_ICONWARNING);
	}
}

void Game::loadSoundEffects() {
	// 音效只在开局加载一次
	bool selectLoaded = Audio::loadSound(
		L"assets/sounds/select.wav", L"pvz_select");
	bool plantLoaded = Audio::loadSound(
		L"assets/sounds/plant.wav", L"pvz_plant");
	bool eatLoaded = Audio::loadSound(
		L"assets/sounds/eat.wav", L"pvz_eat");
	bool collectLoaded = Audio::loadSound(
		L"assets/sounds/collect_sun.wav", L"pvz_collect");
	bool shovelLoaded = Audio::loadSound(
		L"assets/sounds/shovel.wav", L"pvz_shovel");
	bool shootLoaded = Audio::loadSound(
		L"assets/sounds/shoot.wav", L"pvz_shoot");
	bool hitLoaded = Audio::loadSound(
		L"assets/sounds/hit.wav", L"pvz_hit");
	bool winLoaded = Audio::loadSound(
		L"assets/sounds/win.wav", L"pvz_win");
	bool loseLoaded = Audio::loadSound(
		L"assets/sounds/lose.wav", L"pvz_lose");

	if (!selectLoaded || !plantLoaded || !collectLoaded ||
		!shovelLoaded || !shootLoaded || !hitLoaded || !winLoaded || !loseLoaded || !eatLoaded) {
		MessageBoxW(GetHWnd(), L"部分音效加载失败。",
			L"音频提示", MB_OK | MB_ICONWARNING);
	}
}

void Game::processInput() {
	if (state != GameState::PLAYING) return;

	// 处理鼠标事件
	ExMessage msg;
	while (peekmessage(&msg, EX_MOUSE)) {
		mouseX = msg.x;
		mouseY = msg.y;
		// 右键取消拿取 || 在选卡UI范围内点击左键取消
		if (msg.message == WM_RBUTTONDOWN || 
			msg.message == WM_LBUTTONDOWN && isOnSeed(mouseX, mouseY) 
			&& (selectedPlant || shovelSelected)) {

			selectedPlant.reset();
			shovelSelected = false;
			continue;
		}
		if (msg.message != WM_LBUTTONDOWN) {
			continue;
		}

		// 点击铲子按钮：拿取或放回，并取消植物选择
		if (mouseX >= 465 && mouseX < 535 &&
			mouseY >= 5 && mouseY < 77) {
			selectedPlant.reset();
			Audio::playSound(L"pvz_shovel");
			shovelSelected = !shovelSelected;
			continue;
		}

		// 拿着铲子时，点击草坪中的植物将其移除
		if (shovelSelected && isOnLawn(mouseX, mouseY)) {
			int col = (mouseX - startX) / cellWidth;
			int row = (mouseY - startY) / cellHeight;

			if (plants[row][col]) {
				plants[row][col].reset();
				Audio::playSound(L"pvz_plant");
				shovelSelected = false;
			}
			continue;
		}

		// 处理阳光
		bool collectedSun = false;
		for (auto it = suns.begin(); it != suns.end(); ++it) {
			if (it->contains(mouseX, mouseY)) {
				it->beginCollect(50, 35);
				Audio::playSound(L"pvz_collect");
				collectedSun = true;
				break;
			}
		}
		if (collectedSun) {
			continue;
		}

		// 处理种子栏
		if (mouseX >= 10 && mouseX < 456 &&
			mouseY >= 0 && mouseY < 87) {
			if (mouseX >= 95 && mouseX < 145 &&
				mouseY >= 8 && mouseY < 78 && 
				sunCount >= Peashooter::COST) {

				selectedPlant = std::make_unique<Peashooter>(peashooter); // 豌豆射手
				Audio::playSound(L"pvz_select");
			}
			else if (mouseX >= 150 && mouseX < 200 &&
				mouseY >= 8 && mouseY < 78 &&
				sunCount >= Sunflower::COST) {

				selectedPlant = std::make_unique<Sunflower>(sunflower); // 向日葵
				Audio::playSound(L"pvz_select");
			}
			else if (mouseX >= 205 && mouseX < 255 &&
				mouseY >= 8 && mouseY < 78 &&
				sunCount >= Wallnut::COST) {

				selectedPlant = std::make_unique<Wallnut>(wallnut); // 坚果
				Audio::playSound(L"pvz_select");
			}
			continue;
		}
		// 拿着植物时，才允许在草坪空格种植
		if (selectedPlant && isOnLawn(mouseX, mouseY)) {
			int col = (mouseX - startX) / cellWidth;
			int row = (mouseY - startY) / cellHeight;
			int cost = selectedPlant->getCost();

			// 格子为空且阳光足够，种植成功后扣费
			if (!plants[row][col] && sunCount >= cost) {
				selectedPlant->setCell(row, col);
				plants[row][col] = std::move(selectedPlant);
				sunCount -= cost;
				Audio::playSound(L"pvz_plant");
			}
		}
	}
}

void Game::update() {
	// 结算后停止所有游戏计时、生成、移动、攻击及碰撞处理。
	if (state != GameState::PLAYING) return;

	// 阳光掉落
	ULONGLONG now = GetTickCount64();
	if (now - lastSkySunTime >= SKY_SUN_INTERVAL) {
		spawnSkySun();
		lastSkySunTime = now;
	}

	// 随机生成僵尸
	int aliveCount = 0;
	for (const Zombie& zombie : zombies) {
		if (!zombie.isDead()) ++aliveCount;
	}
	if (waveManager.update(now, aliveCount)) {
		// 随机生成僵尸
		// 随机选择第 0～4 行
		std::uniform_int_distribution<int> randomRow(0, rowCount - 1);
		int row;
		do {
			row = randomRow(randomEngine);
		} while (row == previousSpawnRow); // 同一行就再随机

		spawnZombie(row);
		previousSpawnRow = row;
	}

	// 更新场上的植物
	for (int row = 0; row < rowCount; ++row) {
		for (int col = 0; col < colCount; ++col) {
			if (plants[row][col]) plants[row][col]->update(*this);
		}
	}

	// 更新场上的阳光
	for (auto it = suns.begin(); it != suns.end();) {
		it->update();
		if (it->isCollected()) {
			sunCount += Sun::VALUE;
			it = suns.erase(it);
		}
		else if (it->isExpired()) {
			it = suns.erase(it);
		}
		else {
			++it;
		}
	}

	for (auto& row : zombiesByRow) {
		row.clear();
	}

	// 更新场上的僵尸
	for (Zombie& zombie : zombies) {
		zombie.update(*this);
		if (!zombie.isDead()) {
			zombiesByRow[zombie.getRow()].push_back(&zombie);
		}
	}

	// 更新场上的子弹
	for (auto it = bullets.begin(); it != bullets.end();) {
		double previousX = it->getX();
		it->update();

		// 判定命中僵尸
		Zombie* target = nullptr;
		for (Zombie* zombie : zombiesByRow[it->getRow()]) {
			// 可能已被这一帧前面的子弹击杀
			if (zombie->isDead()) continue;

			// 僵尸左右判定范围
			double left = zombie->getX() - 34;
			double right = zombie->getX() + 34;

			// 当前子弹横坐标小于僵尸右边界 且 大于左边界
			if (previousX <= right && it->getX() >= left) {
				// 优先选择最左边的僵尸为目标
				if (!target || zombie->getX() < target->getX()) {
					target = zombie;
				}
			}
		}

		if (target) {
			target->takeDamage(Bullet::DAMAGE);
			Audio::playSound(L"pvz_hit");
		}
		if (target || it->isOffscreen()) it = bullets.erase(it); // 移除子弹
		else ++it;
	}

	// 处理完所有子弹后 移除死亡僵尸
	for (auto it = zombies.begin(); it != zombies.end();) {
		if (it->canRemove()) it = zombies.erase(it);
		else ++it;
	}

	// 所有僵尸攻击完成后，统一移除被吃掉的植物
	for (int row = 0; row < rowCount; ++row) {
		for (int col = 0; col < colCount; ++col) {
			if (plants[row][col] && plants[row][col]->isDead()) {
				plants[row][col].reset();
			}
		}
	}

	// 检查游戏状态
	// 优先检查失败：死亡僵尸不算闯入房屋
	for (const Zombie& zombie : zombies) {
		if (!zombie.isDead() && zombie.getX() < 0.0) {
			endGame(GameState::LOST);
			return;
		}
	}

	// 生成完全部僵尸，并且倒地动画也已播放完毕
	if (waveManager.allSpawned() && zombies.empty()) {
		endGame(GameState::WON);
	}
}

void Game::draw() { // 按顺序绘制，最后绘制的在最上面
	// 绘制背景
	putimage(0, 0, &background);
	// drawGrid();
	drawPlants();
	drawHover();
	drawUi();
	for (Sun& sun : suns) {
		sun.draw();
	}
	for (Zombie& zombie : zombies) {
		zombie.draw();
	}
	for (Bullet& bullet : bullets) {
		bullet.draw();
	}
	drawSelected();
	// 铲子图片中心跟随鼠标
	if (shovelSelected) {
		drawPng(mouseX - 27, mouseY - 29, 54, 58, shovel);
	}

	// 胜利 或 失败 后的文字绘画
	if (state != GameState::PLAYING) {
		drawResult();
	}
}

void Game::drawResult() const {
	const wchar_t* text = state == GameState::WON
		? L"关卡通过" : L"僵尸吃掉了你的脑子";
	const int outline = 8;
	const int availableWidth = getwidth() - 80 - outline * 2;
	int fontSize = 104;

	LOGFONT previousFont;
	gettextstyle(&previousFont);
	LOGFONT resultFont = previousFont;
	resultFont.lfWidth = 0;
	resultFont.lfWeight = FW_HEAVY;
	resultFont.lfItalic = FALSE;
	resultFont.lfUnderline = FALSE;
	resultFont.lfStrikeOut = FALSE;
	resultFont.lfQuality = ANTIALIASED_QUALITY;
	_tcscpy_s(resultFont.lfFaceName, _T("微软雅黑"));
	do {
		resultFont.lfHeight = fontSize;
		settextstyle(&resultFont);
		if (textwidth(text) <= availableWidth || fontSize <= 40) break;
		fontSize -= 2;
	} while (true);

	int x = (getwidth() - textwidth(text)) / 2;
	int y = (getheight() - textheight(text)) / 2;
	COLORREF previousColor = gettextcolor();
	int previousBackground = getbkmode();
	setbkmode(TRANSPARENT);

	// 沿圆形范围叠画黑色文字，形成连续、均匀的粗描边。
	settextcolor(RGB(0, 0, 0));
	for (int dy = -outline; dy <= outline; ++dy) {
		for (int dx = -outline; dx <= outline; ++dx) {
			if (dx * dx + dy * dy <= outline * outline) {
				outtextxy(x + dx, y + dy, text);
			}
		}
	}
	settextcolor(state == GameState::WON
		? RGB(255, 230, 60) : RGB(255, 90, 90));
	outtextxy(x, y, text);

	settextstyle(&previousFont);
	settextcolor(previousColor);
	setbkmode(previousBackground);
}

void Game::drawGrid() {
	// 绘制草坪网格
	setlinecolor(RGB(120, 180, 80));
	setlinestyle(PS_SOLID, 1);
	// 9 列
	for (int col = 0; col <= colCount; ++col) {
		int x = startX + col * cellWidth;
		line(x, startY, x, startY + rowCount * cellHeight);
	}
	// 5 行
	for (int row = 0; row <= rowCount; ++row) {
		int y = startY + row * cellHeight;
		line(startX, y, startX + colCount * cellWidth, y);
	}
}

void Game::drawPlants() {
	// 格子中有植物对象，则调用它的绘制方法
	for (int row = 0; row < rowCount; ++row) {
		for (int col = 0; col < colCount; ++col) {
			if (plants[row][col]) {
				plants[row][col]->draw();
			}
		}
	}
}

void Game::drawHover() {
	// 只有拿着植物、位于草坪且未被种子栏遮挡时才预览
	if (state != GameState::PLAYING || !selectedPlant ||
		shovelSelected || !isOnLawn(mouseX, mouseY) ||
		isOnSeed(mouseX, mouseY)) {
		return;
	}

	// 鼠标所在行列
	int col = (mouseX - startX) / cellWidth;
	int row = (mouseY - startY) / cellHeight;

	// 已占用的格子不显示放置预览
	if (plants[row][col]) return;

	int footX = startX + col * cellWidth + cellWidth / 2;
	int footY = startY + row * cellHeight + cellHeight - 10;

	selectedPlant->drawPlacementPreview(footX, footY);
	
}

void Game::drawUi() {
	// 绘制选卡UI
	drawPng(10, 0, 446, 87, seedBank);
	drawPng(95, 8, 50, 70, peaCard);
	drawPng(150, 8, 50, 70, sunflowerCard);
	drawPng(205, 8, 50, 70, wallnutCard);
	// 铲子位于种子栏右侧
	drawPng(465, 5, 70, 72, shovelBank);
	if (!shovelSelected) {
		drawPng(473, 11, 54, 58, shovel);
	}

	// 绘制太阳数量
	std::wstring sunText = std::to_wstring(sunCount);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(30, 30, 30));
	settextstyle(20, 0, _T("微软雅黑"));


	int textX = 15 + (65 - textwidth(sunText.c_str())) / 2;
	outtextxy(textX, 61, sunText.c_str());
}


void Game::drawSelected() {
	// 绘制选择植物后，鼠标上面的植物跟随图
	if (selectedPlant) {
		selectedPlant->drawPreview(mouseX, mouseY);
	}
	// 绘制鼠标拿着的铲子
	if (shovelSelected) {
		drawPng(mouseX - 27, mouseY - 29, 54, 58, shovel);
	}
}

void Game::spawnSun(int x, int y) {
	suns.emplace_back(x, y, sunFrames);
	suns.back().beginSpawn(x + 35, y + 60);
}

void Game::spawnSkySun() {
	// 留出图片半径，避免落地时超出草坪
	std::uniform_int_distribution<int> randomX(
		startX + 48, startX + colCount * cellWidth - 48);
	std::uniform_int_distribution<int> randomY(
		startY + 80, startY + rowCount * cellHeight - 48);

	int x = randomX(randomEngine);
	int groundY = randomY(randomEngine);

	suns.emplace_back(x, -48, sunFrames);
	suns.back().beginFall(groundY);
}

void Game::spawnZombie(int row) {
	zombies.emplace_back(row, zombieWalkFrames, zombieDieFrames, zombieEatFrames);
}

bool Game::isOnLawn(int x, int y) const {
	return x >= startX && x < startX + colCount * cellWidth &&
		y >= startY && y < startY + rowCount * cellHeight;
}

bool Game::isOnSeed(int x, int y) const {
	return x >= 10 && x < 456 && y >= 0 && y < 87;
}

bool Game::hasZombieAhead(int row, int col) const {
	int muzzleX = startX + col * cellWidth + cellWidth / 2 + 5 + 35; // 射手嘴的横坐标
	for (const Zombie& zombie : zombies) {
		// 如果僵尸在射手的同一列 且 僵尸在射手右边
		if (!zombie.isDead() && zombie.getRow() == row && zombie.getX() > muzzleX) {
			return true;
		}
	}
	return false;
}

void Game::endGame(GameState result) {
	if (state != GameState::PLAYING) return; // 防止重复结算
	state = result;
	selectedPlant.reset();
	shovelSelected = false;

	Audio::stopMusic();
	Audio::stopSounds();
	Audio::playSound(
		result == GameState::WON ? L"pvz_win" : L"pvz_lose");
}
