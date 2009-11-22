/**
 * @file GameLogic.cpp
 */

#include "stdafx.h"

#include "Entity.h"
#include "GameLogic.h"
#include "FmodAudioManager.h"
#include "OgreNewtPhysicsManager.h"
#include "RakNetworkManager.h"

using namespace Bismuth;
using namespace Bismuth::Audio;
using namespace Bismuth::Physics;
using namespace Bismuth::Network;
using namespace Bismuth::Graphics;

GameLogic::GameLogic(bool isServer) : isServer(isServer) {
	// Create ogre root
	Ogre::Root *root = new Ogre::Root("", "", "OgreLog.txt");

	// Renderer must be created first since a valid instance is needed by the physics manager.
	this->renderer = new Renderer();
	this->renderer->init(800, 600, false); // Todo: should not be hard coded

	this->audioManager = new FmodAudioManager(this);
	this->physicsManager = new OgreNewtPhysicsManager(this);
	this->networkManager = new RakNetworkManager(this);

	initResourceLocations();
}

GameLogic::~GameLogic() {
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
	while (!messageQueue.empty()) {
		SharedPtr<Message> message = messageQueue.front();
		messageQueue.pop();

		handleMessage(message);

		if (isServer) {
			// Replicate message over the network
			networkManager->sendMessage(message);
		}
	}
}

void GameLogic::render(){
	// Just delegate to the renderer
	renderer->render();
}

void GameLogic::sendMessage(SharedPtr<Message> message) {
	messageQueue.push(message);
}

void GameLogic::handleMessage(SharedPtr<Message> message) {
	switch (message->getType()) {
		case MsgDebugOut:
			break;
		case MsgEntityAssigned:
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
	if (name == "test1") {
		Ogre::Entity *mesh = renderer->getDefaultSceneManager()->createEntity("stuffMesh", "Models/Room.mesh");
		Ogre::SceneNode *node = renderer->getDefaultSceneManager()->getRootSceneNode()->createChildSceneNode("stuff");
		node->attachObject(mesh);
		node->setPosition(0.0f, -1.0f, 0.0f);
		node->setOrientation(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3(0, 1, 0)));
	}
}