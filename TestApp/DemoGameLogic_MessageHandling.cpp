/**
 * @file DemoGameLogic_MessageHandling.cpp
 */

#include "stdafx.h"
#include "Renderer.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "ShotEntity.h"
#include "Entity.h"
#include "DemoGameLogic.h"
#include <OgreLight.h>
#include <OgreStringConverter.h>
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreSceneManager.h>
#include <OgreParticleSystem.h>
#include <OgreMaterialManager.h>
#include <cstdlib>


using namespace Bismuth;

void DemoGameLogic::handleMessage(SharedPtr<Message> message) {
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
			//std::cout << "**** Faulty MessageType: " << message->getType() << std::endl;
			break;
	}
}

void DemoGameLogic::handleEndOfFrameMessage(SharedPtr<Message> message) {
	GameLogic::handleEndOfFrameMessage(message);
	
	Ogre::Vector3 mousePosition = getInputManager()->getRelativeMousePosition();
	SharedPtr<Entity> playerEntity = getPlayerEntity();

	Ogre::Camera *camera = getRenderer()->getDefaultCamera();
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

	
	if (!playerEntity.isNull() && !dead) {

		int id = playerEntity->getId();

		SharedPtr<RotateEntityMessage> rotateMsg = SharedPtr<RotateEntityMessage>(new RotateEntityMessage(id, Ogre::Radian(-mousePosition.x * 0.005f)));
		sendMessage(rotateMsg);

		if (getInputManager()->isKeyDown(Input::KC_W)) {
			SharedPtr<MoveEntityMessage> moveMsg = SharedPtr<MoveEntityMessage>(new MoveEntityMessage(id, Input::KC_W));
			sendMessage(moveMsg);
		}
		if (getInputManager()->isKeyDown(Input::KC_S)) {
			SharedPtr<MoveEntityMessage> moveMsg = SharedPtr<MoveEntityMessage>(new MoveEntityMessage(id, Input::KC_S));
			sendMessage(moveMsg);
		} 
		if (getInputManager()->isKeyDown(Input::KC_A)) {
			SharedPtr<MoveEntityMessage> moveMsg = SharedPtr<MoveEntityMessage>(new MoveEntityMessage(id, Input::KC_A));
			sendMessage(moveMsg);
		}
		if (getInputManager()->isKeyDown(Input::KC_D)) {
			SharedPtr<MoveEntityMessage> moveMsg = SharedPtr<MoveEntityMessage>(new MoveEntityMessage(id, Input::KC_D));
			sendMessage(moveMsg);
		} 
		if (getInputManager()->isKeyDown(Input::KC_SPACE)) {
			SharedPtr<MoveEntityMessage> moveMsg = SharedPtr<MoveEntityMessage>(new MoveEntityMessage(id, Input::KC_SPACE));
			sendMessage(moveMsg);
		}
		if (getInputManager()->isKeyDown(Input::KC_2) && weapon != 2) {
			weapon = 2;
			nextShotAllowed = getFrameCounter() + 30;
		}
		if (getInputManager()->isKeyDown(Input::KC_3) && weapon != 3) {
			weapon = 3;
			nextShotAllowed = getFrameCounter() + 60;
		}
		if (getInputManager()->isKeyDown(Input::KC_6) && weapon != 6) {
			weapon = 6;
			nextShotAllowed = getFrameCounter() + 90;
		}
		if (getInputManager()->isKeyDown(Input::KC_7) && weapon != 7) {
			weapon = 7;
			nextShotAllowed = getFrameCounter() + 100;
		}
		
		if (getInputManager()->isKeyDown(Input::KC_G)) {
			gDown = true;
		} else if (gDown) {
			gDown = false;
			gogglesEnabled = !gogglesEnabled;
			
			if (gogglesEnabled) {
				getRenderer()->setCompositorEnabled("SeeThrough", true);
				getRenderer()->getDefaultCamera()->getViewport()->setMaterialScheme("wall");
			} else {
				getRenderer()->setCompositorEnabled("SeeThrough", false);
				getRenderer()->getDefaultCamera()->getViewport()->setMaterialScheme("");
			}
		}

		if (getInputManager()->isMouseButtonDown(Input::MB_Left) && nextShotAllowed <= getFrameCounter()) {

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

					nextShotAllowed = getFrameCounter() + 25;
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

					nextShotAllowed = getFrameCounter() + 10;
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

					nextShotAllowed = getFrameCounter() + 30;
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

					nextShotAllowed = getFrameCounter() + 40;
					}
					break;
				default:
					// no weapon selected
					break;

			}			
		}
		if (health < 1) {
			dead = true;
			spawnOnFrame = getFrameCounter() + 200;
			SharedPtr<DeathMessage> deathMessage = SharedPtr<DeathMessage>(new DeathMessage(playerEntity->getId(), getMyPlayerId()));
			sendMessage(deathMessage);
		}
	}

	if (dead && spawnOnFrame < getFrameCounter()) {
		std::vector<int> spawnAreas;
		for (EntityList::iterator entry = getEntities()->begin(); entry != getEntities()->end(); entry++) {
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


void DemoGameLogic::handleCollisionMessage(SharedPtr<Message> message) {
	GameLogic::handleCollisionMessage(message);
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

void DemoGameLogic::handleCollision(SharedPtr<Entity> entity, float velocity) {
	switch(entity->getType()) {
		case ET_shot:
			{
				GET_ENT(ShotEntity, entity);
				if(ent->hasExploded()) {
					break;
				}
				ent->setExploded(true);

				if (ent->getWeapon() == 2) {
					
				} else if (ent->getWeapon() == 6) {
					getPhysicsManager()->explode(entity, 100);	
				} else if (ent->getWeapon() == 7) {
					getPhysicsManager()->explode(entity, 100);
				}

				if (ent->getWeapon() == 6 || ent->getWeapon() == 7) {
					Ogre::SceneNode *explosion = getRenderer()->getDefaultSceneManager()->getRootSceneNode()->createChildSceneNode();
					Ogre::ParticleSystem *ps = getRenderer()->getDefaultSceneManager()->createParticleSystem("Explosion" + Ogre::StringConverter::toString(entity->getId()), "Explosion");
					explosion->attachObject(ps);
					addTimedAnimation(300, explosion);

					explosion->setPosition(entity->getSceneNode()->getPosition());
				}
				
				removeEntity(entity);
			}
			break;
	}
}

void DemoGameLogic::handleStartGameMessage(SharedPtr<Message> message) {
	GameLogic::handleStartGameMessage(message);
	scores = new int[getNumberOfPlayers()];
	for(int i = 0; i < getNumberOfPlayers(); i++) {
		scores[i] = 0;
	}
}


void DemoGameLogic::handleFireMessage(SharedPtr<Message> message) {
	GET_MSG(FireMessage, message);
	SharedPtr<Entity> shotEntity = handleCreateEntityMessage(msg->getCreateEntityMessage());
	GET_ENT(ShotEntity, shotEntity);
	ent->setWeapon(msg->getWeaponId());
	ent->setPlayerEntityId(msg->getFiringEntity());
	
	Ogre::Vector3 shotVector = ent->getOrientation() * -Ogre::Vector3::UNIT_Z;
	switch(msg->getWeaponId()) {
		case 2: // pistol
			getPhysicsManager()->addImpulse(shotEntity, shotVector.normalisedCopy() * 250);
			getPhysicsManager()->setForce(shotEntity, Ogre::Vector3(0, 0, 0));
			break;
		case 3: // machine gun
			getPhysicsManager()->addImpulse(shotEntity, shotVector.normalisedCopy() * 250);
			getPhysicsManager()->setForce(shotEntity, Ogre::Vector3(0, 0, 0));
			break;
		case 6: // grenades
			getPhysicsManager()->addImpulse(shotEntity, shotVector.normalisedCopy() * 20);
			break;
		case 7: // rockets
			getPhysicsManager()->addImpulse(shotEntity, shotVector.normalisedCopy() * 50);
			getPhysicsManager()->setForce(shotEntity, Ogre::Vector3(0, 0, 0));
			break;
	}
}


void DemoGameLogic::handleShotHitPlayer(SharedPtr<Entity> player, SharedPtr<Entity> shot, float velocity) {
	if (player.getPointer() != getPlayerEntity().getPointer()) {
		return;
	}

	GET_ENT(ShotEntity, shot);

	if (ent->getPlayerEntityId() == getPlayerEntity()->getId()) {
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
			getPlayerEntity()->getAudioPropertiesPtr()->soundType = Audio::SoundType_MajorHurt;
		} else if(healthToRemove >= 30) {
			getPlayerEntity()->getAudioPropertiesPtr()->soundType = Audio::SoundType_Hurt;
		} else {
			getPlayerEntity()->getAudioPropertiesPtr()->soundType = Audio::SoundType_MinorHurt;
		}
		getAudioManager()->playSound(getPlayerEntity());
		health -= healthToRemove;
	}
	
}


void DemoGameLogic::handleDeathMessage(SharedPtr<Message> message) {
	GET_MSG(DeathMessage, message);
	SharedPtr<Entity> entity = getEntityById(msg->getPlayerEntityId());

	scores[msg->getPlayerNumber()]++;
	

	//physicsManager->removeUpVector(msg->getPlayerEntityId());
	getPhysicsManager()->addUpVector(msg->getPlayerEntityId(), Ogre::Vector3(0, 0, 0.5f));
	getPhysicsManager()->addImpulse(entity, Ogre::Vector3(0, 5.0f, 0));
	//entity->getSceneNode()->roll(Ogre::Radian(3.14f * 0.5f));

	entity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Destroy;
	getAudioManager()->playSound(getEntityById(msg->getPlayerEntityId()));
}

void DemoGameLogic::handleSpawnMessage(SharedPtr<Message> message) {
	GET_MSG(SpawnMessage, message);
	SharedPtr<Entity> entity = getEntityById(msg->getPlayerEntityId());
	SharedPtr<Entity> spawnEntity = getEntityById(msg->getSpawnEntityId());

	
	//physicsManager->removeUpVector(msg->getPlayerEntityId());
	getPhysicsManager()->addUpVector(msg->getPlayerEntityId(), Ogre::Vector3::UNIT_Y);
	entity->setPosition(spawnEntity->getPosition());
	//entity->setOrientation(spawnEntity->getOrientation());
	// Set orientation does not seem to restore the orientation properly. roll seems to be in separate varaible? 
	//entity->getSceneNode()->roll(Ogre::Radian(-3.14f * 0.5f));


	entity->getAudioPropertiesPtr()->soundType = Audio::SoundType_Spawn;
	getAudioManager()->playSound(entity);
}
