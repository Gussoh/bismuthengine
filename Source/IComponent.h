/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file IComponent.h
 */

#pragma once

namespace Bismuth {

	class Entity;

	/**
	 * IComponent class
	 */
	class IComponent {
	public:
		IComponent() { }
		virtual ~IComponent() {}

		/**
		 * Activtes the component when the entity is activated
		 * @param owner New owner of the component
		 */
		virtual void activate() = 0;

		/**
		 * Deactivates the component when the entity is deactivated
		 */
		virtual void deactivate() = 0;

		/**
		 * Called when the component is attached to an entity
		 */
		virtual void attached(Entity *owner) { setOwner(owner); };

		/**
		 * Called when the component is detached from an entity
		 */
		virtual void detached() = 0;

	protected:
		Entity *getOwner() { return owner; }

	private:
		void setOwner(Entity *owner) { this->owner = owner; }

		Entity *owner;
	};

}