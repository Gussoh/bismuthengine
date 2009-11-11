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

#include "PhysicsManager.h"

namespace Bismuth {
	namespace Physics {
		/**
		 * Template class
		 */
		class NewtonPhysicsManager : public PhysicsManager {
		public:
			NewtonPhysicsManager();
			virtual ~NewtonPhysicsManager();

			virtual void getNearbyEntities(float radius, SharedPtr<Entity> source, std::vector<SharedPtr<Entity> > &entityList);
		private:
		};
	}
}