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
	// Todo: Use raw pointer to game logic for sub systems or things could go really bad, ie GameLogic pointer is deleted twice
	// First when all sub systems go out of scope and a second time when game logic goes out of scope
	// Things become event worse when GameLogic is not dynamicly allocated as in AudioTest, delete on stack memory is baaaad....
	SharedPtr<GameLogic> g = SharedPtr<GameLogic>(this);

	this->audioManager = SharedPtr<AudioManager>(new FmodAudioManager(g));
	this->physicsManager = SharedPtr<PhysicsManager>(new NewtonPhysicsManager(g));
	this->networkManager = SharedPtr<NetworkManager>(new RakNetworkManager(g));
}

GameLogic::~GameLogic() {

}

SharedPtr<Entity> GameLogic::getEntityById(int id) {
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