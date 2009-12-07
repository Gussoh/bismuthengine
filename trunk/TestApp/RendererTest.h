#pragma once

#include "Renderer.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include <string>

using namespace Bismuth;
using namespace Bismuth::Graphics;

class RendererTest : public Test {
public:

	virtual void run() {
		std::cout << "Renderer test" << std::endl;
		
		std::cout << "Server? (y/n) ";
		char isServer;
		std::cin >> isServer;
		GameLogic *gameLogic;

		if (isServer == 'y') {
			std::cout << std::endl << "Number of players: ";
			int numberOfPlayers;
			std::cin >> numberOfPlayers;
			std::cout << std::endl;
			gameLogic = new GameLogic(numberOfPlayers);
		} else {
			std::cout << std::endl << "Server host: ";
			std::string host;
			std::cin >> host;
			std::cout << std::endl;
			gameLogic = new GameLogic(host);
		}

		gameLogic->loadWorld("test1");

		Renderer *renderer = gameLogic->getRenderer();

		SharedPtr<Entity> entity = gameLogic->createEntity("Models/Room.mesh");
		entity->setType(ET_static);
		entity->setMaterial(EMT_stone);

		entity = gameLogic->createEntity("Models/Room_SeeTrough.mesh");
		entity->setType(ET_static);
		entity->setMaterial(EMT_stone);
	
		entity = gameLogic->createEntity("Models/Box01.mesh");
		entity->setPosition(Ogre::Vector3(1, 2, 2));
		entity->setType(ET_dynamic);
		entity->setMaterial(EMT_wood);
		

		SharedPtr<Entity> playerEntity = gameLogic->createEntity();
		playerEntity->setType(ET_player);
		playerEntity->setPosition(Ogre::Vector3(2, 2, 2));

		// Set the camera to follow the player entity
		gameLogic->setPlayerEntity(playerEntity);
		gameLogic->setCameraEntity(playerEntity);
		
		entity->getAudioPropertiesPtr()->soundType = SoundType_Continuous;
		entity->getAudioPropertiesPtr()->directivity = 0;
		gameLogic->getAudioManager()->playSound(entity);

		//camera->lookAt(entity->getPosition());
		
		while (renderer->isWindowOpen()) {
			/*Ogre::Vector3 mousePosition = gameLogic.getInputManager()->getRelativeMousePosition();
			SharedPtr<Entity> playerEntity = gameLogic.getPlayerEntity();
			gameLogic.getRenderer()->getDefaultCamera()->pitch(Ogre::Radian(-mousePosition.y * 0.005f));

			SharedPtr<PlayerRotateMessage> rotateMsg = SharedPtr<PlayerRotateMessage>(new PlayerRotateMessage(&gameLogic, Ogre::Radian(-mousePosition.y * 0.005f)));
			playerEntity->getSceneNode()->yaw(Ogre::Radian(-mousePosition.x * 0.005f));
			playerEntity->setPositionOrientationChanged(true);

			if (gameLogic.getInputManager()->isKeyDown(Input::KC_W)) {
				SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(&gameLogic, Input::KC_W));
				gameLogic.sendMessage(moveMsg);
			}
			if (gameLogic.getInputManager()->isKeyDown(Input::KC_S)) {
				SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(&gameLogic, Input::KC_S));
				gameLogic.sendMessage(moveMsg);
			} 
			if (gameLogic.getInputManager()->isKeyDown(Input::KC_A)) {
				SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(&gameLogic, Input::KC_A));
				gameLogic.sendMessage(moveMsg);
			}
			if (gameLogic.getInputManager()->isKeyDown(Input::KC_D)) {
				SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(&gameLogic, Input::KC_D));
				gameLogic.sendMessage(moveMsg);
			} 
			if (gameLogic.getInputManager()->isKeyDown(Input::KC_SPACE)) {
				SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(&gameLogic, Input::KC_SPACE));
				gameLogic.sendMessage(moveMsg);
			}*/

			gameLogic->update();
			gameLogic->render();
		}
	}
};
