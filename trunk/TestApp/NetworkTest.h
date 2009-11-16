#pragma once

#include "NetworkManager.h"
#include <iostream>

using namespace Bismuth;
using namespace Bismuth::Network;

class NetworkTest : public Test {
public:

	virtual void run() {
		GameLogic engine1(true);
		SharedPtr<NetworkManager> server = engine1.getNetworkManager();
		GameLogic engine2(false);
		SharedPtr<NetworkManager> client = engine2.getNetworkManager();

		server->startServer();

		if (client->connect("127.0.0.1", SERVER_PORT)) {
			std::cout << "Network test was successful!" << std::endl;
		} else {
			std::cout << "Network test FAILED!" << std::endl;
		}
	}
};