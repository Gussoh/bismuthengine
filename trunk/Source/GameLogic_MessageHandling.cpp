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
	scores = new int[numberOfPlayers];
	for(int i = 0; i < numberOfPlayers; i++) {
		scores[i] = 0;
	}
}

void GameLogic::handleFireMessage(SharedPtr<Message> message) {
	GET_MSG(FireMessage, message);
	SharedPtr<Entity> shotEntity = handleCreateEntityMessage(msg->getCreateEntityMessage());
	GET_ENT(ShotEntity, shotEntity);
	ent->setWeapon(msg->getWeaponId());
	ent->setPlayerEntityId(msg->getFiringEntity());
	
	Ogre::Vector3 shotVector = ent->getOrientation() * -Ogre::Vector3::UNIT_Z;
	switch(msg->getWeaponId()) {
		case 2: // pistol
			physicsManager->addImpulse(shotEntity, shotVector.normalisedCopy() * 250);
			physicsManager->setForce(shotEntity, Ogre::Vector3(0, 0, 0));
			break;
		case 3: // machine gun
			physicsManager->addImpulse(shotEntity, shotVector.normalisedCopy() * 250);
			physicsManager->setForce(shotEntity, Ogre::Vector3(0, 0, 0));
			break;
		case 6: // grenades
			physicsManager->addImpulse(shotEntity, shotVector.normalisedCopy() * 20);
			break;
		case 7: // rockets
			physicsManager->addImpulse(shotEntity, shotVector.normalisedCopy() * 50);
			physicsManager->setForce(shotEntity, Ogre::Vector3(0, 0, 0));
			break;
	}
}

void GameLogic::handleShotHitPlayer(SharedPtr<Entity> player, SharedPtr<Entity> shot, float velocity) {
	if (player.getPointer() != playerEntity.getPointer()) {
		return;
	}

	GET_ENT(ShotEntity, shot);

	if (ent->getPlayerEntityId() == playerEntity->getId()) {
		return;
	}
	int healthToRemove = 0;
	switch(ent->getWeapon()) {
		case 2: // Pistol
			healthToRemove = 20;
			break;

		case 3: // Machine gun
			healthToRemove = 10;
			break;

		case 6: // Grenades
			//healthToRemove =(int) (velocity - shot->getPosition().distance(player->getPosition()));
			healthToRemove = velocity;
			break;

		case 7: // Rocket launcher
			//healthToRemove =(int) (velocity - shot->getPosition().distance(player->getPosition()));
			healthToRemove = velocity;
			break;
	}
	if	(healthToRemove > 0) {
		if (healthToRemove >= 60) {
			playerEntity->getAudioPropertiesPtr()->soundType = Audio::SoundType_MajorHurt;
		} else if(healthToRemove >= 30) {
			playerEntity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Hurt;
		} else {
			playerEntity->getAudioPropertiesPtr()->soundType = Audio::SoundType_MinorHurt;
		}
		audioManager->playSound(playerEntity);
		health -= healthToRemove;
	}
	
}

void GameLogic::handleDeathMessage(SharedPtr<Message> message) {
	GET_MSG(DeathMessage, message);
	SharedPtr<Entity> entity = getEntityById(msg->getPlayerEntityId());

	scores[msg->getPlayerNumber()]++;
	

	//physicsManager->removeUpVector(msg->getPlayerEntityId());
	physicsManager->addUpVector(msg->getPlayerEntityId(), Ogre::Vector3(0, 0, 0.5f));
	physicsManager->addImpulse(entity, Ogre::Vector3(0, 5.0f, 0));
	//entity->getSceneNode()->roll(Ogre::Radian(3.14f * 0.5f));

	entity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Destroy;
	audioManager->playSound(getEntityById(msg->getPlayerEntityId()));
}

void GameLogic::handleSpawnMessage(SharedPtr<Message> message) {
	GET_MSG(SpawnMessage, message);
	SharedPtr<Entity> entity = getEntityById(msg->getPlayerEntityId());
	SharedPtr<Entity> spawnEntity = getEntityById(msg->getSpawnEntityId());

	
	//physicsManager->removeUpVector(msg->getPlayerEntityId());
	physicsManager->addUpVector(msg->getPlayerEntityId(), Ogre::Vector3::UNIT_Y);
	entity->setPosition(spawnEntity->getPosition());
	//entity->setOrientation(spawnEntity->getOrientation());
	// Set orientation does not seem to restore the orientation properly. roll seems to be in separate varaible? 
	//entity->getSceneNode()->roll(Ogre::Radian(-3.14f * 0.5f));


	entity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Spawn;
	audioManager->playSound(entity);
}
