/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file EntityFactory.h
 */

#pragma once

namespace Bismuth {
	class Entity;
	class GameLogic;

	typedef SharedPtr<Entity> (*EntityCreatorFunc)(GameLogic *gameLogic);

	/**
	 * EntityFactory class
	 */
	class EntityFactory {
	public:
		EntityFactory();
		virtual ~EntityFactory();

		/**
		 * Register a type of entity
		 * \param name Name of the entity type
		 * \param func Creator function
		 */
		void registerType(const std::string &name, EntityCreatorFunc func);

		/**
		 * Create a new entity
		 * \param gameLogic A pointer to the current game logic instance
		 * \param name Type name of the entity to create
		 * \return An entity of type 'name'
		 */
		SharedPtr<Entity> create(GameLogic *gameLogic, const std::string &name);

	private:
		typedef std::map<std::string, EntityCreatorFunc> TypeMap;
		TypeMap types;

	};

}