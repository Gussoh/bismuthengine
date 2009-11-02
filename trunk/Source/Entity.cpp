/**
 * @file Entity.cpp
 */

#include "stdafx.h"
#include "Entity.h"

using namespace Bismuth;

Entity::Entity() {

}

Entity::~Entity() {

}

void Entity::addComponent(SharedPtr<IComponent> component) {
	if (hasComponent(component)) {
		throw std::logic_error("component is already attached");
	}

	components.push_back(component);
}

void Entity::removeComponent(SharedPtr<IComponent> component) {
	for (IComponentList::iterator iter = components.begin(); iter != components.end(); ++iter) {
		if (*iter == component) {
			iter = components.erase(iter);
		}
	}
}

void Entity::activate() {
	for (IComponentList::iterator iter = components.begin(); iter != components.end(); ++iter) {
		(*iter)->activate();
	}
}

void Entity::deactivate() {
	for (IComponentList::iterator iter = components.begin(); iter != components.end(); ++iter) {
		(*iter)->deactivate();
	}
}

bool Entity::hasComponent(Ogre::SharedPtr<IComponent> component) const {
	for (IComponentList::const_iterator iter = components.begin(); iter != components.end(); ++iter) {
		if (*iter == component) {
			return true;
		}
	}

	return false;
}