/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "Template.h"
#include "RakNetworkManager.h"
#include "GameLogic.h"
#include "Entity.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"

using namespace Bismuth;
using namespace Bismuth::Network;

RakNetworkManager::RakNetworkManager(SharedPtr<GameLogic> gameLogic) {
	RakPeerInterface *rakPeer = RakNetworkFactory::GetRakPeerInterface();
	SocketDescriptor s(666, 0);
	rakPeer->Startup(1, 10, &s, 1);
	


	//SharedPtr<Entity> e = gameLogic->getEntityById(3);
}

RakNetworkManager::~RakNetworkManager() {

}

bool RakNetworkManager::connect(const std::string &ip, const int port) {
	return false;
}