#pragma once


#include "GameLogic.h"
#include <iostream>

using namespace Bismuth;
using namespace Bismuth::Physics;

class PhysicsTest : public Test {
public:

	virtual void run() {
		
		GameLogic logic(true);
		physics = logic.getPhysicsManager();

	}
};