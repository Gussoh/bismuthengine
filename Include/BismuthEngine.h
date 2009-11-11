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

namespace Bismuth {

	using Bismuth::Audio::AudioManager;
	/**
	 * Template class
	 */
	class BismuthEngine {
	public:
		BismuthEngine();
		virtual ~BismuthEngine();

		void run();
		SharedPtr<Audio::AudioManager> getAudioManager() { return audioManager; }

	private:
		SharedPtr<Audio::AudioManager> audioManager;
	};
}