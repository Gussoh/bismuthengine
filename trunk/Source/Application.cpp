/**
 * @file Application.cpp
 */

#include "stdafx.h"
#include "Application.h"
#include "GameLogic.h"
#include "Renderer.h"
#include "OgreTimer.h"

using namespace Bismuth;

Application::Application(bool isServer) : running(false), gameLogic(0), isServer(isServer) {
	
}

Application::~Application() {

}

void Application::run() {
	gameLogic = new GameLogic(isServer);
	Graphics::Renderer *renderer = gameLogic->getRenderer();

	Ogre::Timer timer;
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

	delete gameLogic;
}

void Application::kill() {
	running = false;
}