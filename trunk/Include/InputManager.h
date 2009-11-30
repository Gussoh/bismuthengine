/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file InputManager.h
 */

#pragma once

namespace Bismuth {
namespace Input {

	/**
	 * InputManager class
	 */
	class InputManager {
	public:
		/**
		 * Init the input system
		 * \param windowHandle A win32 window handle to the window to receive input from
		 * \param width Width of the window
		 * \param height Height of the window
		 */
		InputManager() {};
		virtual ~InputManager() {};

		/**
		 * Update the input manager
		 */
		virtual void update() = 0;

		/**
		 * Get the mouse position, relative to the previous frame
		 * \return The relative mouse position in x, y. Relative mouse wheel position in z
		 */
		virtual Ogre::Vector3 getRelativeMousePosition() = 0;

		/**
		 * Return a value indicating whether a certain key is pressed
		 * \return True if the key is pressed, false otherwise
		 */
		virtual bool isKeyDown(int keyCode) = 0;

	private:
	};

}

}