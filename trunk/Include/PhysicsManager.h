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
			/**
			* Finds all entities that within a given distance from an entity.
			*
			* @param radius The radius to use when finding nearby entities.
			* @param source The source entity.
			* @param[out] entityList Will be populated with entities that were found within the radius.
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
			 * Add a piece of static geometry to the world
			 * \param mesh A pointer to an ogre scene node containing the static meshes, the scene node can be destroyed when this method has been called
			 */
			virtual void addStaticGeometry(Ogre::SceneNode *mesh) = 0;

			/**
			 * Removes all static geometry from the world
			 */
			virtual void clearStaticGeometry() = 0;

		private:
		};
	}
}