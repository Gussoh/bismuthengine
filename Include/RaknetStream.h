/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file RaknetStream.h
 */

#pragma once

#include "IStream.h"

namespace Bismuth {

	/**
	 * RaknetStream class
	 */
	class RaknetStream : public IStream {
	public:
		RaknetStream();
		virtual ~RaknetStream();

		// TODO: use bitstream here
		virtual IStream* write(Ogre::Vector3 v) { return this; };
		virtual IStream* write(Ogre::Quaternion v) { return this; };
		virtual IStream* write(int v) { return this; };

		// TODO: use bitstream here
		virtual Ogre::Vector3 readVector3() { return Ogre::Vector3::ZERO; }
		virtual Ogre::Quaternion readQuaternion() { return Ogre::Quaternion::IDENTITY; }
		virtual int readInt() { return 0; }

	private:
	};

}