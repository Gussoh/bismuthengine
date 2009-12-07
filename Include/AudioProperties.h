/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file AudioProperties.h
 */

#pragma once

namespace Bismuth {
	namespace Audio {

		
		enum SoundType {
			SoundType_Default,
			SoundType_Collision,
			SoundType_Continuous
		};

		/**
		 * AudioProperties class
		 */
		struct AudioProperties {
			bool loop;
			SoundType soundType;
			float directivity; // 0 for omnidirectional
			Ogre::Vector3 directivityOrientation;
		};
			
	}
}