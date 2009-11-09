/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file FmodAudioManager.h
 */

#pragma once

#include "AudioManager.h"
#include "fmod.hpp"

namespace Bismuth {
	namespace Audio {
		/**
		 * FmodAudioManager class
		 */
		class FmodAudioManager : public AudioManager {
		public:
			FmodAudioManager();
			virtual ~FmodAudioManager();

			/** 
			* Plays sound
			*/
			virtual void playSound(Entity &entity, AudioProperties &properties);


		private:
			FMOD::System *fmodSystem;
		};
	}
}