#pragma once

#include "GameLogic.h"

class DemoGameLogic : public Bismuth::GameLogic {
public:
	DemoGameLogic(int numberOfPlayers);
	DemoGameLogic(std::string host);

	virtual ~DemoGameLogic();
};