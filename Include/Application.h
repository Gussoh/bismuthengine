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

#include <string>

namespace Bismuth {
	class GameLogic;

	/**
	 * Application class
	 * Inherit and instanciate to create a game!
	 */
	class Application {
	public:
		Application(bool isServer, std::string host = "", int numberOfPlayers = 0);
		virtual ~Application();
	
		/**
		 * Starts the application
		 */
		virtual void run();

		/**
		 * Called right before the main loop is entered
		 */
		virtual void initGame();

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
		virtual void kill();

		/**
		 * Get the current game logic instance
		 */
		GameLogic* getGameLogic() {
			return gameLogic;
		}

		bool getIsServer() const { return isServer; }

	protected:
		virtual GameLogic *createGameLogic(bool isServer);

	private:
		GameLogic *gameLogic;
		bool running;
		bool isServer;
		std::string host;
		int numberOfPlayers;

	};

}