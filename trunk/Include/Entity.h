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
		 */
		void addComponent(SharedPtr<IComponent> component);

		/**
		 * Removesw a component
		 * @param component Component to remove
		 */
		void removeComponent(SharedPtr<IComponent> component);

		void setId(int id) { this->id = id; }
		int getId() const { return id; }

	private:
		int id;

		typedef std::vector<SharedPtr<IComponent> > IComponentList; 
		IComponentList components;
	};

}