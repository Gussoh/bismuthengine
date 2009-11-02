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

namespace Bismuth {

	/**
	 * Entity class
	 */
	class Entity {
	public:
		Entity();
		virtual ~Entity();

		void setId(int id) { this->id = id; }
		int getId() const { return id; }

	private:
		int id;
	};

}