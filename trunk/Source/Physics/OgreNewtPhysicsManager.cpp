/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "OgreNewtPhysicsManager.h"
#include "OgreNewt.h"
#include "Renderer.h"


using namespace Bismuth;
using namespace Bismuth::Physics;
using namespace Bismuth::Graphics;
using namespace std;
using namespace OgreNewt;
using namespace OgreNewt::CollisionPrimitives;

OgreNewtPhysicsManager::OgreNewtPhysicsManager(GameLogic *gameLogic) {
	this->gameLogic = gameLogic;
	this->world = new World();
	
	Renderer *renderer = gameLogic->getRenderer();

	/* Default frame rate: 60 fps */
	frameListener = new BasicFrameListener(Ogre::Root::getSingleton().getAutoCreatedWindow(), 
										   renderer->getDefaultSceneManager(), 
										   world);

	Ogre::Root::getSingleton().addFrameListener(frameListener);
}

OgreNewtPhysicsManager::~OgreNewtPhysicsManager() {
	Ogre::Root::getSingleton().removeFrameListener(frameListener);
	delete frameListener;

	removeAllEntities();
	
	delete world;
	OgreNewt::Debugger::getSingleton().deInit();
}

void OgreNewtPhysicsManager::getNearbyEntities(float radius, SharedPtr<Entity> source, vector<SharedPtr<Entity> > &entityList) {
	
}

void OgreNewtPhysicsManager::addEntity(SharedPtr<Entity> &entity) {
	
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
			throw exception("Entity type not implemented in OgreNewtPhysicsManager.");

	}
	
	idToBodyMap.insert(pair<int, OgreNewt::Body*>(entity->getId(), body));
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

void OgreNewtPhysicsManager::update() {
	EntityList *entities = gameLogic->getEntities();
	for (EntityList::iterator entity = entities->begin(); entity != entities->end(); entity++) {
		//IdToBodyMap::iterator body = idToBodyMap.find((*entity)->getId());

		//(*entity)->setPosition((*entity)->getSceneNode()->getPosition());
		//(*entity)->setOrientation((*entity)->getSceneNode()->getOrientation());
		/*if (body != idToBodyMap.end()) {
			Ogre::Vector3 position;
			Ogre::Quaternion orientation;
			body->second->getPositionOrientation(position, orientation);
			
			(*entity)->setPosition(position);
			(*entity)->setOrientation(orientation);
		}*/
	}
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
	Ogre::Vector3 size(0.5f, 1.0f, 0.5f);
	Collision *collision = new OgreNewt::CollisionPrimitives::Box(world, size);

	Body *body = new Body(world, collision);
	body->setPositionOrientation(entity->getPosition(), entity->getOrientation());
	body->attachToNode(entity->getSceneNode());
	body->setStandardForceCallback();
	
	float mass = 80.0f;
	Ogre::Vector3 inertia = OgreNewt::MomentOfInertia::CalcBoxSolid(1000000.0f, size);
	body->setMassMatrix(mass, inertia);
	
	delete collision;

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
