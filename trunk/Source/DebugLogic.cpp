/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "DebugLogic.h"
#include "RakNetworkManager.h"

using namespace Bismuth;
using namespace Bismuth::Network;

DebugLogic::DebugLogic() {

}

DebugLogic::~DebugLogic() {

}

NetworkManager* DebugLogic::createNetworkManager() {
	return new RakNetworkManager(NULL);
}
