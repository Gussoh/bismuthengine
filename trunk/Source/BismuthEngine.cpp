/**
 * @file BismuthEngine.cpp
 */

#include "stdafx.h"
#include "BismuthEngine.h"

using namespace Bismuth;
using namespace Bismuth::Audio;

BismuthEngine::BismuthEngine() {
	this->audioManager = SharedPtr<AudioManager>();
}

BismuthEngine::~BismuthEngine() {

}

