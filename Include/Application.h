/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file Application.h
 */

#pragma once

namespace Bismuth {
	class GameLogic;

	/**
	 * Application class
	 */
	class Application {
	public:
		Application(bool isServer);
		virtual ~Application();
	
		/**
		 * Starts the application
		 */
		void run();

		/**
		 * Called before update
		 * @param elapsedTime Time in seconds since last update
		 */
		virtual void update(float elapsedTime) {};

		/**
		 * Called before render
		 */
		virtual void prerender() {};

		/**
		 * Called after render
		 */
		virtual void postrender() {};

		/**
		 * Kill the application
		 */
		void kill();

		/**
		 * Get the current game logic instance
		 */
		GameLogic* getGameLogic() {
			return gameLogic;
		}

	private:
		GameLogic *gameLogic;
		bool running;
		bool isServer;

	};

}