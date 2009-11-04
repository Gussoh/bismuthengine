/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file BismuthAudio.h
 */

#pragma once

#include <vector>
#include "Entity.h"
#include "AudioComponent.h"

namespace BismuthAudio {

	struct AudioProperties {
		float reverberationTime; 
		bool isOcclusionObject;
		// directivityFunction
	};

	/**
	 * BismuthAudio class
	 */
	class AudioManager {
	public:
		AudioManager();
		virtual ~AudioManager();
		
		/**
		* Creates a new AudioComponent associated with the specified entity.
		* 
		* @param entity The entity which the component should be associated to.
		* @return A new AudioComponent.
		*/
		AudioComponent* createComponent(Bismuth::Entity &entity);

		int test();
	private:
		std::vector<AudioComponent*> audioComponents;
	};

}