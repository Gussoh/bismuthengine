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

			client->sendMessage(SharedPtr<Message>(new CollisionMessage(1, 2, 4.5f)));
			//client->sendMessage(SharedPtr<Message>(new PlayerMoveMessage((char) 3)));
			//client->sendMessage(SharedPtr<Message>(new PlayerRotateMessage(Ogre::Quaternion::IDENTITY)));
			
			//EntityList entities;
			//entities.push_back(SharedPtr<Entity>(new Entity(3)));
			//server->sendEntities(entities);
			//server->sendEndOfFrame();

			//client->nextFrame();
			//while (client->hasMoreEntitiesInFrame()) {
			//	if(!client->getEntity().isNull()) {
			//		std::cout << "Network test: Client received entity!" << std::endl;
			//	}
			//}
			std::cout << "Network test: Waiting for message!" << std::endl;

			//for(int i = 0; i < 3; i++) {
				SharedPtr<Message> message = server->getMessage(false);
				if(!message.isNull()) {
					std::cout << "Network test: Server receieved message type: " << message->getType() << std::endl;
				}
			//}
			//if(!client->getMessage(true).isNull()) {
			//	std::cout << "Network test: Client receieved message!" << std::endl;
			//}


			std::cout << "Network test: WOOHOOHOOO SUCCESS!" << std::endl;
		} else {
			std::cout << "Network test FAILED!" << std::endl;
		}
	}
};