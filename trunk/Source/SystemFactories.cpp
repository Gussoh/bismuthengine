/**
 * @file SystemFactories.cpp
 */
#include "stdafx.h"
#include "OgreNewtPhysicsManager.h"
#include "FmodAudioManager.h"
#include "RakNetworkManager.h"
#include "OISInputManager.h"

using namespace Bismuth;
using namespace Bismuth::Physics;
using namespace Bismuth::Audio;
using namespace Bismuth::Network;
using namespace Bismuth::Input;

PhysicsManager *Bismuth::createPhysicsManager(GameLogic *gameLogic) {
	return new OgreNewtPhysicsManager(gameLogic);
}

AudioManager *Bismuth::createAudioManager(GameLogic *gameLogic) {
	return new FmodAudioManager(gameLogic);
}

NetworkManager *Bismuth::createNetworkManager(GameLogic *gameLogic) {
	return new RakNetworkManager(gameLogic);
}

InputManager *Bismuth::creatInputManager(int windowHandle, int width, int height) {
	return new OISInputManager(windowHandle, width, height);
}