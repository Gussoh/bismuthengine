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

		int getNumberOfPlayers();

		/**
		 * Get the entity, if any, at the specified screen coordinates
		 * @param x x coordinate
		 * @param y y coordinate
		 * @param maxDistance Maximum distance of the entity
		 * @return An entity or null if there was no entity at the cooridnates
		 */
		SharedPtr<Entity> GetEntityAtScreenPosition(float x, float y, float maxDistance);

		void addSpecialMessage(SharedPtr<Message> message);

	protected:
		

		/**
		 * Handle a message
		 * \param message A pointer to the message to handle
		 */
		virtual void handleMessage(SharedPtr<Message> message);

		// Message handlers
		virtual void handleDebugOutMessage(SharedPtr<Message> message);
		virtual void handleEntityAssignedMessage(SharedPtr<Message> message);
		virtual void handleEndOfFrameMessage(SharedPtr<Message> message); 
		virtual void handleCollisionMessage(SharedPtr<Message> message);
		virtual void handleMoveEntityMessage(SharedPtr<Message> message);
		virtual void handleRotateEntityMessage(SharedPtr<Message> message);
		virtual void handlePressButtonMessage(SharedPtr<Message> message);
		virtual SharedPtr<Entity> handleCreateEntityMessage(SharedPtr<Message> message);
		virtual void handleStartGameMessage(SharedPtr<Message> message);
		virtual void handleIncomingConnectionMessage(SharedPtr<Message> message);
		virtual void handlePlayerIdAssignedMessage(SharedPtr<Message> message);
		virtual void handleFireMessage(SharedPtr<Message> message);

		// Client method
		virtual bool shouldSendMessage(MessageType msgType);

		virtual void handleCollision(SharedPtr<Entity> entity, float velocity);

		void addTimedAnimation(int time, Ogre::SceneNode *node);

	private:
		EntityList entities;

		// Messages which should not be sent over the network, 
		// but need to be handled in a consistent way by all clients and servers alike.
		std::queue<SharedPtr<Message> > specialMessageQueue; 

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
		int nextShotAllowed;
		void initialize();

		int health;
		int weapon;

		QuickGUI::Image* playerAvatar;
		QuickGUI::TextArea *textaHealth;

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
