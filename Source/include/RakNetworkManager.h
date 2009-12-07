/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file RakNetworkManager.h
 */

#pragma once
#include "NetworkManager.h"
#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "GameLogic.h"

namespace Bismuth {


	namespace Network {
		/**
		 * NetworkManager class
		 */

		enum PacketId {
			ID_MESSAGE = ID_USER_PACKET_ENUM + 1,	// Message messageid
			ID_ENTITY								// Entity messageid
		};

		class RakNetworkManager:public NetworkManager {
		public:
			RakNetworkManager(GameLogic *gameLogic);

			virtual ~RakNetworkManager();

			virtual bool connect(const std::string &host);
			
			virtual void disconnect();

			virtual void startServer(int numberOfPlayers);

			virtual void sendEntities(EntityList &entities);

			virtual SharedPtr<Entity> getEntity();

			virtual void sendMessage(SharedPtr<Message> message);

			virtual void sendMessageToSelf(SharedPtr<Message> message);

			virtual SharedPtr<Message> getMessage(bool blocking);

			virtual int getNumberOfConnectedClients();
		private:
			RakPeerInterface *peer;
			bool isServer;
			GameLogic *gameLogic;
			std::queue<SharedPtr<Message> > messageQueue;
			std::queue<SharedPtr<Entity> > entityQueue;
			int numberOfClients;

			void receiveAll();
		};
	}
}