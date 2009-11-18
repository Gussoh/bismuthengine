/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file Entity.h
 */

#pragma once

#include "IStream.h"

namespace Bismuth {
	class GameLogic;

	/**
	 * The basic entity class, represent an object in the game world.
	 */
	class Entity {
	public:
		Entity(GameLogic *gameLogic);
		virtual ~Entity();

		/**
		 * Tick the entity once
		 * @param elapsedTime Time in seconds since last update
		 */
		void update(float elapsedTime);

		void setPosition(const Ogre::Vector3 &position) { this->position = position; }
		Ogre::Vector3 getPosition() const { return position; }

		void setOrientation(const Ogre::Quaternion &orientation) { this->orientation = orientation; }
		Ogre::Quaternion getOrientation() const { return orientation; }

		void setId(int id) { this->id = id; }
		int getId() const { return id; }

		void serialize(IStream *stream) const;
		void deserialize(IStream *stream);

		virtual std::string typeName() const { return "Entity"; }

	private:
		Ogre::Vector3 position;
		Ogre::Quaternion orientation;
		int id;
		GameLogic *gameLogic;

	};

}