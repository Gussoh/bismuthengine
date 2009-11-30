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

		GameLogic gameLogic(false);
		gameLogic.loadWorld("test1");

		Renderer *renderer = gameLogic.getRenderer();
		Ogre::Camera *camera = renderer->getDefaultCamera();

		Ogre::Entity *mesh = renderer->getDefaultSceneManager()->createEntity("stuffMesh", "Models/Room.mesh");
		Ogre::Entity *mesh2 = renderer->getDefaultSceneManager()->createEntity("stuffMesh2", "Models/Room_SeeTrough.mesh");

		Ogre::SceneNode *node = renderer->getDefaultSceneManager()->getRootSceneNode()->createChildSceneNode("stuff");
		node->attachObject(mesh);
		node->attachObject(mesh2);

		SharedPtr<Entity> entity = gameLogic.createEntity();
		entity->setSceneNode(node);
		entity->setPosition(Ogre::Vector3(0, 0, 0));
		entity->setStatic(true);
		gameLogic.getPhysicsManager()->addEntity(entity);
				
		mesh = renderer->getDefaultSceneManager()->createEntity("boxStuffz", "Models/Box01.mesh");
		node = renderer->getDefaultSceneManager()->getRootSceneNode()->createChildSceneNode();
		node->attachObject(mesh);
		
		entity = gameLogic.createEntity();
		entity->setSceneNode(node);
		entity->setPosition(Ogre::Vector3(1, 2, 2));

		gameLogic.getPhysicsManager()->addEntity(entity);
		camera->lookAt(entity->getPosition());
		while (renderer->isWindowOpen()) {
			Ogre::Vector3 mousePosition = gameLogic.getInputManager()->getRelativeMousePosition();
			
			camera->pitch(Ogre::Radian(-mousePosition.y * 0.01f));
			camera->yaw(Ogre::Radian(-mousePosition.x * 0.01f));

			if (gameLogic.getInputManager()->isKeyDown(Input::KC_W)) {
				camera->moveRelative(Ogre::Vector3(0, 0, -0.1f));
			} else if (gameLogic.getInputManager()->isKeyDown(Input::KC_S)) {
				camera->moveRelative(Ogre::Vector3(0, 0, 0.1f));
			}

			gameLogic.update(0.16f);
			gameLogic.render();
		}
	}
};
