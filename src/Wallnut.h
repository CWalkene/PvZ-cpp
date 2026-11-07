#pragma once

#include "Plant.h"

class IMAGE;

class Wallnut : public Plant {
public:
	static constexpr int COST = 50;

	explicit Wallnut(IMAGE* frames) : Plant(frames, 17, 600) {}

	int getCost() const override { return COST; }
};
