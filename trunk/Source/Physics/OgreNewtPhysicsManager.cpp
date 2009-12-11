/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "OgreNewtPhysicsManager.h"
#include "OgreNewt.h"
#include "Renderer.h"
#include <limits>


#define MAX_MOVEMENT_SPEED 10
#define GRAVITY -9.82f

using namespace Bismuth;
using namespace Bismuth::Physics;
using namespace Bismuth::Graphics;
using namespace std;
using namespace OgreNewt;
using namespace OgreNewt::CollisionPrimitives;

OgreNewtPhysicsManager::OgreNewtPhysicsManager(GameLogic *gameLogic) {
	this->gameLogic = gameLogic;
	this->world = new World();

	// 0 is the only mode to use if we want deterministic physics
	this->world->setPlatformArchitecture(0);
	world->setSolverModel(World::SM_ADAPTIVE);
	world->setFrictionModel(World::FM_ADAPTIVE);
	
	Renderer *renderer = gameLogic->getRenderer();

	defaultMaterialPair = new OgreNewt::MaterialPair(world, world->getDefaultMaterialID(), world->getDefaultMaterialID());
	defaultMaterialPair->setContactCallback(this);
}

OgreNewtPhysicsManager::~OgreNewtPhysicsManager() {

	for(UpVectorList::iterator iter = upVectors.begin(); iter != upVectors.end(); iter++) {
		delete (*iter);
	}
	upVectors.clear();

	removeAllEntities();
	
	delete world;
	OgreNewt::Debugger::getSingleton().deInit();
}

void OgreNewtPhysicsManager::getNearbyEntities(float radius, SharedPtr<Entity> source, vector<SharedPtr<Entity> > &entityList) {
	
}

Body* OgreNewtPhysicsManager::createBodyForEntity(SharedPtr<Entity> &entity) {
	
	Body *body;

	switch(entity->getType()) {
		case ET_static:
		case ET_button:
			body = createStaticBody(entity);
			break;
		case ET_dynamic:
			body = createDynamicBody(entity);
			break;
		case ET_player:
			body = createPlayerBody(entity);
			break;
		default:
			//throw exception("EntityType not implemented in OgreNewtPhysicsManager.");
			return 0;

	}

	body->setUserData(entity.get());
	
	idToBodyMap.insert(pair<int, OgreNewt::Body*>(entity->getId(), body));

	return body;
}

void OgreNewtPhysicsManager::removeEntity(SharedPtr<Entity> &entity) {
	IdToBodyMap::iterator iter = idToBodyMap.find(entity->getId());
	if (iter != idToBodyMap.end()) {
		delete iter->second;
		idToBodyMap.erase(iter);
	}
}

void OgreNewtPhysicsManager::removeAllEntities() {
	for (IdToBodyMap::iterator iter = idToBodyMap.begin(); iter != idToBodyMap.end(); iter++) {
		delete iter->second;
	}

	idToBodyMap.clear();
}

void OgreNewtPhysicsManager::update(float stepTime) {
	EntityList *entities = gameLogic->getEntities();
	for (EntityList::iterator iter = entities->begin(); iter != entities->end(); iter++) {
		SharedPtr<Entity> entity = iter->second;
		IdToBodyMap::iterator idBodyPair = idToBodyMap.find(entity->getId());

		Body *body;

		// If body does not exist for an entity, create a body for it.
		if (idBodyPair == idToBodyMap.end()) {
			body = createBodyForEntity(entity);
			entity->setPositionOrientationChanged(false);
		} else {
			body = idBodyPair->second;
		}
	
		if (body != 0 && entity->hasPositionOrientationChanged()) {
			body->setPositionOrientation(entity->getPosition(), entity->getOrientation());
			entity->setPositionOrientationChanged(false);
		}
	}

	collisionHashSet.clear();
	
	world->update(stepTime);

	idToImpulseMap.clear();
}

void OgreNewtPhysicsManager::addImpulse(SharedPtr<Entity> &entity, Ogre::Vector3 &direction) {
	IdToImpulseMap::iterator element = idToImpulseMap.find(entity->getId());
	if(element == idToImpulseMap.end()) {
		idToImpulseMap.insert(std::pair<int, Ogre::Vector3>(entity->getId(), direction)); 
	} else {
		element->second += direction;
	}
}


Body* OgreNewtPhysicsManager::createDynamicBody(SharedPtr<Entity> &entity) {
	if (entity->getSceneNode() == 0) {
		throw exception("createDynamicBody failed because entity did not have a scenenode.");
	}

	Collision *collision = new ConvexHull(world, entity->getSceneNode());
	
	Body *body = new Body(world, collision);
	
	// Assume pnly one mesh per entity
	Ogre::AxisAlignedBox box = entity->getSceneNode()->getAttachedObject(0)->getBoundingBox();
	float mass = calcMass(entity->getMaterial(), box.volume());
	Ogre::Vector3 inertia = OgreNewt::MomentOfInertia::CalcBoxSolid(mass, box.getSize());

	body->attachToNode(entity->getSceneNode());
	body->setPositionOrientation(entity->getPosition(), entity->getOrientation());
	body->setMassMatrix(mass, inertia);
	body->setCenterOfMass(box.getCenter());
	body->setCustomForceAndTorqueCallback<OgreNewtPhysicsManager>(&OgreNewtPhysicsManager::dynamicBodyForceCallback, this);
	
	delete collision;

	return body;
}

Body* OgreNewtPhysicsManager::createStaticBody(SharedPtr<Entity> &entity) {
	if (entity->getSceneNode() == 0) {
		throw exception("createStaticBody failed because entity did not have a scenenode.");
	}
	// Parse the ogre scene node into an Newton physics body
	OgreNewt::CollisionPrimitives::TreeCollisionSceneParser *parser = new OgreNewt::CollisionPrimitives::TreeCollisionSceneParser(world);
	parser->parseScene(entity->getSceneNode());

	// Create a body using the parsed mesh data
	Body *body = new Body(world, parser);
	body->attachToNode(entity->getSceneNode());
	body->setPositionOrientation(entity->getPosition(), entity->getOrientation());
	
	// Clean up
	delete parser;

	return body;
}

Body* OgreNewtPhysicsManager::createPlayerBody(SharedPtr<Entity> &entity) {

	if (entity->getSceneNode() == 0) {
		throw exception("createDynamicBody failed because entity did not have a scenenode.");
	}

	// We need to detach the camera if it is attached to the object. 
	// Otherwise, object 0 might be the camera and not the mesh.
	bool entityHasCameraAttached = gameLogic->getCameraEntity().getPointer() == entity.getPointer();

	if(entityHasCameraAttached) {
		entity->getSceneNode()->detachObject(gameLogic->getRenderer()->getDefaultCamera());	
	} 

	// We always create a box for the player model since it is easier to predict how the player will move around.
	Ogre::AxisAlignedBox box = entity->getSceneNode()->getAttachedObject(0)->getBoundingBox();
	Collision *collision = new OgreNewt::CollisionPrimitives::Box(world, box.getSize(), Ogre::Quaternion::IDENTITY, box.getCenter());
	Body *body = new Body(world, collision);

	float mass = calcMass(entity->getMaterial(), box.volume()) * 2;
	Ogre::Vector3 inertia = OgreNewt::MomentOfInertia::CalcBoxSolid(numeric_limits<float>::infinity(), box.getSize());

	body->attachToNode(entity->getSceneNode());
	body->setPositionOrientation(entity->getPosition(), entity->getOrientation());
	body->setMassMatrix(mass, inertia);
	body->setCenterOfMass(box.getCenter());
	body->setCustomForceAndTorqueCallback<OgreNewtPhysicsManager>(&OgreNewtPhysicsManager::playerBodyForceCallback, this);
	
	
	delete collision;

	// We need to detach the camera if it is attached to the object. 
	// Otherwise, object 0 might be the camera and not the mesh.
	if(entityHasCameraAttached) {
		entity->getSceneNode()->attachObject(gameLogic->getRenderer()->getDefaultCamera());
	}


	body->setAutoFreeze(0);
	upVectors.push_back(new OgreNewt::BasicJoints::UpVector(world, body, Ogre::Vector3::UNIT_Y));
	body->setContinuousCollisionMode(1);

	return body;
}

float OgreNewtPhysicsManager::calcMass(EntityMaterial material, float volume) {
	switch (material) {
		case EMT_rubber:
			return volume * 0.6f;
		case EMT_steel:
			return volume * 3.0f;
		case EMT_stone:
			return volume * 5.0f;
		case EMT_styrofoam:
			return volume * 0.2f;
		case EMT_wood:
			return volume * 1.0f;
		case EMT_player:
			return volume * 2.0f;
		default:
			throw exception("Unknown material type when calculating mass: " + material);
	}
}

int OgreNewtPhysicsManager::userProcess() {
	Entity* entity0 = (Entity*)m_body0->getUserData();
	Entity* entity1 = (Entity*)m_body1->getUserData();

	float collisionSpeed = getContactNormalSpeed();
	Ogre::Vector3 contactForce = getContactForce();

	setContactElasticity(getElasticityValue(entity0->getMaterial(), entity1->getMaterial()));
	if (entity0->getMaterial() == EMT_player || entity1->getMaterial() == EMT_player) {
		setContactStaticFrictionCoef(2.0f, 0);
		setContactKineticFrictionCoef(2.0f, 0);
	}

	if (contactForce.y > 0) {
		entity0->setContact(true);
		entity1->setContact(true);
	} 
	
	if (collisionSpeed > 1.0f) {

		int id0 = entity0->getId();
		int id1 = entity1->getId();
		int hashId;

		if (id0 < id1) {
			hashId = (id0 << 16) + id1;
		} else {
			hashId = (id1 << 16) + id0;
		}

		if (collisionHashSet.find(hashId) == collisionHashSet.end()) {
			collisionHashSet.insert(hashId);

			SharedPtr<Message> message = SharedPtr<Message>(new CollisionMessage(id0, id1, collisionSpeed));
			gameLogic->sendMessage(message);
			//std::cout << "Collision! Between " << id0 << " and " << id1 << ". Speed: " << collisionSpeed << std::endl;
		}
	}

	return 1;
}


void OgreNewtPhysicsManager::dynamicBodyForceCallback(Body *body) {
	float mass;
	Ogre::Vector3 inertia;
	body->getMassMatrix(mass, inertia);
	body->setForce(Ogre::Vector3(0, GRAVITY * mass, 0));
}

void OgreNewtPhysicsManager::playerBodyForceCallback(OgreNewt::Body *body) {
	Entity *entity = (Entity*)body->getUserData();
	Ogre::Vector3 velocity = body->getVelocity();
	float maxMoveSpeed = MAX_MOVEMENT_SPEED;

	// Prevent player bodies from rotating, rotate should only be done by mouse movement.
	body->setOmega(Ogre::Vector3(0, 0, 0));

	IdToImpulseMap::iterator impulseElem = idToImpulseMap.find(entity->getId());
	if (impulseElem != idToImpulseMap.end()) {
		velocity += impulseElem->second;
	}

	if (entity->hasContact()) {
		float savedVelocityY = velocity.y;
		velocity.y = 0; // We don't modify vertical velocity

		velocity *= 0.95f; // Damp velocity so the player can switch direction and stop fast.
		if (velocity.squaredLength() > maxMoveSpeed * maxMoveSpeed) {
			velocity.normalise();
			velocity *= maxMoveSpeed;
	
		}

		velocity.y = savedVelocityY;
	}

	body->setVelocity(velocity);
	entity->setContact(false);	// Updated by the collisionCallback if the body still has contact

	float mass;
	Ogre::Vector3 inertia;
	body->getMassMatrix(mass, inertia);
	body->setForce(Ogre::Vector3(0, GRAVITY * mass, 0));

	
}


float OgreNewtPhysicsManager::getElasticityValue(EntityMaterial material1, EntityMaterial material2) {
	float elasticity = 0;
	if (material1 == EMT_rubber || material2 == EMT_rubber) {
		return 0.9f;
	}
	if (material1 == EMT_styrofoam || material2 == EMT_styrofoam) {
		return 0.5f;
	}
	if (material1 == EMT_wood || material2 == EMT_wood) {
		return 0.2f;
	}
	if (material1 == EMT_player || material2 == EMT_player) {
		return 0.2f;
	}
	if (material1 == EMT_stone || material2 == EMT_stone) {
		return 0.1f;
	}
	if (material1 == EMT_steel || material2 == EMT_steel) {
		return 0.1f;
	}

	return 0.1f;
}
