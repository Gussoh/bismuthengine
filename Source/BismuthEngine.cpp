/**
 * @file BismuthEngine.cpp
 */

#include "stdafx.h"
#include "BismuthEngine.h"
#include "FmodAudioManager.h"

using namespace Bismuth;
using namespace Bismuth::Audio;

BismuthEngine::BismuthEngine() {
	this->audioManager = SharedPtr<AudioManager>(new FmodAudioManager());
}

BismuthEngine::~BismuthEngine() {

}

