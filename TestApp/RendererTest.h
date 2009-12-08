#pragma once

#include "Renderer.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "Entity.h"
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
		entityMsg->setMeshName("Models/ninja.mesh");
		entityMsg->setPosition(Ogre::Vector3(1, 1, 2));
		entityMsg->setEntityType(ET_dynamic);
		entityMsg->setEntityMaterial(EMT_wood);
		Audio::AudioProperties audioProperties;
		audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Collision, "Audio/jaguar.wav"));
		entityMsg->setAudioProperties(audioProperties);
		gameLogic->sendMessage(entityMsg);

		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setPosition(Ogre::Vector3(0, 2, 0));
		entityMsg->setEntityType(ET_light);
		entityMsg->setEntityMaterial(EMT_wood);
		gameLogic->sendMessage(entityMsg);

		for(int i = 0; i < gameLogic->getNumberOfPlayers(); i++) {
			SharedPtr<CreateEntityMessage> playerEntityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
			playerEntityMsg->setMeshName("Models/Box01.mesh");
			playerEntityMsg->setEntityType(ET_player);
			playerEntityMsg->setPosition(Ogre::Vector3(2, 2, -2 + i));
			gameLogic->sendMessage(playerEntityMsg);
			
		}

		// Handle all sent messages.
		gameLogic->update();
		
		int currentPlayer = 0;
		for (EntityList::iterator iter = gameLogic->getEntities()->begin(); 
			iter != gameLogic->getEntities()->end(); 
			iter++) {
			
			if(iter->second->getType() == ET_player) {
				std::cout << std::endl << "Found entity with type player" << std::endl;
				gameLogic->sendMessage(SharedPtr<EntityAssignedMessage>(new EntityAssignedMessage(iter->second->getId(), currentPlayer)));
				currentPlayer++;
			}
		}
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
		while (gameLogic->getRenderer()->isWindowOpen()) {
			gameLogic->update();
			gameLogic->render();
		}
	}
};
