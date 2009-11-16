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
#include "GameLogic.h"
#include "OgreNewt.h"

namespace Bismuth {
	namespace Physics {
		/**
		 * NewtonPhysicsManager class
		 */
		class NewtonPhysicsManager : public PhysicsManager {
		public:
			NewtonPhysicsManager(GameLogic *gameLogic);
			virtual ~NewtonPhysicsManager();

			virtual void getNearbyEntities(float radius, SharedPtr<Entity> source, std::vector<SharedPtr<Entity> > &entityList);

			virtual void addEntity(SharedPtr<Entity> entity);

			virtual void removeEntity(SharedPtr<Entity> entity);

			virtual void removeAllEntities();

		private:
			GameLogic *gameLogic;
			OgreNewt::World *world;
		};
	}
}