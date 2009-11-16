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

namespace Bismuth {

	class GameLogic;

	namespace Network {
		/**
		 * NetworkManager class
		 */
		class RakNetworkManager:public NetworkManager {
		public:
			RakNetworkManager(GameLogic *gameLogic);

			virtual ~RakNetworkManager();

			virtual bool connect(const std::string &host, const int port);
			
			virtual void disconnect();

			virtual void startServer();

			virtual SharedPtr<IStream> createStream();
		private:
			RakPeerInterface *peer;
			bool isServer;
			GameLogic *gameLogic;
		};
	}
}