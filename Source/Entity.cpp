/**
 * @file Entity.cpp
 */

#include "stdafx.h"
#include "Entity.h"

using namespace Bismuth;

Entity::Entity(int id) {
	this->id = id;
}
	
Entity::Entity() {
}

Entity::~Entity() {

}

void Entity::serialize(IStream *stream) const {
	stream->write(id);
	stream->write(position);
	stream->write(orientation);
}

void Entity::deserialize(IStream *stream) {
	id = stream->readInt();
	position = stream->readVector3();
	orientation = stream->readQuaternion();
}