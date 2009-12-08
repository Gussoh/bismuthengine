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
#include "Renderer.h"
#include "InputManager.h"
#include "Entity.h"
#include <string>

namespace Bismuth {

	typedef std::map<int, SharedPtr<Entity> > EntityList;

	/**
	 * GameLogic class
	 *
	 * Override handleMessage in a sub class to create your own game logic.
	 * The server game logic should also dispatch all messages to the clients.
	 */
	class GameLogic {
	public:
		GameLogic(int numberOfPlayers);
		GameLogic(std::string host);

		virtual ~GameLogic();

		/**
		 * Initialise resource locations
		 */
		virtual void initResourceLocations();
		virtual void guiTest();

		/**
		 * Loads the game world
		 * \name Name of the game world to load, could be loaded from file / memory 
		 * or created directly by code.
		 */
		virtual void loadWorld(const std::string &name);

		/**
		 * Ticks the game logic once, handles all queued messages
		 */
		void update();

		/**
		 * Renders one frame
		 */
		void render();

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

		/**
		* Creates a new entity and adds it to the game logic.
		*/
		SharedPtr<Entity> createEntity();

		/**
		* Creates a new entity and a new sceneNode with it. 
		* A mesh (Ogre::Entity) will also be created for the sceneNode.
		*/
		SharedPtr<Entity> createEntity(const Ogre::String &meshName);

		Audio::AudioManager* getAudioManager() const		{ return audioManager; }

		Physics::PhysicsManager* getPhysicsManager() const	{ return physicsManager; }

		Network::NetworkManager* getNetworkManager() const	{ return networkManager; }

		Graphics::Renderer* getRenderer() const				{ return renderer; }

		Input::InputManager* getInputManager() const { return inputManager; }

		/**
		 * Get the current player entity
		 * \return The current player entity, null if one has not been assigned
		 */
		SharedPtr<Entity> getPlayerEntity() const { return playerEntity; };
		void setPlayerEntity(SharedPtr<Entity> &entity) { playerEntity = entity; }

		EntityList* getEntities() { return &entities; };

		SharedPtr<Entity> getCameraEntity() const { return cameraEntity; }

		void setCameraEntity(SharedPtr<Entity> &entity);

		bool isGameStarted();

	protected:
		

		/**
		 * Handle a message
		 * \param message A pointer to the message to handle
		 */
		void handleMessage(SharedPtr<Message> message);

		// Message handlers
		void handleDebugOutMessage(SharedPtr<Message> message);
		void handleEntityAssignedMessage(SharedPtr<Message> message);
		void handleEndOfFrameMessage(SharedPtr<Message> message); 
		void handleCollisionMessage(SharedPtr<Message> message);
		void handlePlayerMoveMessage(SharedPtr<Message> message);
		void handlePlayerRotateMessage(SharedPtr<Message> message);
		void handlePressButtonMessage(SharedPtr<Message> message);
		void handleCreateEntityMessage(SharedPtr<Message> message);

	private:
		EntityList entities;
		std::queue<SharedPtr<Message> > messageQueue;

		SharedPtr<Entity> playerEntity;
		SharedPtr<Entity> cameraEntity;

		Audio::AudioManager *audioManager;
		Physics::PhysicsManager *physicsManager;
		Network::NetworkManager *networkManager;
		Graphics::Renderer *renderer;
		Input::InputManager *inputManager;
		int nextEntityId;
		bool isServer, gameStarted;
		clock_t lastUpdate;
		int numberOfPlayers;

		void initialize();
	};

}
