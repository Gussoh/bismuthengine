#pragma once

#include "Renderer.h"

using namespace Bismuth;
using namespace Bismuth::Graphics;

class RendererTest : public Test {
public:

	virtual void run() {
		std::cout << "Renderer test" << std::endl;

		Renderer renderer;
		renderer.init(800, 600, false);

		while (renderer.isWindowOpen()) {
			// Nothing to doooo
			renderer.messagePump();
		}

		renderer.kill();
	}
};
