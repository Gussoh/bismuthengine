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
		class OgreNewtPhysicsManager : public PhysicsManager {
		public:
			OgreNewtPhysicsManager(GameLogic *gameLogic);
			virtual ~OgreNewtPhysicsManager();

			virtual void getNearbyEntities(float radius, SharedPtr<Entity> source, std::vector<SharedPtr<Entity> > &entityList);

			virtual bool addEntity(SharedPtr<Entity> &entity);

			virtual void removeEntity(SharedPtr<Entity> &entity);

			virtual void removeAllEntities();

			
		private:
			
			OgreNewt::Body* createStaticBody(Ogre::SceneNode *sceneNode);

			OgreNewt::Body* createDynamicBody(Ogre::SceneNode *sceneNode);

			OgreNewt::Body* createPlayerBody();

			GameLogic *gameLogic;
			OgreNewt::World *world;
			OgreNewt::BasicFrameListener *frameListener;
			typedef std::map<int, OgreNewt::Body*> IdToBodyMap;
			IdToBodyMap idToBodyMap;

		};
	}
}
