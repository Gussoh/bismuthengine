/**
 * @file AudioComponent.cpp
 */

#include "stdafx.h"
#include "Template.h"
#include "AudioComponent.h"

using namespace BismuthAudio;

AudioComponent::AudioComponent(Bismuth::Entity &entity) {
	this->entity = &entity;
}

AudioComponent::~AudioComponent() {

}

/**
 * Activtes the component when the entity is activated
 */
void AudioComponent::activate() {

}
/**
 * Deactivates the component when the entity is deactivated
 */
void AudioComponent::deactivate() {

}

/**
 * Called when the component is attached to an entity
 */
void AudioComponent::attached() {

}

/**
 * Called when the component is detached from an entity
 */
void AudioComponent::detached() {

}
