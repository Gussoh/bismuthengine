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
		case MsgDeath:
			handleDeathMessage(message);
			break;
		case MsgSpawn:
			handleSpawnMessage(message);
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

	if (!playerEntity.isNull() && !dead) {

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
		if (inputManager->isKeyDown(Input::KC_2) && weapon != 2) {
			weapon = 2;
			nextShotAllowed = frameCounter + 30;
		}
		if (inputManager->isKeyDown(Input::KC_3) && weapon != 3) {
			weapon = 3;
			nextShotAllowed = frameCounter + 60;
		}
		if (inputManager->isKeyDown(Input::KC_6) && weapon != 6) {
			weapon = 6;
			nextShotAllowed = frameCounter + 90;
		}
		if (inputManager->isKeyDown(Input::KC_7) && weapon != 7) {
			weapon = 7;
			nextShotAllowed = frameCounter + 100;
		}
		
		if (inputManager->isKeyDown(Input::KC_G)) {
			gDown = true;
		} else if (gDown) {
			gDown = false;
			gogglesEnabled = !gogglesEnabled;
			
			if (gogglesEnabled) {
				renderer->setCompositorEnabled("SeeThrough", true);
				renderer->getDefaultCamera()->getViewport()->setMaterialScheme("wall");
			} else {
				renderer->setCompositorEnabled("SeeThrough", false);
				renderer->getDefaultCamera()->getViewport()->setMaterialScheme("");
			}
		}

		if (inputManager->isMouseButtonDown(Input::MB_Left) && nextShotAllowed <= frameCounter) {

			Ogre::Quaternion shotOrientation = playerEntity->getOrientation() * camera->getOrientation();

			switch(weapon) {
				case 2: // pistol
					{
					SharedPtr<CreateEntityMessage> shotEntityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
					shotEntityMsg->setMeshName("Models/small_bullet.mesh");
					shotEntityMsg->setEntityType(ET_shot);

					Audio::AudioProperties audioProperties;
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Collision, "Audio/pistolhit1.wav"));
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Create, "Audio/pistol1.wav"));
					//audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Continuous, "Audio/flyingrocket1.wav"));
					shotEntityMsg->setAudioProperties(audioProperties);

					shotEntityMsg->setEntityMaterial(EMT_steel);
					shotEntityMsg->setOrientation(shotOrientation);
					shotEntityMsg->setPosition(playerEntity->getPosition() + (shotOrientation * -Ogre::Vector3::UNIT_Z) * 1.0f + Ogre::Vector3::UNIT_Y);
					shotEntityMsg->setScale(1.5f);
					SharedPtr<FireMessage> fireMsg = SharedPtr<FireMessage>(new FireMessage(2, playerEntity->getId(), shotEntityMsg));
					sendMessage(fireMsg);

					nextShotAllowed = frameCounter + 25;
					}
					break;
				case 3: // machine gun
					{
					SharedPtr<CreateEntityMessage> shotEntityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
					shotEntityMsg->setMeshName("Models/small_bullet_red.mesh");
					shotEntityMsg->setEntityType(ET_shot);

					Audio::AudioProperties audioProperties;
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Collision, "Audio/pistolhit1.wav"));
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Create, "Audio/machinegun1.wav"));
					//audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Continuous, "Audio/flyingrocket1.wav"));
					shotEntityMsg->setAudioProperties(audioProperties);

					shotEntityMsg->setEntityMaterial(EMT_steel);
					shotEntityMsg->setOrientation(shotOrientation);
					shotEntityMsg->setPosition(playerEntity->getPosition() + (shotOrientation * -Ogre::Vector3::UNIT_Z) * 1.0f + Ogre::Vector3::UNIT_Y);
					shotEntityMsg->setScale(0.8f);
					SharedPtr<FireMessage> fireMsg = SharedPtr<FireMessage>(new FireMessage(3, playerEntity->getId(), shotEntityMsg));
					sendMessage(fireMsg);

					nextShotAllowed = frameCounter + 10;
					}
					break;
				case 6: // grenades
					{
					SharedPtr<CreateEntityMessage> shotEntityMsg = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
					shotEntityMsg->setMeshName("Models/throw_grenade.mesh");
					shotEntityMsg->setEntityType(ET_shot);

					Audio::AudioProperties audioProperties;
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Collision, "Audio/bigboom.wav"));
					audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Create, "Audio/grenade1.wav"));
					//audioProperties.sounds.insert(std::make_pair(Audio::SoundType_Continuous, "Audio/flyingrocket1.wav"));
					shotEntityMsg->setAudioProperties(audioProperties);

					shotEntityMsg->setEntityMaterial(EMT_steel);
					shotEntityMsg->setOrientation(shotOrientation);
					shotEntityMsg->setPosition(playerEntity->getPosition() + (shotOrientation * -Ogre::Vector3::UNIT_Z) * 1.0f + Ogre::Vector3::UNIT_Y);
					
					SharedPtr<FireMessage> fireMsg = SharedPtr<FireMessage>(new FireMessage(6, playerEntity->getId(), shotEntityMsg));
					sendMessage(fireMsg);

					nextShotAllowed = frameCounter + 30;
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
					
					SharedPtr<FireMessage> fireMsg = SharedPtr<FireMessage>(new FireMessage(7, playerEntity->getId(), shotEntityMsg));
					sendMessage(fireMsg);

					nextShotAllowed = frameCounter + 40;
					}
					break;
				default:
					// no weapon selected
					break;

			}

			
		}
		if (health < 1) {
			dead = true;
			spawnOnFrame = frameCounter + 200;
			SharedPtr<DeathMessage> deathMessage = SharedPtr<DeathMessage>(new DeathMessage(playerEntity->getId(), myPlayerId));
			sendMessage(deathMessage);
		}

		
	}

	if (dead && spawnOnFrame < frameCounter) {
		std::vector<int> spawnAreas;
		for (EntityList::iterator entry = entities.begin(); entry != entities.end(); entry++) {
			SharedPtr<Entity> entity = entry->second;
			if(entity->getType() == ET_spawnarea) {
				spawnAreas.push_back(entry->first);
			}
		}

		int spawnEntityId = spawnAreas[std::rand() % spawnAreas.size()];
		SharedPtr<SpawnMessage> spawnMessage = SharedPtr<SpawnMessage>(new SpawnMessage(spawnEntityId, playerEntity->getId()));
		sendMessage(spawnMessage);
		dead = false;
		health = 100;
	}
}

void GameLogic::handleCollisionMessage(SharedPtr<Message> message) {
	GET_MSG(CollisionMessage, message);

	SharedPtr<Entity> entity1 = msg->getEntity1();
	SharedPtr<Entity> entity2 = msg->getEntity2();

	float velocity = msg->getVelocity();

	// player - shot collisions needs to be handled in a special way

	if (entity1->getType() == ET_shot && entity2->getType() == ET_player) {
		handleShotHitPlayer(entity2, entity1, velocity);
	} else if (entity1->getType() == ET_player && entity2->getType() == ET_shot) {
		handleShotHitPlayer(entity1, entity2, velocity);
	}

	handleCollision(entity1, velocity);
	handleCollision(entity2, velocity);
}

void GameLogic::handleCollision(SharedPtr<Entity> entity, float velocity) {
	switch(entity->getType()) {
		case ET_dynamic:
			entity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Collision;
			entity->getAudioPropertiesPtr()->collisionSpeed = velocity;
			audioManager->playSound(entity);
			break;
		case ET_shot:
			{
				GET_ENT(ShotEntity, entity);
				if(ent->hasExploded()) {
					break;
				}
				ent->setExploded(true);
				ent->getAudioPropertiesPtr()->soundType = Audio::SoundType_Collision;
				ent->getAudioPropertiesPtr()->collisionSpeed = velocity;
				audioManager->playSound(entity);
				if (ent->getWeapon() == 2) {
					
				} else if (ent->getWeapon() == 6) {
					physicsManager->explode(entity, 100);	
				} else if (ent->getWeapon() == 7) {
					physicsManager->explode(entity, 100);
				}

				if (ent->getWeapon() == 6 || ent->getWeapon() == 7) {
					Ogre::SceneNode *explosion = renderer->getDefaultSceneManager()->getRootSceneNode()->createChildSceneNode();
					Ogre::ParticleSystem *ps = renderer->getDefaultSceneManager()->createParticleSystem("Explosion" + Ogre::StringConverter::toString(entity->getId()), "Explosion");
					explosion->attachObject(ps);
					addTimedAnimation(300, explosion);

					explosion->setPosition(entity->getSceneNode()->getPosition());
				}
				
				removeEntity(entity);
			}
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
