#include "stdafx.h"
#include "Renderer.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "Entity.h"
#include "DemoGameLogic.h"

using namespace Bismuth;

DemoGameLogic::DemoGameLogic(std::string host) : GameLogic(host) {

}

DemoGameLogic::DemoGameLogic(int numberOfPlayers) : GameLogic(numberOfPlayers) {

}

DemoGameLogic::~DemoGameLogic() {

}