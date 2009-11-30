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
	this->entityFactory = new EntityFactory();

	initResourceLocations();
}

GameLogic::~GameLogic() {
	entities.clear();
	delete entityFactory;
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

void GameLogic::update(float elapsedTime) {
	inputManager->update();

	SharedPtr<Message> message;
	while (!(message = networkManager->getMessage()).isNull()) {
		messageQueue.push(message);
	}

	while (!messageQueue.empty()) {
		SharedPtr<Message> message = messageQueue.front();
		messageQueue.pop();

		handleMessage(message);
	}

	if (isServer) {
		networkManager->sendEndOfFrame();
	} else {
		networkManager->nextFrame();
	}
}

void GameLogic::render(){
	// Just delegate to the renderer
	renderer->render();
}

void GameLogic::sendMessage(SharedPtr<Message> message) {
	messageQueue.push(message);
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
	nextEntityId++;
	entities.push_back(entity);

	return entity;
}
