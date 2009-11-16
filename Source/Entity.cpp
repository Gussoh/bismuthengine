/**
 * @file Entity.cpp
 */

#include "stdafx.h"
#include "Entity.h"

using namespace Bismuth;

Entity::Entity(GameLogic *gameLogic) : gameLogic(gameLogic) {

}

Entity::~Entity() {

}

void Entity::serialize(Ogre::SharedPtr<IStream> stream) const {
	stream->write(id);
	stream->write(position);
	stream->write(orientation);
}

void Entity::deserialize(Ogre::SharedPtr<IStream> stream) {
	id = stream->readInt();
	position = stream->readVector3();
	orientation = stream->readQuaternion();
}