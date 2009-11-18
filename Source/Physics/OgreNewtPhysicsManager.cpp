/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "OgreNewtPhysicsManager.h"
#include "Newton.h"
#include "Renderer.h"

using namespace Bismuth;
using namespace Bismuth::Physics;
using namespace Bismuth::Graphics;
using namespace std;
using namespace OgreNewt;

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
	delete world;
}

void OgreNewtPhysicsManager::getNearbyEntities(float radius, SharedPtr<Entity> source, vector<SharedPtr<Entity> > &entityList) {
		
}

void OgreNewtPhysicsManager::addEntity(SharedPtr<Entity> entity) {
	
}

void OgreNewtPhysicsManager::removeEntity(SharedPtr<Entity> entity) {

}

void OgreNewtPhysicsManager::removeAllEntities() {

}
