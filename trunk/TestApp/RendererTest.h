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
		entityMsg->setMeshName("Models/floor.mesh");
		entityMsg->setEntityType(ET_static);
		entityMsg->setEntityMaterial(EMT_stone);
		entityMsg->setScale(10.0f);
		gameLogic->sendMessage(entityMsg);

		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setMeshName("Models/ramp.mesh");
		entityMsg->setEntityType(ET_static);
		entityMsg->setEntityMaterial(EMT_stone);
		entityMsg->setScale(10.0f);
		gameLogic->sendMessage(entityMsg);

		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setMeshName("Models/walls.mesh");
		entityMsg->setEntityType(ET_static);
		entityMsg->setEntityMaterial(EMT_stone);
		entityMsg->setScale(10.0f);
		gameLogic->sendMessage(entityMsg);

		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setMeshName("Models/wall_01.mesh");
		entityMsg->setEntityType(ET_static);
		entityMsg->setEntityMaterial(EMT_stone);
		entityMsg->setScale(10.0f);
		gameLogic->sendMessage(entityMsg);

		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setMeshName("Models/outer_ramp.mesh");
		entityMsg->setEntityType(ET_static);
		entityMsg->setEntityMaterial(EMT_stone);
		entityMsg->setScale(10.0f);
		gameLogic->sendMessage(entityMsg);

		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setMeshName("Models/shelter.mesh");
		entityMsg->setEntityType(ET_static);
		entityMsg->setEntityMaterial(EMT_stone);
		entityMsg->setScale(10.0f);
		gameLogic->sendMessage(entityMsg);

		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setMeshName("Models/Room_SeeTrough.mesh");
		entityMsg->setEntityType(ET_static);
		entityMsg->setEntityMaterial(EMT_rubber);
		gameLogic->sendMessage(entityMsg);



		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setMeshName("Models/ninja.mesh");
		entityMsg->setPosition(Ogre::Vector3(1, 1, 2));
		entityMsg->setEntityType(ET_dynamic);
		entityMsg->setEntityMaterial(EMT_wood);

		Audio::AudioProperties audioProperties;
		audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Create, "Audio/startgame1.wav"));
		audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Collision, "Audio/metal1.wav"));
		entityMsg->setAudioProperties(audioProperties);
		gameLogic->sendMessage(entityMsg);
		//gameLogic->getAudioManager()->playSoundtrack();

		entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		entityMsg->setPosition(Ogre::Vector3(0, 1, 0));
		entityMsg->setEntityType(ET_light);
		entityMsg->setEntityMaterial(EMT_wood);
		gameLogic->sendMessage(entityMsg);

		/*for(int i = 0; i < 49; i++) {
			entityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
			entityMsg->setMeshName("Models/boringBox.mesh");
			entityMsg->setPosition(Ogre::Vector3((i % 7) - 5 * 2, 3.5f * (i / 7), -4));
			entityMsg->setEntityType(ET_dynamic);
			entityMsg->setEntityMaterial(EMT_wood);
			entityMsg->setScale(4.0f);
			Audio::AudioProperties audioProperties;
			audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Collision, "Audio/wood1.wav"));
			entityMsg->setAudioProperties(audioProperties);
			gameLogic->sendMessage(entityMsg);
		}*/

		for(int i = 0; i < gameLogic->getNumberOfPlayers(); i++) {
			SharedPtr<CreateEntityMessage> playerEntityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
			playerEntityMsg->setMeshName("Models/ninja.mesh");
			playerEntityMsg->setEntityType(ET_player);
			playerEntityMsg->setEntityMaterial(EMT_player);
			playerEntityMsg->setPosition(Ogre::Vector3(2, 1, -2 + i));
			Audio::AudioProperties audioProperties;
			audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Hurt, "Audio/hurt1.wav"));
			audioProperties.sounds.insert(std::make_pair(Audio::SoundType_MajorHurt, "Audio/majorhurt1.wav"));
			audioProperties.sounds.insert(std::make_pair(Audio::SoundType_MinorHurt, "Audio/minorhurt1.wav"));
			audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Destroy, "Audio/majorhurt1.wav"));
			audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Spawn, "Audio/spawn1.wav"));
			playerEntityMsg->setAudioProperties(audioProperties);
			gameLogic->sendMessage(playerEntityMsg);
		}

		SharedPtr<CreateEntityMessage> spawnEntityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		spawnEntityMsg->setEntityType(ET_spawnarea);
		spawnEntityMsg->setPosition(Ogre::Vector3(2, 2.5f, -3));
		gameLogic->sendMessage(spawnEntityMsg);
		spawnEntityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		spawnEntityMsg->setEntityType(ET_spawnarea);
		spawnEntityMsg->setPosition(Ogre::Vector3(6, 2.5f, -6));
		gameLogic->sendMessage(spawnEntityMsg);
		spawnEntityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
		spawnEntityMsg->setEntityType(ET_spawnarea);
		spawnEntityMsg->setPosition(Ogre::Vector3(-20, 2.5f, 20));
		gameLogic->sendMessage(spawnEntityMsg);
		// Handle all sent messages and send them to clients.
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
		
		std::cout << "Server? (y/N) ";
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
			gameLogic->render();
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
