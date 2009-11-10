/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file Renderer.h
 */

#pragma once

namespace Ogre {
	class Camera;
}

namespace Bismuth {
namespace Graphics {

	/**
	 * Renderer class
	 */
	class Renderer {
	public:
		Renderer();
		virtual ~Renderer();

		/**
		 * Initalise the renderer
		 * @param widht Width of the window
		 * @param height Height of the window
		 * @param fullscreen Set to true to run in fullscreen mode
		 * @throw std::logic_error If the renderer has already been initialised
		 */
		void init(int width, int height, bool fullscreen);

		/**
		 * Kills the renderer
		 * @throw std::logic_error If the renderer has not been initliased
		 */
		void kill();

		int getWindowHandle();

		Ogre::Camera *getDefaultCamera();

		bool isWindowOpen() const;

		void messagePump();

	private:
		bool alive;
		Ogre::Camera *camera;

	};

}
}