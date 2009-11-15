/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file GameLogic.h
 */

#pragma once

#include "Message.h"
#include "AudioManager.h"
#include "PhysicsManager.h"
#include "NetworkManager.h"

namespace Bismuth {

	class Entity;
	using Bismuth::Audio::AudioManager;
	using Bismuth::Physics::PhysicsManager;
	using Bismuth::Network::NetworkManager;

	/**
	 * GameLogic class
	 *
	 * Override handleMessage in a sub class to create your own game logic.
	 * The server game logic should also dispatch all messages to the clients.
	 */
	class GameLogic {
	public:
		GameLogic(bool isServer);
		virtual ~GameLogic();

		/**
		 * Ticks the game logic once, handles all queued messages
		 * @param elapsedTime Time in seconds since last update
		 */
		void update(float elapsedTime);

		/**
		 * Adds a message to the message queue, note: asynchronius
		 * @param message Message to send
		 */
		void sendMessage(SharedPtr<Message> message);

		/**
		 * Get an entity by id
		 * \param id Id of the entity to lookup
		 * \return The entity with the specified id, null if it does not exist
		 */
		SharedPtr<Entity> getEntityById(int id);

		SharedPtr<AudioManager> getAudioManager()		{ return audioManager; }
		SharedPtr<PhysicsManager> getPhysicsManager()	{ return physicsManager; }
		SharedPtr<NetworkManager> getNetworkManager()	{ return networkManager; }

		/**
		 * Get the current player entity
		 * \return The current player entity, null if one has not been assigned
		 */
		SharedPtr<Entity> getPlayerEntity() const { return playerEntity; };

	protected:
		void setPlayerEntity(SharedPtr<Entity> entity) { playerEntity = entity; }

		/**
		 * Handle a message
		 * \param message A pointer to the message to handle
		 */
		void handleMessage(SharedPtr<Message> message);

		// Message handlers
		void handleDebugOutMessage(SharedPtr<Message> message);
		void handleEntityAssignedMessage(SharedPtr<Message> message);

	private:
		std::vector<SharedPtr<Entity> > entities;
		std::queue<SharedPtr<Message> > messageQueue;

		SharedPtr<Entity> playerEntity;

		SharedPtr<AudioManager> audioManager;
		SharedPtr<PhysicsManager> physicsManager;
		SharedPtr<NetworkManager> networkManager;

		bool isServer;
	};

}