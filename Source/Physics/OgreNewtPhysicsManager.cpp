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

bool OgreNewtPhysicsManager::addEntity(SharedPtr<Entity> &entity) {
	if (!entity->getSceneNode()) {
		return false; // TODO: Tell caller that adding physics is not possible in some way
	}
	
	Body *body;

	if (entity->isStatic()) {
		body = createStaticBody(entity->getSceneNode());
	} else {
		body = createDynamicBody(entity->getSceneNode());
	}

	body->setPositionOrientation(entity->getPosition(), entity->getOrientation());
	body->attachToNode(entity->getSceneNode());

	idToBodyMap.insert(pair<int, OgreNewt::Body*>(entity->getId(), body));

	return true;
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


Body* OgreNewtPhysicsManager::createDynamicBody(Ogre::SceneNode *sceneNode) {
	Collision *collision = new ConvexHull(world, sceneNode);
	Body *body = new Body(world, collision);
	
	delete collision;

	return body;
}

Body* OgreNewtPhysicsManager::createStaticBody(Ogre::SceneNode *sceneNode) {
	// Parse the ogre scene node into an Newton physics body
	OgreNewt::CollisionPrimitives::TreeCollisionSceneParser *parser = new OgreNewt::CollisionPrimitives::TreeCollisionSceneParser(world);
	parser->parseScene(sceneNode);

	// Create a body using the parsed mesh data
	OgreNewt::Body *body = new OgreNewt::Body(world, parser);
	
	// Clean up
	delete parser;

	return body;
}
