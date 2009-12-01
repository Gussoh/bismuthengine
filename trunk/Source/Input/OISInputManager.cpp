/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "OISInputManager.h"

using namespace Bismuth;
using namespace Bismuth::Input;

OISInputManager::OISInputManager(int windowHandle, int width, int height) {
	inputManager = OIS::InputManager::createInputSystem(windowHandle);

	mouse = (OIS::Mouse*)inputManager->createInputObject(OIS::OISMouse, true);
	keyboard = (OIS::Keyboard*)inputManager->createInputObject(OIS::OISKeyboard, true);

	const OIS::MouseState &ms = mouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

OISInputManager::~OISInputManager() {
	OIS::InputManager::destroyInputSystem(inputManager);
}

void OISInputManager::update() {
	mouse->capture();
	keyboard->capture();
}

Ogre::Vector3 OISInputManager::getRelativeMousePosition() {
	OIS::MouseState ms = mouse->getMouseState();
	return Ogre::Vector3(ms.X.rel, ms.Y.rel, ms.Z.rel);
}

bool OISInputManager::isKeyDown(KeyCode keyCode) {
	return keyboard->isKeyDown(OIS::KeyCode(keyCode));
}
