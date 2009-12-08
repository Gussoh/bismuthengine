/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file RakNetStream.h
 */

#pragma once

#include "IStream.h"
#include "BitStream.h"
#include "RakNetTypes.h"

namespace Bismuth {

	/**
	 * RaknetStream class
	 */
	class RakNetStream : public IStream {
	public:
		RakNetStream() {}
		RakNetStream(Packet *p) : bitStream(p->data, p->length, false) {}

		virtual ~RakNetStream() {
		}

		virtual IStream* write(Ogre::Vector3 v) { 
			bitStream.Write(v);
			return this; 
		};
		virtual IStream* write(Ogre::Quaternion v) { 
			bitStream.Write(v);
			return this; 
		};
		virtual IStream* write(int v) { 
			bitStream.Write(v);
			return this; 
		};

		virtual IStream* write(MessageID id) {
			bitStream.Write(id);
			return this;
		}

		virtual IStream* write(char c) {
			bitStream.Write(c);
			return this;
		}

		virtual IStream* write(float f) {
			bitStream.Write(f);
			return this;
		}

		virtual IStream* write(std::string s) {
			bitStream.Write(s);
			return this;
		}

		virtual Ogre::Vector3 readVector3() { 
			Ogre::Vector3 v;
			bitStream.Read(v);
			return v;
		}
		virtual Ogre::Quaternion readQuaternion() { 
			Ogre::Quaternion v;
			bitStream.Read(v);
			return v; 
		}
		virtual int readInt() { 
			int v;
			bitStream.Read(v);
			return v; 
		}
		virtual char readChar() { 
			char v;
			bitStream.Read(v);
			return v; 
		}
		virtual float readFloat() { 
			float v;
			bitStream.Read(v);
			return v; 
		}

		virtual std::string readString() {
			std::string s;
			bitStream.Read(s);
			return s;
		}

		virtual RakNet::BitStream* getRakNetBitStream() {
			return &bitStream;
		}

	private:
		RakNet::BitStream bitStream;
	};

}