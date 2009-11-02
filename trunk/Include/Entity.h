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
#include "PropertySet.h"

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
		 * Check if the entity has a specific property
		 * @param id Id of the property to check for
		 * @return True if the property exists
		 */
		bool hasProperty(const PropertyID &id) const;

		/**
		 * Add a property, if the property already exists then nothing happens
		 * @param id Id of the property to ad
		 * @param defaultValue Default value to set if the property does not exist
		 */
		void addProperty(const PropertyID &id, Any defaultValue);

		/**
		 * Set a property, the property will be added if it does not exists
		 * @param id Id of the property
		 * @param value Value to set
		 */
		void setProperty(const PropertyID &id, Any value);

		/**
		 * Get the value of a property
		 * @param id Id of the property to set
		 * @return Value of the property
		 * @throw std::logic_error If the property does not exist
		 */
		Any getProperty(const PropertyID &id);

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

		PropertySet propertySet;
	};

}