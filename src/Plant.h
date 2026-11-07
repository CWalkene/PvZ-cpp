#pragma once

#include <Windows.h>

class IMAGE;
class Game;

class Plant {
public:
	// 引用共享动画，不为每株植物重新加载图片
	Plant(IMAGE* frames, int frameCount, int health = 100);
	virtual ~Plant() = default;

	// 由具体植物返回自己的 COST，不额外保存价格变量
	virtual int getCost() const = 0;

	// 设置种植位置
	void setCell(int row, int col);
	int getRow() const;
	int getCol() const;

	// 绘制种下的植物，以及鼠标上的静态预览
	virtual void draw();
	void drawPreview(int mouseX, int mouseY);
	void drawPlacementPreview(int footX, int footY) const;

	virtual void update(Game& game);

	void takeDamage(int damage);
	bool isDead() const { return health <= 0; }

protected:
	// 按当前植物的位置和尺寸绘制指定图
	void drawFrame(IMAGE& image) const;

private:
	int row = 0;
	int col = 0;

	// 图片由 Game 持有，植物只引用，不负责释放
	IMAGE* frames;
	int frameCount; // 总帧
	int frame = 0; // 当前帧

	int health;

	// 每株植物独立记录动画开始时间
	ULONGLONG animationStart;
};
