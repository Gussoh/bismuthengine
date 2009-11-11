/**
 * @file GameLogic.cpp
 */

#include "stdafx.h"

#include "Entity.h"
#include "GameLogic.h"
#include "FmodAudioManager.h"
#include "NewtonPhysicsManager.h"

using namespace Bismuth;
using namespace Bismuth::Audio;
using namespace Bismuth::Physics;

GameLogic::GameLogic() {
	SharedPtr<GameLogic> g = SharedPtr<GameLogic>(this);
	this->audioManager = SharedPtr<AudioManager>(new FmodAudioManager(g));
	this->physicsManager = SharedPtr<PhysicsManager>(new NewtonPhysicsManager(g));
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
	}
}

void GameLogic::sendMessage(SharedPtr<Message> message) {
	messageQueue.push(message);
}

void GameLogic::handleMessage(SharedPtr<Message> message) {
	if (message->getType() == MsgDebugOut) {
		DebugOutMessage* msg = dynamic_cast<DebugOutMessage*>(message.getPointer());
		if (msg != 0) {
			std::cout << msg->getText();
		}
	}
}