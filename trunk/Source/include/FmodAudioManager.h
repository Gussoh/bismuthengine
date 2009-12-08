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
#include "GameLogic.h"
#include "fmod.hpp"

namespace Bismuth {
	namespace Audio {
		/**
		 * FmodAudioManager class
		 */
		class FmodAudioManager : public AudioManager {
		public:
			FmodAudioManager(GameLogic *gameLogic);
			virtual ~FmodAudioManager();

			virtual void update();

			virtual void playSound(SharedPtr<Entity> &entity);
			virtual void playSoundtrack();
			// initialize/update listener, maybe do this in update()
			// updateListener, use System::set3DListenerAttributes
			virtual void updateListener();

			virtual FMOD::Sound *createSound(const std::string &filename, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exInfo);

		private:
			GameLogic *gameLogic;
			FMOD::System *fmodSystem;
			FMOD_VECTOR ogreToFmodVector(Ogre::Vector3 ogreVector);

			typedef std::map<std::string, FMOD::Sound*> SoundCache;
			SoundCache soundCache;
		};
	}
}