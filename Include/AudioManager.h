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
			* Update the audio system
			*/
			virtual void update() = 0;

			/** 
			* Plays a sound.
			*
			* @param entity The entity from which the sound comes from.
			*/
			virtual void playSound(SharedPtr<Entity> &entity) = 0;

			/**
			 * Play a sound track
			 */
			virtual void playSoundtrack() = 0;

			/**
			 * Preload all sounds
			 */
			virtual void preloadSounds() = 0;

			/**
			 * Set the master volume for all sounds
			 */
			virtual void setMasterVolume(float volume) = 0;
			
			/**
			 * Get the current master volume for all sounds
			 */
			virtual float getMasterVolume() = 0;

		};



}
}