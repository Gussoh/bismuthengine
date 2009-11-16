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

	return peer->Connect(host.c_str(), port, 0, 0);
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

/**
void sendEntities(List entities) (
	throw exception if not server
	send to all
)

Entity getEntity() {
	get entity received in fifo, or null if empty
}

void sendMessage(Message message) (
	if client - send to server
	if server - send to all
)

Message getMessage() {
	get message received in fifo, or null if empty
}

bool connect(server, port) (
)

void startServer() (
)

void disconnect() (
)*/