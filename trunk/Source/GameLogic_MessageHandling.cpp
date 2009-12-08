/**
 * @file GameLogic_MessageHandling.cpp
 */

#include "stdafx.h"
#include "GameLogic.h"
#include "FmodAudioManager.h"
#include "OgreNewtPhysicsManager.h"
#include "RakNetworkManager.h"
#include "OISInputManager.h"
#include <OgreLight.h>
#include <OgreStringConverter.h>

using namespace Bismuth;


void GameLogic::handleMessage(SharedPtr<Message> message) {
	switch (message->getType()) {
		case MsgDebugOut:
			handleDebugOutMessage(message);
			break;
		case MsgEntityAssigned:
			handleEntityAssignedMessage(message);
			break;
		case MsgEndOfFrame:
			handleEndOfFrameMessage(message);
			break;
		case MsgCollision:
			handleCollisionMessage(message);
			break;
		case MsgPlayerMove:
			handlePlayerMoveMessage(message);
			break;
		case MsgPlayerRotate:
			handlePlayerRotateMessage(message);
			break;
		case MsgPressButton:
			handlePressButtonMessage(message);
			break;
		case MsgCreateEntity:
			handleCreateEntityMessage(message);
			break;
	/*	case MsgStartGame:
			handleStartGameMessage(message);
			break;
		case MsgIncomingConnection:
			handleIncomingConnectionMessage(message);
			break;
		case MsgPlayerIdAssigned:
			handlePlayerIdAssignedMessage(message);
			break;*/
		default:
			std::cout << "**** Faulty MessageType: " << message->getType() << std::endl;
			break;
	}
}

void GameLogic::handleDebugOutMessage(SharedPtr<Message> message) {
	GET_MSG(DebugOutMessage, message);
	std::cout << msg->getText();
}

void GameLogic::handleEntityAssignedMessage(SharedPtr<Message> message) {
	GET_MSG(EntityAssignedMessage, message);

	if(msg->getPlayerId() == myPlayerId) {
		setPlayerEntity(getEntityById(msg->getEntityId()));
		setCameraEntity(getPlayerEntity());
		std::cout << "Player entity assigned: " << msg->getEntityId() << std::endl;
	}
}

void GameLogic::handleEndOfFrameMessage(SharedPtr<Message> message) {
	GET_MSG(EndOfFrameMessage, message);

	physicsManager->update(msg->getStepTime());
	inputManager->update();
	audioManager->update();

	Ogre::Vector3 mousePosition = inputManager->getRelativeMousePosition();
	SharedPtr<Entity> playerEntity = getPlayerEntity();
	if (!playerEntity.isNull()) {
		
		renderer->getDefaultCamera()->pitch(Ogre::Radian(-mousePosition.y * 0.005f));

		SharedPtr<PlayerRotateMessage> rotateMsg = SharedPtr<PlayerRotateMessage>(new PlayerRotateMessage(this, Ogre::Radian(-mousePosition.x * 0.005f)));
		sendMessage(rotateMsg);
		//playerEntity->getSceneNode()->yaw(Ogre::Radian(-mousePosition.x * 0.005f));
		playerEntity->setPositionOrientationChanged(true);

		if (inputManager->isKeyDown(Input::KC_W)) {
			SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(this, Input::KC_W));
			sendMessage(moveMsg);
		}
		if (inputManager->isKeyDown(Input::KC_S)) {
			SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(this, Input::KC_S));
			sendMessage(moveMsg);
		} 
		if (inputManager->isKeyDown(Input::KC_A)) {
			SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(this, Input::KC_A));
			sendMessage(moveMsg);
		}
		if (inputManager->isKeyDown(Input::KC_D)) {
			SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(this, Input::KC_D));
			sendMessage(moveMsg);
		} 
		if (inputManager->isKeyDown(Input::KC_SPACE)) {
			SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(this, Input::KC_SPACE));
			sendMessage(moveMsg);
		}
	}

}

void GameLogic::handleCollisionMessage(SharedPtr<Message> message) {
	GET_MSG(CollisionMessage, message);

	SharedPtr<Entity> entity1 = getEntityById(msg->getEntityId1());
	SharedPtr<Entity> entity2 = getEntityById(msg->getEntityId2());

	if ((entity1->getType() == ET_player && entity2->getType() == ET_dynamic)) {
		entity2->getAudioPropertiesPtr()->soundType = Audio::SoundType_Collision;
		audioManager->playSound(entity2);
	}
}

void GameLogic::handlePlayerMoveMessage(SharedPtr<Message> message) {
	GET_MSG(PlayerMoveMessage, message);
	SharedPtr<Entity> entity = getEntityById(msg->getEntityId());
	if (!entity.isNull()) {
		Ogre::Vector3 impulseVector;
		switch(msg->getDirection()) {
			case Input::KC_W:
				impulseVector = entity->getOrientation() * -Ogre::Vector3::UNIT_Z;
				break;
			case Input::KC_S:
				impulseVector = entity->getOrientation() * Ogre::Vector3::UNIT_Z;
				break;
			case Input::KC_A:
				impulseVector = entity->getOrientation() * -Ogre::Vector3::UNIT_X;
				break;
			case Input::KC_D:
				impulseVector = entity->getOrientation() * Ogre::Vector3::UNIT_X;
				break;
			case Input::KC_SPACE:
				impulseVector = entity->getOrientation() * Ogre::Vector3::UNIT_Y;
				break;
			default:
				return;
		}
		physicsManager->addImpulse(entity, impulseVector * 0.2f);
	}
}

void GameLogic::handlePlayerRotateMessage(SharedPtr<Message> message) {
	GET_MSG(PlayerRotateMessage, message);
	SharedPtr<Entity> entity = getEntityById(msg->getEntityId());
	if (!entity.isNull()) {
		entity->getSceneNode()->yaw(msg->getRotation());
		entity->setPositionOrientationChanged(true);
	}
}

void GameLogic::handlePressButtonMessage(SharedPtr<Message> message) {
	GET_MSG(PressButtonMessage, message);
	// TODO: implement!
}

void GameLogic::handleCreateEntityMessage(SharedPtr<Message> message) {
	GET_MSG(CreateEntityMessage, message);

	SharedPtr<Entity> entity;
	if (msg->getMeshName().empty()) {
		entity = createEntity();
	} else {
		entity = createEntity(msg->getMeshName());
	}

	entity->setPosition(msg->getPosition());
	entity->setOrientation(msg->getOrientation());
	entity->setType(msg->getEntityType());
	entity->setMaterial(msg->getEntityMaterial());
	entity->setAudioProperties(msg->getAudioProperties());

	if (entity->getType() == ET_light) {
		Ogre::Light *light = renderer->getDefaultSceneManager()->createLight("light" + Ogre::StringConverter::toString(entity->getId()));
		light->setType(Ogre::Light::LT_DIRECTIONAL);
		light->setDirection(0.2f, -0.9f, 0.2f);
		entity->getSceneNode()->attachObject(light);
	}
}

void GameLogic::handleIncomingConnectionMessage(SharedPtr<Message> message) {
	networkManager->sendMessage(SharedPtr<Message>(new PlayerIdAssignedMessage(playerIdCounter)));
	playerIdCounter++;
}

void GameLogic::handlePlayerIdAssignedMessage(SharedPtr<Message> message) {
	GET_MSG(PlayerIdAssignedMessage, message);
	if (myPlayerId == 0) {
		myPlayerId = msg->getPlayerId();
	}
}

void GameLogic::handleStartGameMessage(SharedPtr<Message> message) {
	GET_MSG(StartGameMessage, message);
	numberOfPlayers = msg->getNumberOfPlayers();
	gameStarted = true;
}
