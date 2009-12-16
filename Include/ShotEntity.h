/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file Template.h
 */

#pragma once

#include "Entity.h"

namespace Bismuth {

	/**
	 * Template class
	 */
	class ShotEntity : public Entity {
	public:
		ShotEntity() { }
		virtual ~ShotEntity() { }

		void setPlayer(int player) { this->player = player; }
		int getPlayer() { return player; }

		void setWeapon(int weapon) { this->weapon = weapon; }
		int getWeapon() { return weapon; }

	private:
		int player;
		int weapon;
	};

}