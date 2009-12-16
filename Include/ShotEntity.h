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
		ShotEntity(int id) : Entity(id) { }
		virtual ~ShotEntity() { }

		void setPlayerEntity(int playerEntity) { this->playerEntity = playerEntity; }
		int getPlayerEnity() { return playerEntity; }

		void setWeapon(int weapon) { this->weapon = weapon; }
		int getWeapon() { return weapon; }

	private:
		int playerEntity;
		int weapon;
	};

}