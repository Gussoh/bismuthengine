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

#include "Entity.h"
#include "Message.h"
#include "IStream.h"

#define MAX_CLIENTS 16
#define SERVER_PORT 27542

namespace Bismuth {

	// Dirty hack to avoid including GameLogic.h:
	typedef std::vector<SharedPtr<Entity> > EntityList;

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
			
			/**
			* Send a list of entries FROM THE SERVER to all clients
			*
			*/
			virtual void sendEntities(EntityList &entities) = 0;

			/**
			* Get an received entity. Call repaeatedly until it returns an empty SharedPtr
			*
			*/
			virtual SharedPtr<Entity> getEntity() = 0;

			/**
			* Send a message to either the server or all clients depending on whether you are client or server
			*
			*/
			virtual void sendMessage(SharedPtr<Message> message) = 0;

			/**
			* Get an received message. Call repaeatedly until it returns an empty SharedPtr
			*
			*/
			virtual SharedPtr<Message> getMessage() = 0;
		private:
		};
	}
}