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

	virtual void loadWorld(GameLogic *gameLogic) {

		SharedPtr<CreateEntityMessage> entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setMeshName("Models/Room.mesh");
		entityMsg->setEntityType(ET_static);
		entityMsg->setEntityMaterial(EMT_stone);
		gameLogic->sendMessage(entityMsg);

		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setMeshName("Models/Room_SeeTrough.mesh");
		entityMsg->setEntityType(ET_static);
		entityMsg->setEntityMaterial(EMT_stone);
		gameLogic->sendMessage(entityMsg);


		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setMeshName("Models/Box01.mesh");
		entityMsg->setPosition(Ogre::Vector3(1, 2, 2));
		entityMsg->setEntityType(ET_dynamic);
		entityMsg->setEntityMaterial(EMT_wood);
		gameLogic->sendMessage(entityMsg);
	
		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setEntityType(ET_player);
		entityMsg->setPosition(Ogre::Vector3(2, 2, 2));
		gameLogic->sendMessage(entityMsg);
	}

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

		// Wait for players to connect.
		while (!gameLogic->isGameStarted()) {
			// Waiting ...
		}

		if (isServer == 'y') {
			loadWorld(gameLogic);
		}

		Renderer *renderer = gameLogic->getRenderer();

		while (renderer->isWindowOpen()) {
			gameLogic->update();
			gameLogic->render();
		}
	}
};
