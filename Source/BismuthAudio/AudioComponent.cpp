/**
 * @file AudioComponent.cpp
 */

#include "stdafx.h"
#include "Template.h"
#include "AudioComponent.h"
#include "Entity.h"

using namespace BismuthAudio;

AudioComponent::AudioComponent() {
	
}

AudioComponent::~AudioComponent() {

}

void AudioComponent::activate() {

}

void AudioComponent::deactivate() {

}

void AudioComponent::attached(Bismuth::Entity *owner) {
	IComponent::attached(owner);
}

void AudioComponent::detached() {
	IComponent::detached();
}


