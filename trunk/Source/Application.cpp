/**
 * @file Application.cpp
 */

#include "stdafx.h"
#include "Application.h"
#include "GameLogic.h"
#include "Renderer.h"
#include "OgreTimer.h"

using namespace Bismuth;

Application::Application(bool isServer, std::string host, int numberOfPlayers) : running(false), gameLogic(0), isServer(isServer), host(host), numberOfPlayers(numberOfPlayers) {
	
}

Application::~Application() {

}

void Application::initGame() {

}

void Application::run() {
	gameLogic = createGameLogic();
	gameLogic->initialize();

	Graphics::Renderer *renderer = gameLogic->getRenderer();

	while (!gameLogic->isGameStarted()) {
		gameLogic->render();
	}

	initGame();

	Ogre::Timer timer;
	running = true;
	while (running) {
		float deltaTime = timer.getMilliseconds() * 0.001f;

		this->update(deltaTime);
		gameLogic->update();

		this->prerender();
		gameLogic->render();
		this->postrender();

		running = running && renderer->isWindowOpen();

		timer.reset();
	}
}

void Application::kill() {
	running = false;
}
