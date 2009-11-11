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
		GameLogic();
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

		SharedPtr<Entity> getEntityById(int id);

		SharedPtr<AudioManager> getAudioManager()		{ return audioManager; }
		SharedPtr<PhysicsManager> getPhysicsManager()	{ return physicsManager; }
		SharedPtr<NetworkManager> getNetworkManager()	{ return networkManager; }

	protected:
		void handleMessage(SharedPtr<Message> message);

	private:
		std::vector<SharedPtr<Entity> > entities;
		std::queue<SharedPtr<Message> > messageQueue;

		SharedPtr<AudioManager> audioManager;
		SharedPtr<PhysicsManager> physicsManager;
		SharedPtr<NetworkManager> networkManager;
	};

}