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
	components.push_back(component);
}

void Entity::removeComponent(SharedPtr<IComponent> component) {
	for (IComponentList::iterator iter = components.begin(); iter != components.end(); ++iter) {
		if (*iter == component) {
			iter = components.erase(iter);
		}
	}
}