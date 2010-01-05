/**
 * @file GameLogic_MessageHandling.cpp
 */

#include "stdafx.h"
#include "GameLogic.h"
#include "ShotEntity.h"
#include <OgreLight.h>
#include <OgreStringConverter.h>
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreParticleSystem.h>
#include <OgreMaterialManager.h>
#include <cstdlib>

using namespace Bismuth;

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
	frameCounter++;

	GET_MSG(EndOfFrameMessage, message);

	physicsManager->update(msg->getStepTime());
	inputManager->update();
	audioManager->update();

	while (!localMessageQueue.empty()) {
		handleMessage(localMessageQueue.front());
		localMessageQueue.pop();
	}

	TimedAnimationList::iterator iter = timedAnimations.begin();
	while (iter != timedAnimations.end()) {
		SharedPtr<TimedAnimation> anim = *iter;
		anim->time--;

		if (anim->time <= 0) {
			renderer->getDefaultSceneManager()->destroySceneNode(anim->sceneNode);
			iter = timedAnimations.erase(iter);
		}

		if (iter != timedAnimations.end()) {
			++iter;
		}
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
		getPhysicsManager()->addImpulse(entity, impulseVector);
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


SharedPtr<Entity> GameLogic::handleCreateEntityMessage(SharedPtr<Message> message) {
	GET_MSG(CreateEntityMessage, message);

	SharedPtr<Entity> entity;
	if (msg->getMeshName().empty()) {
		entity = createEntity(msg->getEntityType());
	} else {
		entity = createEntity(msg->getEntityType(), msg->getMeshName());
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
		light->setDiffuseColour(Ogre::ColourValue(0.98f, 0.98f, 0.92f, 1.0f));
		Ogre::Vector3 dir(-0.15f, -0.95f, 0.0f);
		dir.normalise();
		light->setDirection(dir);
		entity->getSceneNode()->attachObject(light);
	}

	entity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Continuous;
	audioManager->playSound(entity);

	entity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Create;
	audioManager->playSound(entity);

	return entity;
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

void GameLogic::handleCollisionMessage(SharedPtr<Message> message) {
	GET_MSG(CollisionMessage, message);

	SharedPtr<Entity> entity1 = msg->getEntity1();
	SharedPtr<Entity> entity2 = msg->getEntity2();

	float velocity = msg->getVelocity();
	
	entity1->getAudioPropertiesPtr()->soundType = Audio::SoundType_Collision;
	entity1->getAudioPropertiesPtr()->collisionSpeed = velocity;
	getAudioManager()->playSound(entity1);

	entity2->getAudioPropertiesPtr()->soundType = Audio::SoundType_Collision;
	entity2->getAudioPropertiesPtr()->collisionSpeed = velocity;
	getAudioManager()->playSound(entity2);

}


