#pragma once

#include "NetworkManager.h"
#include "DebugLogic.h"
#include <iostream>

using namespace Bismuth;
using namespace Bismuth::Network;

class NetworkTest : public Test {
public:

	virtual void run() {
		DebugLogic engine1;
		NetworkManager *server = engine1.createNetworkManager();
		DebugLogic engine2;
		NetworkManager *client = engine2.createNetworkManager();

		server->startServer();

		if (client->connect("127.0.0.1", SERVER_PORT)) {
			std::cout << "Network test: Connection successful!" << std::endl;
			client->sendMessage(SharedPtr<Message>(new Message((MessageType) 21)));
			
			EntityList entities;
			entities.push_back(SharedPtr<Entity>(new Entity(NULL)));
			server->sendEntities(entities);
			server->sendEndOfFrame();

			client->nextFrame();
			while (client->hasMoreEntitiesInFrame()) {
				if(!client->getEntity().isNull()) {
					std::cout << "Network test: Client received entity!" << std::endl;
				}
			}
			std::cout << "Network test: End of frame!" << std::endl;

			if(!server->getMessage().isNull()) {
				std::cout << "Network test: Server receieved message!" << std::endl;
			}


			std::cout << "Network test: WOOHOOHOOO SUCCESS!" << std::endl;
		} else {
			std::cout << "Network test FAILED!" << std::endl;
		}
	}
};