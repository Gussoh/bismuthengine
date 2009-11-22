/**
 * @file Renderer.cpp
 */

#include "stdafx.h"
#include "Renderer.h"
#include "OgreRoot.h"
#include "OgreRenderSystem.h"
#include "OgreRenderWindow.h"
#include "OgreCamera.h"
#include "OgreViewport.h"
#include "OgreSceneManager.h"
#include "OgreColourValue.h"
#include "OgreWindowEventUtilities.h"
#include "OgreResourceGroupManager.h"

using namespace Bismuth;
using namespace Bismuth::Graphics;
using namespace Ogre;

Renderer::Renderer() : alive(false) {

}

Renderer::~Renderer() {

}

void Renderer::init(int width, int height, bool fullscreen) {
	if (alive)
	{
		throw std::logic_error("renderer is already initialised");
	}

	Root *root = Root::getSingletonPtr();

	// Load the render system
#ifdef _DEBUG
	root->loadPlugin("RenderSystem_Direct3D9_d");
#else
	root->loadPlugin("RenderSystem_Direct3D9");
#endif

	// Select the d3d render system
	root->setRenderSystem(root->getRenderSystemByName("Direct3D9 Rendering Subsystem"));
	RenderSystem *rs = root->getRenderSystem();

	rs->setConfigOption("Full Screen", StringConverter::toString(fullscreen));
	rs->setConfigOption("Video Mode", StringConverter::toString(width) + " x " + StringConverter::toString(height) + " @ 32-bit colour");

	root->initialise(true, "BismuthEngine");

	RenderWindow *window = root->getAutoCreatedWindow();

	alive = true;

	// Select a default scene manager
	sceneManager = root->createSceneManager(ST_EXTERIOR_CLOSE, "SceneManager");
	sceneManager->setAmbientLight(ColourValue(0.6f, 0.6f, 0.6f));

	// Create the default camera
	camera = sceneManager->createCamera("defaultCamera");
	camera->setNearClipDistance(0.1f);
	camera->setFarClipDistance(512.0f);
	camera->setFOVy(Degree(65.0f));
	camera->setAspectRatio(width / (float)height);

	// And a viewport
	Viewport *vp = window->addViewport(camera);
	vp->setBackgroundColour(ColourValue(0.2f, 0.4f, 0.3f, 1.0f));
}

void Renderer::kill() {
	if (!alive)
	{
		throw std::logic_error("renderer is not initialised");
	}
}

bool Renderer::isWindowOpen() const {
	return alive && !Root::getSingleton().getAutoCreatedWindow()->isClosed();
}

void Renderer::render() {
	Root::getSingleton().renderOneFrame();
	WindowEventUtilities::messagePump();
}