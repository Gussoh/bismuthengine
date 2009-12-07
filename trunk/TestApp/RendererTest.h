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

		GameLogic gameLogic(true);
		gameLogic.loadWorld("test1");

		Renderer *renderer = gameLogic.getRenderer();

		SharedPtr<Entity> entity = gameLogic.createEntity("Models/Room.mesh");
		entity->setType(ET_static);
		entity->setMaterial(EMT_stone);

		entity = gameLogic.createEntity("Models/Room_SeeTrough.mesh");
		entity->setType(ET_static);
		entity->setMaterial(EMT_stone);
	
		entity = gameLogic.createEntity("Models/Box01.mesh");
		entity->setPosition(Ogre::Vector3(1, 2, 2));
		entity->setType(ET_dynamic);
		entity->setMaterial(EMT_wood);

		SharedPtr<Entity> playerEntity = gameLogic.createEntity();
		playerEntity->setType(ET_player);
		playerEntity->setPosition(Ogre::Vector3(2, 2, 2));

		// Set the camera to follow the player entity
		gameLogic.setPlayerEntity(playerEntity);
		gameLogic.setCameraEntity(playerEntity);
		

		//camera->lookAt(entity->getPosition());
		
		while (renderer->isWindowOpen()) {
			Ogre::Vector3 mousePosition = gameLogic.getInputManager()->getRelativeMousePosition();
			SharedPtr<Entity> playerEntity = gameLogic.getPlayerEntity();
			playerEntity->getSceneNode()->pitch(Ogre::Radian(-mousePosition.y * 0.005f));
			playerEntity->getSceneNode()->yaw(Ogre::Radian(-mousePosition.x * 0.005f));
			playerEntity->setPositionOrientationChanged(true);

			if (gameLogic.getInputManager()->isKeyDown(Input::KC_W)) {
				gameLogic.getPhysicsManager()->addImpulse(playerEntity, Ogre::Vector3(0, 0, -0.25f));
				//camera->moveRelative(Ogre::Vector3(0, 0, -0.1f));
			} else if (gameLogic.getInputManager()->isKeyDown(Input::KC_S)) {
				gameLogic.getPhysicsManager()->addImpulse(playerEntity, Ogre::Vector3(0, 0, 0.25f));
				//camera->moveRelative(Ogre::Vector3(0, 0, 0.1f));
			} else if (gameLogic.getInputManager()->isKeyDown(Input::KC_A)) {
				gameLogic.getPhysicsManager()->addImpulse(playerEntity, Ogre::Vector3(-0.25, 0, 0));
				//camera->moveRelative(Ogre::Vector3(-0.1f, 0, 0.0f));
			} else if (gameLogic.getInputManager()->isKeyDown(Input::KC_D)) {
				//camera->moveRelative(Ogre::Vector3(0.1f, 0, 0.0f));
				gameLogic.getPhysicsManager()->addImpulse(playerEntity, Ogre::Vector3(0.25, 0, 0));
			} 

			//gameLogic.update();
			gameLogic.render();
		}
	}
};
