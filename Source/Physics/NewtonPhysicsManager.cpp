/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "Template.h"
#include "NewtonPhysicsManager.h"
#include "Newton.h"

using namespace Bismuth;
using namespace Bismuth::Physics;
using namespace std;


NewtonPhysicsManager::NewtonPhysicsManager(SharedPtr<GameLogic> gameLogic) {
	this->gameLogic = gameLogic;
}

NewtonPhysicsManager::~NewtonPhysicsManager() {

}

void NewtonPhysicsManager::getNearbyEntities(float radius, SharedPtr<Entity> source, vector<SharedPtr<Entity> > &entityList) {

}

void NewtonPhysicsManager::addEntity(SharedPtr<Entity> entity) {

}

void NewtonPhysicsManager::removeEntity(SharedPtr<Entity> entity) {

}

void NewtonPhysicsManager::removeAllEntities() {

}
