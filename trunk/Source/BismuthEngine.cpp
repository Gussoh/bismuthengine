/**
 * @file BismuthEngine.cpp
 */

#include "stdafx.h"
#include "BismuthEngine.h"

using namespace Bismuth;

BismuthEngine::BismuthEngine() {
	this->audioManager = new BismuthAudio::AudioManager();
	this->networkManager = new BismuthNetwork::NetworkManager();
}

BismuthEngine::~BismuthEngine() {

}