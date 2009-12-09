/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "OgreNewtPhysicsManager.h"
#include "OgreNewt.h"
#include "Renderer.h"
#include <limits>


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

		
		// Set contact for each entity to false. It will be updated by the callback to userProcess.
		entity->setContact(false);
		

		// Fix the yaw and roll for all player controlled entities
		if (entity->getType() == ET_player) {

			Ogre::Radian yaw = entity->getOrientation().getYaw();
			entity->setOrientation(Ogre::Quaternion());
			entity->getSceneNode()->yaw(yaw);
		}
	
		if (body != 0 && entity->hasPositionOrientationChanged()) {
			body->setPositionOrientation(entity->getPosition(), entity->getOrientation());
			entity->setPositionOrientationChanged(false);
		}
	}
	
	world->update(stepTime);
}

void OgreNewtPhysicsManager::addImpulse(SharedPtr<Entity> &entity, Ogre::Vector3 &direction) {
	Body *body = idToBodyMap.find(entity->getId())->second;
	body->addImpulse(direction, entity->getSceneNode()->getPosition());
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
	body->setStandardForceCallback();
	
	std::cout << "Entity: " << entity->getId() << ", mass: " << mass << ", volume: " << box.getSize() << std::endl;
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

	float mass = calcMass(entity->getMaterial(), box.volume());
	Ogre::Vector3 inertia = OgreNewt::MomentOfInertia::CalcBoxSolid(numeric_limits<float>::infinity(), box.getSize());

	body->attachToNode(entity->getSceneNode());
	body->setPositionOrientation(entity->getPosition(), entity->getOrientation());
	body->setMassMatrix(mass, inertia);
	body->setCenterOfMass(box.getCenter());
	body->setStandardForceCallback();
	
	std::cout << "Entity: " << entity->getId() << ", mass: " << mass << ", volume: " << box.getSize() << std::endl;
	delete collision;

	// We need to detach the camera if it is attached to the object. 
	// Otherwise, object 0 might be the camera and not the mesh.
	if(entityHasCameraAttached) {
		entity->getSceneNode()->attachObject(gameLogic->getRenderer()->getDefaultCamera());
	}

	body->setAutoFreeze(0);

	return body;
	
}

float OgreNewtPhysicsManager::calcMass(EntityMaterial material, float volume) {
	switch (material) {
		case EMT_plastic:
			return volume * 0.6f;
		case EMT_steel:
			return volume * 3.0f;
		case EMT_stone:
			return volume * 5.0f;
		case EMT_styrofoam:
			return volume * 0.2f;
		case EMT_wood:
			return volume * 1.0f;
		default:
			throw exception("Unknown material type when calculating mass: " + material);
	}
}

int OgreNewtPhysicsManager::userProcess() {
	Entity* entity0 = (Entity*)m_body0->getUserData();
	Entity* entity1 = (Entity*)m_body1->getUserData();

	float collisionSpeed = getContactNormalSpeed();
	Ogre::Vector3 contactForce = getContactForce();


	if (contactForce.x + contactForce.y + contactForce.z > 0) {
		std::cout << "Contact! Between " << entity0->getId() << " and " << entity1->getId() << "." << std::endl;
		entity1->setContact(true);
		entity0->setContact(true);
	} else if (contactForce.x + contactForce.y + contactForce.z < 0) {
		entity0->setContact(true);
		entity1->setContact(true);
		std::cout << "Contact! Between " << entity0->getId() << " and " << entity1->getId() << "." << std::endl;
	}
	
	
	if (collisionSpeed > 100.0f) {
		SharedPtr<Message> message = SharedPtr<Message>(new CollisionMessage(entity0->getId(), entity1->getId(), (m_body1->getVelocity() - m_body0->getVelocity()).length()));
		gameLogic->sendMessage(message);
		std::cout << "Collision! Between " << entity0->getId() << " and " << entity1->getId() << "." << std::endl;
	}

	return 1;
}