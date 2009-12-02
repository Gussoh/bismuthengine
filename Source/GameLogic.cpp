/**
 * @file GameLogic.cpp
 */

#include "stdafx.h"

#include "Entity.h"
#include "GameLogic.h"
#include "FmodAudioManager.h"
#include "OgreNewtPhysicsManager.h"
#include "RakNetworkManager.h"
#include "OISInputManager.h"
#include <ctime>

using namespace Bismuth;
using namespace Bismuth::Audio;
using namespace Bismuth::Physics;
using namespace Bismuth::Network;
using namespace Bismuth::Graphics;
using namespace Bismuth::Input;

GameLogic::GameLogic(bool isServer) : isServer(isServer), nextEntityId(0) {
	// Create ogre root
	Ogre::Root *root = new Ogre::Root("", "", "OgreLog.txt");

	// Renderer must be created first since a valid instance is needed by the physics manager.
	// What about isServer??
	this->renderer = new Renderer();
	this->renderer->init(800, 600, false); // Todo: should not be hard coded

	this->audioManager = new FmodAudioManager(this);
	this->physicsManager = new OgreNewtPhysicsManager(this);
	this->networkManager = new RakNetworkManager(this);
	this->inputManager = new OISInputManager(this->renderer->getWindowHandle(), 800, 600);

	initResourceLocations();
}

GameLogic::~GameLogic() {
	entities.clear();
	delete inputManager;
	delete networkManager;
	delete physicsManager;
	delete audioManager;
	delete renderer;

	delete Ogre::Root::getSingletonPtr();
}

void GameLogic::initResourceLocations() {
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\..\\..\\Assets", "FileSystem", "General", true);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

SharedPtr<Entity> GameLogic::getEntityById(int id) {
	// Todo: Fix this
	
	return SharedPtr<Entity>();
}

void GameLogic::update() {
	if (isServer) {
		for(;;) {
			clock_t lastUpdate = std::clock();
			SharedPtr<Message> m = networkManager->getMessage(false);
			if (!m.isNull()) {
				if (m->getType() == MsgEndOfFrame) {
					lastUpdate = std::clock();
					// save current time in variable
					// do physics with m.getStep();
				} else {
					// collect messages an propagate onto network
					handleMessage(m);
				}
			} else {
				// sleep  a while ?
			}
			
			// if time since last end of frame goes beyond threshold. send end of fram.
			float step = (float) (std::clock() - lastUpdate) / CLOCKS_PER_SEC;
			if (step > .01) {
				// send eof
				networkManager->sendEndOfFrame(step);
			}
			
		}
	} else {
		SharedPtr<Message> m = networkManager->getMessage(true);
		if (m->getType() == MsgEndOfFrame) {
			// do physics with m.getStep();
		} else {
			handleMessage(m);
		}
		// collect keypresses and stuff
		// send them onto network.
	}



	physicsManager->update(elapsedTime);
	inputManager->update();
	//audioManager->update();

	/*SharedPtr<Message> message;
	while (!(message = networkManager->getMessage()).isNull()) {
		if (isServer) {

		} else { // all messages from server should be processed
			messageQueue.push(message);
		}
	}

	while (!messageQueue.empty()) {
		SharedPtr<Message> message = messageQueue.front();
		messageQueue.pop();

		handleMessage(message);
	}

	if (isServer) {
		networkManager->sendEntities(entities);

		networkManager->sendEndOfFrame();
	} else {
		networkManager->nextFrame();
	}*/
}

void GameLogic::render(){
	// Just delegate to the renderer
	renderer->render();
}

void GameLogic::sendMessage(SharedPtr<Message> message) {
	if (isServer) {
		messageQueue.push(message);	
	}

	// Todo: Need to filter out collision messages on the client,
	// as they should not be sent to the server.
	networkManager->sendMessage(message);
}

void GameLogic::handleMessage(SharedPtr<Message> message) {
	switch (message->getType()) {
		case MsgDebugOut:
			handleDebugOutMessage(message);
			break;
		case MsgEntityAssigned:
			handleEntityAssignedMessage(message);
			break;
		default:
			break;
	}
}

void GameLogic::handleDebugOutMessage(SharedPtr<Message> message) {
	GET_MSG(DebugOutMessage);
	std::cout << msg->getText();
}

void GameLogic::handleEntityAssignedMessage(SharedPtr<Message> message) {
	GET_MSG(EntityAssignedMessage);

	// Todo: check playerId 
	setPlayerEntity(getEntityById(msg->getEntityId()));
}

void GameLogic::loadWorld(const std::string &name) {
	// Todo: Write a basic load function
	
}

SharedPtr<Entity> GameLogic::createEntity() {
	SharedPtr<Entity> entity = SharedPtr<Entity>(new Entity(nextEntityId));
	entities.push_back(entity);
	
	Ogre::SceneNode *node = renderer->getDefaultSceneManager()->getRootSceneNode()->createChildSceneNode("Node" + entity->getId());
	entity->setSceneNode(node);

	nextEntityId++;

	return entity;
}

SharedPtr<Entity> GameLogic::createEntity(const Ogre::String &meshName) {
	SharedPtr<Entity> entity = createEntity();
	Ogre::Entity *mesh = renderer->getDefaultSceneManager()->createEntity("Mesh" + entity->getId(), meshName);

	entity->getSceneNode()->attachObject(mesh);

	return entity;
}

void GameLogic::setCameraEntity(SharedPtr<Entity> &entity) { 
	if (!cameraEntity.isNull()) {
		cameraEntity->getSceneNode()->detachObject(renderer->getDefaultCamera());
	}

	entity->getSceneNode()->attachObject(renderer->getDefaultCamera());
	
	cameraEntity = entity; 
}
