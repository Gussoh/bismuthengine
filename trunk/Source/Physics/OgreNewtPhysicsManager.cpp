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
	clearStaticGeometry();
	
	delete world;
	OgreNewt::Debugger::getSingleton().deInit();
}

void OgreNewtPhysicsManager::getNearbyEntities(float radius, SharedPtr<Entity> source, vector<SharedPtr<Entity> > &entityList) {
	
}

void OgreNewtPhysicsManager::addEntity(SharedPtr<Entity> &entity) {
	if (!entity->getSceneNode())
	{
		return; // TODO: Tell caller that adding physics is not possible in some way
	}
	Collision *collision = new ConvexHull(world, entity->getSceneNode());
	Body *body = new Body(world, collision);
	body->attachToNode(entity->getSceneNode());
	body->setPositionOrientation(entity->getPosition(), entity->getOrientation());
	delete collision;

	idToBodyMap.insert(pair<int, OgreNewt::Body*>(entity->getId(), body));
}

void OgreNewtPhysicsManager::removeEntity(SharedPtr<Entity> &entity) {
	
}

void OgreNewtPhysicsManager::removeAllEntities() {
	for (IdToBodyMap::iterator iter = idToBodyMap.begin(); iter != idToBodyMap.end(); iter++) {
		delete iter->second;
	}

	idToBodyMap.clear();
}

void OgreNewtPhysicsManager::addStaticGeometry(Ogre::SceneNode *mesh) {
	// Parse the ogre scene node into an Newton physics body
	OgreNewt::CollisionPrimitives::TreeCollisionSceneParser *parser = new OgreNewt::CollisionPrimitives::TreeCollisionSceneParser(world);
	parser->parseScene(mesh, true);

	// Create a body using the parsed mesh data
	OgreNewt::Body *body = new OgreNewt::Body(world, parser);
	staticGeometry.push_back(body);

	// Clean up
	delete parser;
}

void OgreNewtPhysicsManager::clearStaticGeometry() {
	for (StaticGeometryList::iterator iter = staticGeometry.begin(); iter != staticGeometry.end(); ++iter) {
		delete (*iter);
	}

	staticGeometry.clear();
}