/**
 * @file GameLogic_MessageHandling.cpp
 */

#include "stdafx.h"
#include "GameLogic.h"
#include <OgreLight.h>
#include <OgreStringConverter.h>
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>

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
		case MsgMoveEntity:
			handleMoveEntityMessage(message);
			break;
		case MsgRotateEntity:
			handleRotateEntityMessage(message);
			break;
		case MsgPressButton:
			handlePressButtonMessage(message);
			break;
		case MsgCreateEntity:
			handleCreateEntityMessage(message);
			break;
		case MsgStartGame:
			handleStartGameMessage(message);
			break;
		case MsgIncomingConnection:
			handleIncomingConnectionMessage(message);
			break;
		case MsgPlayerIdAssigned:
			handlePlayerIdAssignedMessage(message);
			break;
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

	Ogre::Camera *camera = renderer->getDefaultCamera();
	camera->pitch(Ogre::Radian(-mousePosition.y * 0.005f));
	
	Ogre::Quaternion cameraOrientation = camera->getOrientation();
	
	// Limit the pitch between -90 degress and +90 degrees.
	if (Ogre::Math::ACos(cameraOrientation.w).valueRadians() > 3.1415f * 0.25f)
	{
		if (cameraOrientation.x > 0)
			// Set orientation to 90 degrees on X-axis.
			camera->setOrientation(Ogre::Quaternion(0.7071f, 0.7071f, 0, 0));
		else if (cameraOrientation.x < 0)
			// Sets orientation to -90 degrees on X-axis.
			camera->setOrientation(Ogre::Quaternion(0.7071f, -0.7071f, 0, 0));
	}

	if (!playerEntity.isNull()) {

		int id = playerEntity->getId();

		SharedPtr<RotateEntityMessage> rotateMsg = SharedPtr<RotateEntityMessage>(new RotateEntityMessage(id, Ogre::Radian(-mousePosition.x * 0.005f)));
		sendMessage(rotateMsg);

		if (inputManager->isKeyDown(Input::KC_W)) {
			SharedPtr<MoveEntityMessage> moveMsg = SharedPtr<MoveEntityMessage>(new MoveEntityMessage(id, Input::KC_W));
			sendMessage(moveMsg);
		}
		if (inputManager->isKeyDown(Input::KC_S)) {
			SharedPtr<MoveEntityMessage> moveMsg = SharedPtr<MoveEntityMessage>(new MoveEntityMessage(id, Input::KC_S));
			sendMessage(moveMsg);
		} 
		if (inputManager->isKeyDown(Input::KC_A)) {
			SharedPtr<MoveEntityMessage> moveMsg = SharedPtr<MoveEntityMessage>(new MoveEntityMessage(id, Input::KC_A));
			sendMessage(moveMsg);
		}
		if (inputManager->isKeyDown(Input::KC_D)) {
			SharedPtr<MoveEntityMessage> moveMsg = SharedPtr<MoveEntityMessage>(new MoveEntityMessage(id, Input::KC_D));
			sendMessage(moveMsg);
		} 
		if (inputManager->isKeyDown(Input::KC_SPACE)) {
			SharedPtr<MoveEntityMessage> moveMsg = SharedPtr<MoveEntityMessage>(new MoveEntityMessage(id, Input::KC_SPACE));
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

void GameLogic::handleMoveEntityMessage(SharedPtr<Message> message) {
	GET_MSG(MoveEntityMessage, message);
	SharedPtr<Entity> entity = getEntityById(msg->getEntityId());
	if (!entity.isNull()) {
		Ogre::Vector3 impulseVector;

		if (entity->hasContact()) {
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
					impulseVector = entity->getOrientation() * Ogre::Vector3::UNIT_Y * 5;
					break;
				default:
					return;
			}
		} else {
			switch(msg->getDirection()) {
				case Input::KC_W:
					impulseVector = entity->getOrientation() * -Ogre::Vector3::UNIT_Z * 0.05f;
					break;
				case Input::KC_S:
					impulseVector = entity->getOrientation() * Ogre::Vector3::UNIT_Z * 0.05f;
					break;
				case Input::KC_A:
					impulseVector = entity->getOrientation() * -Ogre::Vector3::UNIT_X * 0.05f;
					break;
				case Input::KC_D:
					impulseVector = entity->getOrientation() * Ogre::Vector3::UNIT_X * 0.05f;
					break;
				case Input::KC_SPACE:
					impulseVector = entity->getOrientation() * Ogre::Vector3::UNIT_Y * 0.0f;
					break;
				default:
					return;
			}
		}
		physicsManager->addImpulse(entity, impulseVector);
	}
}

void GameLogic::handleRotateEntityMessage(SharedPtr<Message> message) {
	GET_MSG(RotateEntityMessage, message);
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
	entity->getSceneNode()->setScale(msg->getScale(), msg->getScale(), msg->getScale());
	
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
	if (myPlayerId == -1) {
		myPlayerId = msg->getPlayerId();
	}
}

void GameLogic::handleStartGameMessage(SharedPtr<Message> message) {
	GET_MSG(StartGameMessage, message);
	numberOfPlayers = msg->getNumberOfPlayers();
	gameStarted = true;
}
