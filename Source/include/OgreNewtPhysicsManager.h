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
#include <vector>
#include <map>

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

			virtual SharedPtr<Entity> getFirstEntityAlongRay(const Ogre::Vector3 &startPoint, const Ogre::Vector3 &endPoint);
		private:
			
			OgreNewt::Body* createBodyForEntity(SharedPtr<Entity> &entity);

			OgreNewt::Body* createStaticBody(SharedPtr<Entity> &entity);

			OgreNewt::Body* createDynamicBody(SharedPtr<Entity> &entity);

			OgreNewt::Body* createPlayerBody(SharedPtr<Entity> &entity);

			float calcMass(EntityMaterial material, float volume);

			float getElasticityValue(EntityMaterial material1, EntityMaterial material2);

			void dynamicBodyForceCallback(OgreNewt::Body *body);

			void playerBodyForceCallback(OgreNewt::Body *body);

			GameLogic *gameLogic;
			OgreNewt::World *world;
			OgreNewt::BasicFrameListener *frameListener;
			typedef std::map<int, OgreNewt::Body*> IdToBodyMap;
			IdToBodyMap idToBodyMap;

			typedef std::vector<OgreNewt::BasicJoints::UpVector*> UpVectorList;
			UpVectorList upVectors;

			typedef std::set<int> CollisionHashSet;
			CollisionHashSet collisionHashSet;

			typedef std::map<int, Ogre::Vector3> IdToImpulseMap;
			IdToImpulseMap idToImpulseMap;

			OgreNewt::MaterialPair *defaultMaterialPair;

		};
	}
}
