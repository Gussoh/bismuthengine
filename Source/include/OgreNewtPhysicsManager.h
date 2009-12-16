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

			virtual SharedPtr<Entity> getFirstEntityAlongRay(const Ogre::Vector3 &startPoint, const Ogre::Vector3 &endPoint);

			virtual void explode(SharedPtr<Entity> origin, float force);

			virtual void setForce(SharedPtr<Entity> entity, Ogre::Vector3 force);

			virtual float getMass(SharedPtr<Entity> entity);

		protected:
			virtual int userProcess();
			
		private:
			
			OgreNewt::Body* createBodyForEntity(SharedPtr<Entity> &entity);

			OgreNewt::Body* createStaticBody(SharedPtr<Entity> &entity);

			OgreNewt::Body* createDynamicBody(SharedPtr<Entity> &entity, EntityType type);

			OgreNewt::Body* createPlayerBody(SharedPtr<Entity> &entity);

			float calcMass(EntityMaterial material, float volume);

			float getElasticityValue(EntityMaterial material1, EntityMaterial material2);

			void dynamicBodyForceCallback(OgreNewt::Body *body);

			void playerBodyForceCallback(OgreNewt::Body *body);

			void shotForceCallback(OgreNewt::Body *body);

			GameLogic *gameLogic;
			OgreNewt::World *world;
			OgreNewt::BasicFrameListener *frameListener;
			OgreNewt::MaterialPair *defaultMaterialPair;

			typedef std::map<int, OgreNewt::Body*> IdToBodyMap;
			IdToBodyMap idToBodyMap;

			typedef std::map<int, OgreNewt::BasicJoints::UpVector*> UpVectorMap;
			UpVectorMap upVectorMap;

			typedef std::set<int> CollisionHashSet;
			CollisionHashSet collisionHashSet;

			typedef std::map<int, Ogre::Vector3> IdToVectorMap;
			IdToVectorMap idToImpulseMap;
			IdToVectorMap idToForceMap;

			typedef std::set<int*> UserDataSet;
			UserDataSet userDataSet;

		};
	}
}
