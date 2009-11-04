/**
 * @file AudioComponent.cpp
 */

#include "stdafx.h"
#include "Template.h"
#include "AudioComponent.h"
#include "Entity.h"

using namespace BismuthAudio;
using namespace Bismuth;

AudioComponent::AudioComponent() {
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
void AudioComponent::attached(Entity *owner) {
	IComponent::attached(owner);
}

/**
 * Called when the component is detached from an entity
 */
void AudioComponent::detached() {

}
