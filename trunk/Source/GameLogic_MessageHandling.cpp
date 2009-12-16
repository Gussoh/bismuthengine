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
		case MsgFire:
			handleFireMessage(message);
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
	frameCounter++;

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
		if (cameraOrientation.x > 0) {
			// Set orientation to 90 degrees on X-axis.
			camera->setOrientation(Ogre::Quaternion(0.7071f, 0.7071f, 0, 0));
		}
		else if (cameraOrientation.x < 0) {
			// Sets orientation to -90 degrees on X-axis.
			camera->setOrientation(Ogre::Quaternion(0.7071f, -0.7071f, 0, 0));
		}
	}


	while (!specialMessageQueue.empty()) {
		handleMessage(specialMessageQueue.front());
		specialMessageQueue.pop();
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
		if (inputManager->isKeyDown(Input::KC_2)) {
			weapon = 2;
			nextShotAllowed = frameCounter + 30;
		}
		if (inputManager->isKeyDown(Input::KC_6)) {
			weapon = 6;
			nextShotAllowed = frameCounter + 100;
		}
		if (inputManager->isKeyDown(Input::KC_7)) {
			weapon = 7;
			nextShotAllowed = frameCounter + 100;
		}
		if (inputManager->isMouseButtonDown(Input::MB_Left) && nextShotAllowed <= frameCounter) {

			Ogre::Quaternion shotOrientation = playerEntity->getOrientation() * camera->getOrientation();

			switch(weapon) {
				case 2: // pistol
					{
					SharedPtr<CreateEntityMessage> shotEntityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
					shotEntityMsg->setMeshName("Models/grenade.mesh");
					shotEntityMsg->setEntityType(ET_shot);

					Audio::AudioProperties audioProperties;
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Collision, "Audio/bigboom.wav"));
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Create, "Audio/rocket1.wav"));
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Continuous, "Audio/flyingrocket1.wav"));
					shotEntityMsg->setAudioProperties(audioProperties);

					shotEntityMsg->setEntityMaterial(EMT_steel);
					shotEntityMsg->setOrientation(shotOrientation);
					shotEntityMsg->setPosition(playerEntity->getPosition() + (shotOrientation * -Ogre::Vector3::UNIT_Z) * 1.0f + Ogre::Vector3::UNIT_Y);
					//shotEntityMsg->setScale(0.5f);
					SharedPtr<FireMessage> fireMsg = SharedPtr<FireMessage>(new FireMessage(6, shotEntityMsg));
					sendMessage(fireMsg);

					nextShotAllowed = frameCounter + 20;
					}
					break;
				case 6: // grenades
					{
					SharedPtr<CreateEntityMessage> shotEntityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
					shotEntityMsg->setMeshName("Models/grenade.mesh");
					shotEntityMsg->setEntityType(ET_shot);

					Audio::AudioProperties audioProperties;
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Collision, "Audio/bigboom.wav"));
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Create, "Audio/rocket1.wav"));
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Continuous, "Audio/flyingrocket1.wav"));
					shotEntityMsg->setAudioProperties(audioProperties);

					shotEntityMsg->setEntityMaterial(EMT_steel);
					shotEntityMsg->setOrientation(shotOrientation);
					shotEntityMsg->setPosition(playerEntity->getPosition() + (shotOrientation * -Ogre::Vector3::UNIT_Z) * 1.0f + Ogre::Vector3::UNIT_Y);
					//shotEntityMsg->setScale(0.5f);
					SharedPtr<FireMessage> fireMsg = SharedPtr<FireMessage>(new FireMessage(6, shotEntityMsg));
					sendMessage(fireMsg);

					nextShotAllowed = frameCounter + 20;
					}
					break;
				case 7: // rockets
					{
					SharedPtr<CreateEntityMessage> shotEntityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
					shotEntityMsg->setMeshName("Models/grenade.mesh");
					shotEntityMsg->setEntityType(ET_shot);

					Audio::AudioProperties audioProperties;
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Collision, "Audio/bigboom.wav"));
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Create, "Audio/rocket1.wav"));
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Continuous, "Audio/flyingrocket1.wav"));
					shotEntityMsg->setAudioProperties(audioProperties);

					shotEntityMsg->setEntityMaterial(EMT_steel);
					shotEntityMsg->setOrientation(shotOrientation);
					shotEntityMsg->setPosition(playerEntity->getPosition() + (shotOrientation * -Ogre::Vector3::UNIT_Z) * 1.0f + Ogre::Vector3::UNIT_Y);
					//shotEntityMsg->setScale(0.5f);
					SharedPtr<FireMessage> fireMsg = SharedPtr<FireMessage>(new FireMessage(7, shotEntityMsg));
					sendMessage(fireMsg);

					nextShotAllowed = frameCounter + 20;
					}
					break;
				default:
					// no weapon selected
					break;

			}

			
		}
	}

}

void GameLogic::handleCollisionMessage(SharedPtr<Message> message) {
	GET_MSG(CollisionMessage, message);

	SharedPtr<Entity> entity1 = getEntityById(msg->getEntityId1());
	SharedPtr<Entity> entity2 = getEntityById(msg->getEntityId2());

	float velocity = msg->getVelocity();

	if (!entity1.isNull()) {
		handleCollision(entity1, velocity);
	} 
	if (!entity2.isNull()) {
		handleCollision(entity2, velocity);
	}
}

void GameLogic::handleCollision(SharedPtr<Entity> entity, float velocity) {
	switch(entity->getType()) {
		case ET_dynamic:
			entity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Collision;
			entity->getAudioPropertiesPtr()->collisionSpeed = velocity;
			audioManager->playSound(entity);
			break;
		case ET_shot:
			entity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Collision;
			entity->getAudioPropertiesPtr()->collisionSpeed = velocity;
			audioManager->playSound(entity);
			physicsManager->explode(entity, 50);
			removeEntity(entity);
			break;
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

SharedPtr<Entity> GameLogic::handleCreateEntityMessage(SharedPtr<Message> message) {
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
		light->setDiffuseColour(Ogre::ColourValue(0.98f, 0.98f, 0.92f, 1.0f));
		Ogre::Vector3 dir(-0.15f, -0.95f, 0.0f);
		dir.normalise();
		light->setDirection(dir);
		entity->getSceneNode()->attachObject(light);
	}

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

void GameLogic::handleFireMessage(SharedPtr<Message> message) {
	GET_MSG(FireMessage, message);
	SharedPtr<Entity> shotEntity = handleCreateEntityMessage(msg->getCreateEntityMessage());
	Ogre::Vector3 shotVector = shotEntity->getOrientation() * -Ogre::Vector3::UNIT_Z;
	switch(msg->getWeaponId()) {
		case 2: // pistol
			physicsManager->addImpulse(shotEntity, shotVector.normalisedCopy() * 20);
			shotEntity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Create;
			audioManager->playSound(shotEntity);
			shotEntity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Continuous;
			audioManager->playSound(shotEntity);
			break;
		case 6: // grenades
			physicsManager->addImpulse(shotEntity, shotVector.normalisedCopy() * 20);
			shotEntity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Create;
			audioManager->playSound(shotEntity);
			shotEntity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Continuous;
			audioManager->playSound(shotEntity);
			break;
		case 7: // rockets
			physicsManager->addImpulse(shotEntity, shotVector.normalisedCopy() * 20);
			shotEntity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Create;
			audioManager->playSound(shotEntity);
			shotEntity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Continuous;
			audioManager->playSound(shotEntity);
			break;
	}
}
