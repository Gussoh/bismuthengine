/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file Entity.h
 */

#pragma once

#include "IComponent.h"

namespace Bismuth {

	/**
	 * The basic entity class, represent an object in the game world.
	 * Every entity consists of a number of components.
	 */
	class Entity {
	public:
		Entity();
		virtual ~Entity();

		/**
		 * Adds a component
		 * @param component Component to add
		 * @throw std::logic_error If the component is already attached
		 */
		void addComponent(SharedPtr<IComponent> component);

		/**
		 * Removes a component
		 * @param component Component to remove
		 */
		void removeComponent(SharedPtr<IComponent> component);

		/**
		 * Checks if a specific component is attached
		 * @param component Component to check for
		 * @return True if the component is attached, false otherwise
		 */
		bool hasComponent(SharedPtr<IComponent> component) const;

		/**
		 * Activates the entity when it's added to the entity manager
		 */
		void activate();

		/**
		 * Deactivates the entity when it's removed from the entity manager
		 */
		void deactivate();

		/**
		 * Set the unique identifier
		 */
		void setId(int id) { this->id = id; }

		/**
		 * Get the unique identifier
		 */
		int getId() const { return id; }

	private:
		int id;

		typedef std::vector<SharedPtr<IComponent> > IComponentList; 
		IComponentList components;
	};

}