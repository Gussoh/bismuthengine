/**
 * @file Entity.cpp
 */

#include "stdafx.h"
#include "Entity.h"
#include "OgreRoot.h"

using namespace Bismuth;

Entity::Entity(int id) : id(id), audioProperties(), contact(false) {
	
}
	
Entity::Entity() {
}

Entity::~Entity() {
	sceneNode->getParentSceneNode()->removeAndDestroyChild(sceneNode->getName());	
}

void Entity::serialize(IStream *stream) const {
	stream->write(getId());
	stream->write(getPosition());
	stream->write(getOrientation());
	stream->write((int)getMaterial());
	stream->write((int)getType());
}

void Entity::deserialize(IStream *stream) {
	setId(stream->readInt());
	setPosition(stream->readVector3());
	setOrientation(stream->readQuaternion());
	setMaterial((EntityMaterial)stream->readInt());
	setType((EntityType)stream->readInt());
}

