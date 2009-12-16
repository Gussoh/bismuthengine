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
#include <OgreRay.h>

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
			* Updates position and orientation of all entities.
			* Entities which does not exist in the physics world will be automatically added, depending on the entity type.
			*/
			virtual void update(float stepTime) = 0;

			virtual void addImpulse(SharedPtr<Entity> &entity, Ogre::Vector3 &direction) = 0;

			/**
			 * Get the first entity along a ray
			 */
			virtual SharedPtr<Entity> getFirstEntityAlongRay(const Ogre::Vector3 &startPoint, const Ogre::Vector3 &endPoint) = 0;

			virtual void explode(SharedPtr<Entity> origin, float force) = 0;

			virtual void setForce(SharedPtr<Entity> entity, Ogre::Vector3 force) = 0;

			virtual float getMass(SharedPtr<Entity> entity) = 0;

			virtual float getVelocity(SharedPtr<Entity> entity) = 0;

		private:
		};
	}
}
