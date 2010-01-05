#pragma once

#include "Application.h"

class DemoApplication : public Bismuth::Application {
public:
	DemoApplication(bool isServer, std::string host = "", int numberOfPlayers = 0);
	virtual ~DemoApplication();

	virtual void initGame();

	void loadWorld();

	virtual Bismuth::GameLogic *createGameLogic();
};