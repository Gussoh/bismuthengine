#pragma once

#include "Renderer.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreSceneNode.h"

using namespace Bismuth;
using namespace Bismuth::Graphics;

class RendererTest : public Test {
public:

	virtual void run() {
		std::cout << "Renderer test" << std::endl;

		GameLogic gameLogic(false);
		gameLogic.loadWorld("test1");

		Renderer *renderer = gameLogic.getRenderer();
		while (renderer->isWindowOpen()) {
			gameLogic.update(0.16f);
			gameLogic.render();
		}
	}
};
