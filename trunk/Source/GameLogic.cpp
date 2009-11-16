/**
 * @file GameLogic.cpp
 */

#include "stdafx.h"

#include "Entity.h"
#include "GameLogic.h"
#include "FmodAudioManager.h"
#include "NewtonPhysicsManager.h"
#include "RakNetworkManager.h"

using namespace Bismuth;
using namespace Bismuth::Audio;
using namespace Bismuth::Physics;
using namespace Bismuth::Network;

GameLogic::GameLogic(bool isServer) : isServer(isServer) {
	this->audioManager = SharedPtr<AudioManager>(new FmodAudioManager(this));
	this->physicsManager = SharedPtr<PhysicsManager>(new NewtonPhysicsManager(this));
	this->networkManager = SharedPtr<NetworkManager>(new RakNetworkManager(this));
}

GameLogic::~GameLogic() {

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