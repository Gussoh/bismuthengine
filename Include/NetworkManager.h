/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file NetworkManager.h
 */

#pragma once


#define MAX_CLIENTS 16
#define SERVER_PORT 27542

namespace Bismuth {

	class GameLogic;

	namespace Network {
		/**
		 * NetworkManager class
		 */
		class NetworkManager {
		public:
			//NetworkManager(SharedPtr<GameLogic> gameLogic);

			virtual ~NetworkManager();

			virtual bool connect(const std::string &host, const int port) = 0;

			virtual void disconnect();

			virtual void startServer();

		private:
		};
	}
}