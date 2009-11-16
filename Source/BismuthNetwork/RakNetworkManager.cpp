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
#include "MessageIdentifiers.h"
#include "RaknetStream.h"

using namespace Bismuth;
using namespace Bismuth::Network;

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

SharedPtr<Bismuth::IStream> RakNetworkManager::createStream() {
	return SharedPtr<Bismuth::IStream>(new RaknetStream());
}

void sendEntities(std::vector<SharedPtr<Entity> > entities) {
/**
	throw exception if not server
	send to all
	*/
}

SharedPtr<Entity> getEntity() {
	//get entity received in fifo, or null if empty
	return SharedPtr<Entity>();
}

void sendMessage(SharedPtr<Message> message) {
	/*
	if client - send to server
	if server - send to all
	*/
}

SharedPtr<Message> getMessage() {
	// get message received in fifo, or null if empty
	return SharedPtr<Message>();
}
