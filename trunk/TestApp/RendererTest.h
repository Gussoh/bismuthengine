#pragma once

#include "Renderer.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreSceneNode.h"

using namespace Bismuth;
using namespace Bismuth::Graphics;

class RendererTest : public Test {
public:

	virtual void run() {
		std::cout << "Renderer test" << std::endl;

		Renderer renderer;
		renderer.init(800, 600, false);

		Ogre::Entity *mesh = renderer.getDefaultSceneManager()->createEntity("stuffMesh", "Models/Room.mesh");
		Ogre::SceneNode *node = renderer.getDefaultSceneManager()->getRootSceneNode()->createChildSceneNode("stuff");
		node->attachObject(mesh);
		node->setPosition(0.0f, -1.0f, 0.0f);
		node->setOrientation(Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3(0, 1, 0)));

		while (renderer.isWindowOpen()) {
			// Nothing to doooo
			renderer.render();
		}

		renderer.kill();
	}
};
