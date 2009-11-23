/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file DebugLogic.h
 */

#pragma once

#include "NetworkManager.h"

namespace Bismuth {

	/**
	 * Template class
	 */
	class DebugLogic {
	public:
		DebugLogic();
		virtual ~DebugLogic();

		Bismuth::Network::NetworkManager* createNetworkManager();

	private:
	};

}