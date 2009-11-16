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

namespace Bismuth {

	/**
	 * IStream class
	 */
	class IStream {
	public:
		IStream() { } // Implementation needed when linking
		virtual ~IStream() { }  // Implementation needed when linking

		virtual IStream* write(Ogre::Vector3 v) { return this; };
		virtual IStream* write(Ogre::Quaternion v) { return this; };
		virtual IStream* write(int v) { return this; };

		virtual Ogre::Vector3 readVector3() { return Ogre::Vector3::ZERO; }
		virtual Ogre::Quaternion readQuaternion() { return Ogre::Quaternion::IDENTITY; }
 		virtual int readInt() { return 0; }

	private:
	};

}