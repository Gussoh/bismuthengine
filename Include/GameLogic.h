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
#include "GUI.h"
#include "QuickGUI.h"
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

		/**
		 * Init the game logic
		 */
		void initialize();

		/**
		 * Loads the game world
		 * \name Name of the game world to load, could be loaded from file / memory 
		 * or created directly by code.
		 */
		virtual void loadWorld(const std::string &name);

		/**
		 * Ticks the game logic once, handles all queued messages
		 */
		virtual void update();

		/**
		 * Refresh the user interface
		 */
		virtual void updateGui();

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
		 * Removes an entity
		 * @param entity Entity to remove
		 */
		void removeEntity(SharedPtr<Entity> entity);

		/**
		 * Get an entity by id
		 * \param id Id of the entity to lookup
		 * \return The entity with the specified id, null if it does not exist
		 */
		SharedPtr<Entity> getEntityById(int id);

		/**
		* Creates a new entity and adds it to the game logic.
		*/
		SharedPtr<Entity> createEntity(EntityType type);

		/**
		* Creates a new entity and a new sceneNode with it. 
		* A mesh (Ogre::Entity) will also be created for the sceneNode.
		*/
		SharedPtr<Entity> createEntity(EntityType type, const Ogre::String &meshName);

		/**
		 * Get the audio manager
		 */
		Audio::AudioManager* getAudioManager() const		{ return audioManager; }

		/**
		 * Get the physics manager
		 */
		Physics::PhysicsManager* getPhysicsManager() const	{ return physicsManager; }

		/**
		 * Get the network manager
		 */
		Network::NetworkManager* getNetworkManager() const	{ return networkManager; }

		/**
		 * Get the renderer
		 */
		Graphics::Renderer* getRenderer() const				{ return renderer; }

		/**
		 * Get the inut manager
		 */
		Input::InputManager* getInputManager() const { return inputManager; }

		/**
		 * Get the current player entity
		 * @return The current player entity, null if one has not been assigned
		 */
		SharedPtr<Entity> getPlayerEntity() const { return playerEntity; };

		/**
		 * Set the current player entity
		 * @param entity Entity to become the current player entity
		 */
		void setPlayerEntity(SharedPtr<Entity> &entity) { playerEntity = entity; }

		/**
		 * Get a list containing all entities
		 */
		EntityList* getEntities() { return &entities; };

		/**
		 * Get the camera entity
		 */
		SharedPtr<Entity> getCameraEntity() const { return cameraEntity; }

		/**
		 * Set the current camera entity
		 * @param entity Entity to become the current camera entity
		 */
		void setCameraEntity(SharedPtr<Entity> &entity);

		/**
		 * Get a value indicatin wheter the game has started or not
		 * @return True if the game has started, false otherwise
		 */
		bool isGameStarted();

		/**
		 * Get the current number of players
		 */
		int getNumberOfPlayers();

		/**
		 * Get the entity, if any, at the specified screen coordinates
		 * @param x x coordinate
		 * @param y y coordinate
		 * @param maxDistance Maximum distance of the entity
		 * @return An entity or null if there was no entity at the cooridnates
		 */
		SharedPtr<Entity> GetEntityAtScreenPosition(float x, float y, float maxDistance);

		/**
		 * Add a message on the local message queue, these message are not replicated to other clients
		 * @param message Message to enqueue
		 * @remarks Message that affect the state of the game world should not be enqueued using this function
		 * as the state changes will not be replicated over the network.
		 */
		void addLocalMessage(SharedPtr<Message> message);

		int getMyPlayerId() { return myPlayerId; }

		int getFrameCounter() { return frameCounter; }

	protected:
		virtual void initGui();

		/**
		 * Handles a message by forwarding the message to a message handler function.
		 * \param message A pointer to the message to handle
		 */
		virtual void handleMessage(SharedPtr<Message> message) = 0;

		// Pre-defined message handlers

		/**
		 * Handles messages of type DebugOutMessage.
		 * Prints the message to stdout.
		 */
		virtual void handleDebugOutMessage(SharedPtr<Message> message);

		/**
		 * Handles messages of type EntityAssignedMessage.
		 * Sets the PlayerEntity and the CameraEntity to the entity specified in the message.
		 */
		virtual void handleEntityAssignedMessage(SharedPtr<Message> message);

		/**
		 * Handles messages of type EndOfFrameMessage.
		 * Updates the state of the game engine.
		 * It will also call handleMessage for all messages in the local message queue.
		 */
		virtual void handleEndOfFrameMessage(SharedPtr<Message> message); 
		
		/**
		 * Handles messages of type MoveEntityMessage.
		 * Adds an impulse to an entity.
		 */
		virtual void handleMoveEntityMessage(SharedPtr<Message> message);

		/**
		 * Handles messages of type RotateEntityMessage.
		 * Rotates an entity.
		 */
		virtual void handleRotateEntityMessage(SharedPtr<Message> message);
	
		virtual SharedPtr<Entity> handleCreateEntityMessage(SharedPtr<Message> message);
		virtual void handleStartGameMessage(SharedPtr<Message> message);

		/**
		 * Handles messages of type CollisionMessage
		 * Plays the collision sounds of the involved entities.
		 */
		virtual void handleCollisionMessage(SharedPtr<Message> message);

		virtual void handleIncomingConnectionMessage(SharedPtr<Message> message);
		virtual void handlePlayerIdAssignedMessage(SharedPtr<Message> message);

		// Client method
		virtual bool shouldSendMessage(MessageType msgType);

		

		void addTimedAnimation(int time, Ogre::SceneNode *node);

	private:
		EntityList entities;

		// Messages which should not be sent over the network, 
		// but need to be handled in a consistent way by all clients and servers alike.
		std::queue<SharedPtr<Message> > localMessageQueue; 

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
		int playerIdCounter;
		int myPlayerId;
		int frameCounter;
		
		
		
		std::string host;

		struct TimedAnimation {
			int time;
			Ogre::SceneNode *sceneNode;
		};

		typedef std::vector<SharedPtr<TimedAnimation> > TimedAnimationList;
		TimedAnimationList timedAnimations;

		
	
	};

	extern Physics::PhysicsManager *createPhysicsManager(GameLogic *gameLogic);
	extern Audio::AudioManager *createAudioManager(GameLogic *gameLogic);
	extern Network::NetworkManager *createNetworkManager(GameLogic *gameLogic);
	extern Input::InputManager *creatInputManager(int windowHandle, int width, int height);


}
