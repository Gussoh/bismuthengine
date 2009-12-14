/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file OISInputManager.h
 */

#pragma once

#include "InputManager.h"
#include "ois/OIS.h"

namespace Bismuth {
namespace Input {

	/**
	 * OISInputManager class
	 */
	class OISInputManager : public InputManager {
	public:
		OISInputManager(int windowHandle, int width, int height);
		virtual ~OISInputManager();

		virtual void update();

		virtual Ogre::Vector3 getRelativeMousePosition();

		virtual Ogre::Vector3 getMousePosition();

		virtual bool isKeyDown(KeyCode keyCode);

		virtual bool isMouseButtonDown(MouseButtonID buttonId);

	private:
		OIS::InputManager *inputManager;
		
		OIS::Keyboard *keyboard;
		OIS::Mouse *mouse;
	};

}

}