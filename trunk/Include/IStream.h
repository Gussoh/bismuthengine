/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file IStream.h
 */

#pragma once

#include <string>

namespace Bismuth {

	/**
	 * IStream class
	 */
	class IStream {
	public:
		IStream() { } // Implementation needed when linking
		virtual ~IStream() { }  // Implementation needed when linking

		virtual IStream* write(Ogre::Vector3 v) = 0;
		virtual IStream* write(Ogre::Quaternion v) = 0;
		virtual IStream* write(int v) = 0;
		virtual IStream* write(char c) = 0;
		virtual IStream* write(float f) = 0;
		virtual IStream* write(std::string s) = 0;

		virtual Ogre::Vector3 readVector3() = 0;
		virtual Ogre::Quaternion readQuaternion() = 0;
 		virtual int readInt() = 0;
		virtual char readChar() = 0;
		virtual float readFloat() = 0;
		virtual std::string readString() = 0;

	private:
	};

}