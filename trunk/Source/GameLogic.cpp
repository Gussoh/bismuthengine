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
	// Renderer must be created first since a valid instance is needed by the physics manager.
	this->renderer = new Renderer();
	this->audioManager = new FmodAudioManager(this);
	this->physicsManager = new OgreNewtPhysicsManager(this);
	this->networkManager = new RakNetworkManager(this);
}

GameLogic::~GameLogic() {
	delete networkManager;
	delete physicsManager;
	delete audioManager;
	delete renderer;
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
			// Todo: Replicate message over the network
		}
	}
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
}