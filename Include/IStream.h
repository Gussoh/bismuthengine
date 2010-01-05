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
	 * Interface for an arbitrary stream
	 */
	class IStream {
	public:
		IStream() { } // Implementation needed when linking
		virtual ~IStream() { }  // Implementation needed when linking

		/**
		 * Write a 3d vector to the stream
		 */
		virtual IStream* write(Ogre::Vector3 v) = 0;

		/**
		 * Write a quaternion to the stream
		 */
		virtual IStream* write(Ogre::Quaternion v) = 0;

		/**
		 * Write an integer to the stream
		 */
		virtual IStream* write(int v) = 0;

		/**
		 * Write a single character to the stream
		 */
		virtual IStream* write(char c) = 0;

		/**
		 * Write a float to the stream
		 */
		virtual IStream* write(float f) = 0;

		/**
		 * Write a string to the stream
		 */
		virtual IStream* write(std::string s) = 0;

		/**
		 * Read a 3d vector from the stream
		 */
		virtual Ogre::Vector3 readVector3() = 0;

		/**
		 * Read a quaternion from the stream
		 */
		virtual Ogre::Quaternion readQuaternion() = 0;

		/**
		 * Read an integer from the stream
		 */
 		virtual int readInt() = 0;

		/**
		 * Read a single character from the stream
		 */
		virtual char readChar() = 0;

		/**
		 * Read a float from the stream
		 */
		virtual float readFloat() = 0;

		/**
		 * Read a string from the stream
		 */
		virtual std::string readString() = 0;

	private:
	};

}