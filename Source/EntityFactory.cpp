/**
 * @file EntityFactory.cpp
 */

#include "stdafx.h"
#include "EntityFactory.h"
#include "Entity.h"
#include "GameLogic.h"

using namespace Bismuth;

EntityFactory::EntityFactory() {

}

EntityFactory::~EntityFactory() {

}

void EntityFactory::registerType(const std::string &name, EntityCreatorFunc func) {
	types.insert(std::make_pair(name, func));
}

SharedPtr<Entity> EntityFactory::create(const std::string &name) {
	return types[name](entityCounter++);
}