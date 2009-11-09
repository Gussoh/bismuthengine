/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file GameLogic.h
 */

#pragma once

namespace Bismuth {

	class Entity;

	/**
	 * GameLogic class
	 */
	class GameLogic {
	public:
		GameLogic();
		virtual ~GameLogic();

		SharedPtr<Entity> getEntityById(int id);

	private:
		std::vector<SharedPtr<Entity>> entities;
	};

}