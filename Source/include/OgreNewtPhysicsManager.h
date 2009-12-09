/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file OgreNewtPhysicsManager.h
 */

#pragma once

#include "PhysicsManager.h"
#include "GameLogic.h"
#include "OgreNewt.h"

namespace Bismuth {
	namespace Physics {
		

		/**
		 * OgreNewtPhysicsManager class
		 */
		class OgreNewtPhysicsManager : public PhysicsManager, public OgreNewt::ContactCallback {
		public:
			OgreNewtPhysicsManager(GameLogic *gameLogic);
			virtual ~OgreNewtPhysicsManager();

			virtual void getNearbyEntities(float radius, SharedPtr<Entity> source, std::vector<SharedPtr<Entity> > &entityList);

			virtual void removeEntity(SharedPtr<Entity> &entity);

			virtual void removeAllEntities();

			virtual void update(float stepTime);

			virtual void addImpulse(SharedPtr<Entity> &entity, Ogre::Vector3 &direction);

			virtual int userProcess();
		private:
			
			OgreNewt::Body* createBodyForEntity(SharedPtr<Entity> &entity);

			OgreNewt::Body* createStaticBody(SharedPtr<Entity> &entity);

			OgreNewt::Body* createDynamicBody(SharedPtr<Entity> &entity);

			OgreNewt::Body* createPlayerBody(SharedPtr<Entity> &entity);

			float calcMass(EntityMaterial material, float volume);

			GameLogic *gameLogic;
			OgreNewt::World *world;
			OgreNewt::BasicFrameListener *frameListener;
			typedef std::map<int, OgreNewt::Body*> IdToBodyMap;
			IdToBodyMap idToBodyMap;

			OgreNewt::MaterialPair *defaultMaterialPair;

		};
	}
}
