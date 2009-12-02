/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "Template.h"
#include "RakNetworkManager.h"
#include "GameLogic.h"
#include "Entity.h"
#include "BitStream.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "PacketPriority.h"
#include "MessageIdentifiers.h"
#include "RaknetStream.h"
#include <windows.h>

using namespace Bismuth;
using namespace Bismuth::Network;
using namespace RakNet;

static const char ORDERING_CHANNEL = 1; // use same ordering channel for everything... until we know everything is working.

RakNetworkManager::RakNetworkManager(GameLogic *gameLogic) {
	this->gameLogic = gameLogic;
	peer = RakNetworkFactory::GetRakPeerInterface();
	//SharedPtr<Entity> e = gameLogic->getEntityById(3);
}

RakNetworkManager::~RakNetworkManager() {
	peer->Shutdown(10); // arbitrary number 10?
	RakNetworkFactory::DestroyRakPeerInterface(peer);
}

bool RakNetworkManager::connect(const std::string &host, const int port = SERVER_PORT) {
	isServer = false;
	peer->Startup(1, 10, &SocketDescriptor(), 1);

	if(!peer->Connect(host.c_str(), port, 0, 0)) {
		return false;
	}

	Packet *packet;

	while (1) {
		packet=peer->Receive();
		if (packet)	{
			switch (packet->data[0]) {
			
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("Our connection request has been accepted.\n");
				return true;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				return false;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed.\n");
				return false;
			default:
				printf("Uncaught message with identifier %i has arrived.\n", packet->data[0]);
				return false;
			}

			peer->DeallocatePacket(packet);
		}
	}

	return false; // this is not supposed to happen
}

void RakNetworkManager::disconnect() {
	if (! isServer ) {
		/// ??? WHAT TO DO!? no such command
	}
}

void RakNetworkManager::startServer() {
	isServer = true;
	peer->Startup(MAX_CLIENTS, 10, &SocketDescriptor(SERVER_PORT,0), 1);
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

}

void RakNetworkManager::sendEntities(EntityList &entities) {
	/**
	throw exception if not server
	send to all
	*/

	MessageID id = ID_ENTITY;
	for (EntityList::iterator iter = entities.begin(); iter != entities.end(); ++iter) {
		
		RakNetStream rakNetStream;
		rakNetStream.write(id);
		(*iter)->serialize(&rakNetStream);
		peer->Send(rakNetStream.getRakNetBitStream(), MEDIUM_PRIORITY, RELIABLE_ORDERED, ORDERING_CHANNEL, UNASSIGNED_SYSTEM_ADDRESS, true);
	}
}

SharedPtr<Entity> RakNetworkManager::getEntity() {
	//get entity received in fifo, or empty message if empty
	if (entityQueue.empty()) {
		receiveAll();
	}
	if (entityQueue.empty()) {
		return SharedPtr<Entity>();
	} else {
		SharedPtr<Entity> entity = entityQueue.front();
		entityQueue.pop();
		return entity;
	}
}

void RakNetworkManager::sendMessage(SharedPtr<Message> message) {
	/*
	if client - send to server
	if server - send to all
	(done automatically if message is sent "to all connected nodes" -> broadcast!)
	*/

	MessageID id = ID_MESSAGE;
	RakNetStream rakNetStream;
	rakNetStream.write(id);
	message->serialize(&rakNetStream);
	peer->Send(rakNetStream.getRakNetBitStream(), MEDIUM_PRIORITY, RELIABLE_ORDERED, ORDERING_CHANNEL, UNASSIGNED_SYSTEM_ADDRESS, true);
}

SharedPtr<Message> RakNetworkManager::getMessage(bool block) {

	if (block) {
		while(messageQueue.empty()) {
			receiveAll();
			if (messageQueue.empty()) {
				Sleep(3);
			}
		}
	} else {
		if (messageQueue.empty()) {
			receiveAll();
		}

		if (messageQueue.empty()) {
			return SharedPtr<Message>();
		}
	}
	
	SharedPtr<Message> message = messageQueue.front();
	messageQueue.pop();
	
	return message;
}

void RakNetworkManager::sendEndOfFrame(float step) {
	SharedPtr<Message> m = SharedPtr<Message>(new EndOfFrameMessage(step));
	RakNetworkManager::sendMessage(m);
	messageQueue.push(m);
}

void RakNetworkManager::receiveAll() {
	while (!endOfFrame || isServer) {
		Packet *packet;
		packet=peer->Receive();
		if (packet)	{
			switch (packet->data[0]) {
			
			case ID_MESSAGE:
				{
				messageQueue.push(MessageFactory::createFromStream(&RakNetStream(packet)));
				break;
				}
			case ID_ENTITY:
				{
				SharedPtr<Entity> entity(new Entity());
				entity->deserialize(&RakNetStream(packet));
				entityQueue.push(entity);
				break;
				}
			case ID_NEW_INCOMING_CONNECTION:
				{
				printf("Network got incoming connection... OK.. do something?\n");
				break;
				}
			default:
				printf("Uncaught message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}

			peer->DeallocatePacket(packet);
		} else {
		}
	}
}

