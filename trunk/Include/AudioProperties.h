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
			SoundType_Continuous,
			SoundType_Create,
			SoundType_Destroy,
			SoundType_Hurt,
			SoundType_MajorHurt,
			SoundType_MinorHurt
		};

		typedef std::map<SoundType, std::string> SoundMap;

		/**
		 * AudioProperties class
		 */
		struct AudioProperties {
		public:
			AudioProperties() : loop(false), soundType(SoundType_Default), directivity(0), directivityOrientation(Ogre::Vector3::ZERO) {
			}

			bool loop;
			SoundType soundType;
			float directivity; // 0 for omnidirectional
			Ogre::Vector3 directivityOrientation;
			float collisionSpeed;
			SoundMap sounds;
		};
			
	}
}