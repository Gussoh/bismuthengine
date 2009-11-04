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

#include "AudioManager.h"
#include "NetworkManager.h"

namespace Bismuth {

	/**
	 * Template class
	 */
	class BismuthEngine {
	private:
		BismuthNetwork::NetworkManager *networkManager;
		BismuthAudio::AudioManager *audioManager;
	public:
		BismuthEngine();
		virtual ~BismuthEngine();

		BismuthNetwork::NetworkManager* getNetworkManager() {
			return networkManager;
		}

		BismuthAudio::AudioManager* getAudioManager() {
			return audioManager;
		}
	};


}