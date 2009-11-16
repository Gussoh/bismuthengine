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
	class IStream;

	namespace Network {
		/**
		 * NetworkManager class
		 */
		class NetworkManager {
		public:

			/**
			*	Connect to game server. This locks the thread until connection is accepted or timeout.
			*	@param host host or IP
			*	@param post network port
			*	@return true if connection succeeded, false otherwise.
			**/
			virtual bool connect(const std::string &host, const int port) = 0;

			virtual void disconnect() = 0;

			virtual void startServer() = 0;

			virtual SharedPtr<IStream> createStream() = 0;

		private:
		};
	}
}