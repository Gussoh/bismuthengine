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

void AudioComponent::activate() {

}

void AudioComponent::deactivate() {

}

void AudioComponent::attached(Entity *owner) {
	IComponent::attached(owner);
}

void AudioComponent::detached() {
	IComponent::detached();
}


