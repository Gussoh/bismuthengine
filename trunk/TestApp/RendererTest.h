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
		
		entity->getAudioPropertiesPtr()->soundType = SoundType_Continuous;
		entity->getAudioPropertiesPtr()->directivity = 0;
		gameLogic.getAudioManager()->playSound(entity);

		//camera->lookAt(entity->getPosition());
		
		while (renderer->isWindowOpen()) {
			Ogre::Vector3 mousePosition = gameLogic.getInputManager()->getRelativeMousePosition();
			SharedPtr<Entity> playerEntity = gameLogic.getPlayerEntity();
			playerEntity->getSceneNode()->pitch(Ogre::Radian(-mousePosition.y * 0.005f));
			playerEntity->getSceneNode()->yaw(Ogre::Radian(-mousePosition.x * 0.005f));
			playerEntity->setPositionOrientationChanged(true);

			if (gameLogic.getInputManager()->isKeyDown(Input::KC_W)) {
				SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(&gameLogic, Input::KC_W));
				gameLogic.sendMessage(moveMsg);
			} else if (gameLogic.getInputManager()->isKeyDown(Input::KC_S)) {
				SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(&gameLogic, Input::KC_S));
				gameLogic.sendMessage(moveMsg);
			} 
			
			if (gameLogic.getInputManager()->isKeyDown(Input::KC_A)) {
				SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(&gameLogic, Input::KC_A));
				gameLogic.sendMessage(moveMsg);
			} else if (gameLogic.getInputManager()->isKeyDown(Input::KC_D)) {
				SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(&gameLogic, Input::KC_D));
				gameLogic.sendMessage(moveMsg);
			} 

			gameLogic.update();
			gameLogic.render();
		}
	}
};
