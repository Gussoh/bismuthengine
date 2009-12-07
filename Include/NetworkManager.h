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


#define SERVER_PORT 27542

namespace Bismuth {

	// Dirty hack to avoid including GameLogic.h:
	typedef std::map<int, SharedPtr<Entity> > EntityList;
 
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
			virtual bool connect(const std::string &host) = 0;

			virtual void disconnect() = 0;

			virtual void startServer(int numberOfPlayers) = 0;
			
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
			* Send a message to self. Used by the server only.
			*/
			virtual void sendMessageToSelf(SharedPtr<Message> message) = 0;

			/**
			* Get a received message.
			* @param blocking if true: block until message is recieved. If false: return empty ptr if empty queue.
			*/
			virtual SharedPtr<Message> getMessage(bool blocking) = 0;
			
			/**
			*
			*/
			virtual int getNumberOfConnectedClients() = 0;
		private:
		};
	}
}