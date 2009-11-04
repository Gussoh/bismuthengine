/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file Template.h
 */

#pragma once

#include "IComponent.h"

namespace Bismuth {
	class Entity;
}

namespace BismuthAudio {

	/**
	 * Template class
	 */
	class AudioComponent : public Bismuth::IComponent {
	public:
		AudioComponent();
		virtual ~AudioComponent();
		/**
		 * Activtes the component when the entity is activated
		 */
		virtual void activate();

		/**
		 * Deactivates the component when the entity is deactivated
		 */
		virtual void deactivate();

		/**
		 * Called when the component is attached to an entity
		 */
		virtual void attached(Bismuth::Entity *owner);

		/**
		 * Called when the component is detached from an entity
		 */
		virtual void detached();
	};

}