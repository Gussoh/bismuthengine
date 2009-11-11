/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file AudioManager.h
 */

#pragma once

#include <vector>
#include "Entity.h"
#include "AudioProperties.h"

namespace Bismuth {
	namespace Audio {

		/**
		 * BismuthAudio class
		 */
		class AudioManager {
		public:
			
			/** 
			* Plays a sound.
			*
			* @param entity The entity from which the sound comes from.
			*/
			virtual void playSound(const Entity &entity, const AudioProperties &properties) = 0;
		};

}
}