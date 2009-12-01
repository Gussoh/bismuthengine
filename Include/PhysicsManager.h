/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file PhysicsManager.h
 */

#pragma once

#include "Entity.h"
#include <vector>

namespace Bismuth {

	namespace Physics {
		/**
		 * PhysicsManager class
		 */
		class PhysicsManager {
		public:

			virtual ~PhysicsManager() { };
			/**
			* Finds all entities that within a given distance from an entity.
			*
			* @param radius The radius to use when finding nearby entities.
			* @param source The source entity.
			* @param[out] entityList will be populated with entities that were found within the radius.
			*/
			virtual void getNearbyEntities(float radius, SharedPtr<Entity> source, std::vector<SharedPtr<Entity> > &entityList) = 0;

			/**
			* Adds an entity to the physics engine.
			*
			* @param entity The entity to add to the physics engine.
			*/
			virtual void addEntity(SharedPtr<Entity> &entity) = 0;

			/**
			* Removes an entity from the physics engine.
			*
			* @param entity The entity to remove from the physics engine.
			*/
			virtual void removeEntity(SharedPtr<Entity> &entity) = 0;

			/**
			* Removes all entities from the physics engine.
			*/
			virtual void removeAllEntities() = 0;
			
			/**
			* Updates position and orientation of all physics controlled entities.
			*/
			virtual void update() = 0;

			virtual void addImpulse(SharedPtr<Entity> &entity, Ogre::Vector3 &direction) = 0;

		private:
		};
	}
}
