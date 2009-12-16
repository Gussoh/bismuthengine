/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file ShotEntity.h
 */

#pragma once

#include "Entity.h"

namespace Bismuth {

	/**
	 * ShotEntity class
	 */
	class ShotEntity : public Entity {
	public:
		ShotEntity(int id) : Entity(id), exploded(false) { }
		virtual ~ShotEntity() { }

		void setPlayerEntityId(int playerEntity) { this->playerEntity = playerEntity; }
		int getPlayerEntityId() { return playerEntity; }

		void setWeapon(int weapon) { this->weapon = weapon; }
		int getWeapon() { return weapon; }

		void setExploded(bool value) { this->exploded = value; }
		bool hasExploded() { return exploded; }

	private:
		int playerEntity;
		int weapon;
		bool exploded;
	};

}