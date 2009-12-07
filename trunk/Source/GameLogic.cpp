/**
 * @file GameLogic.cpp
 */

#include "stdafx.h"

#include "Entity.h"
#include "GameLogic.h"
#include "FmodAudioManager.h"
#include "OgreNewtPhysicsManager.h"
#include "RakNetworkManager.h"
#include "OISInputManager.h"
#include <QuickGUI.h>
#include <ctime>

using namespace Bismuth;
using namespace Bismuth::Audio;
using namespace Bismuth::Physics;
using namespace Bismuth::Network;
using namespace Bismuth::Graphics;
using namespace Bismuth::Input;

GameLogic::GameLogic(bool isServer) : isServer(isServer), nextEntityId(0), lastUpdate(0) {
	// Create ogre root
	Ogre::Root *root = new Ogre::Root("", "", "OgreLog.txt");

	// Renderer must be created first since a valid instance is needed by the physics manager.
	// What about isServer??
	this->renderer = new Renderer();
	this->renderer->init(800, 600, false); // Todo: should not be hard coded

	this->audioManager = new FmodAudioManager(this);
	this->physicsManager = new OgreNewtPhysicsManager(this);
	this->networkManager = new RakNetworkManager(this);
	this->inputManager = new OISInputManager(this->renderer->getWindowHandle(), 800, 600);

	initResourceLocations();
	guiTest();
}

void GameLogic::guiTest()
{
	new QuickGUI::Root();

	QuickGUI::SkinTypeManager::getSingletonPtr()->loadTypes();
	QuickGUI::GUIManagerDesc d;
	d.sceneManager = renderer->getDefaultSceneManager();
	d.viewport = renderer->getDefaultCamera()->getViewport();
	//d.queueID = Ogre::RENDER_QUEUE_OVERLAY;
	QuickGUI::GUIManager* mGUIManager = QuickGUI::Root::getSingletonPtr()->createGUIManager(d);

	// SHEET
	QuickGUI::SheetDesc* sd = QuickGUI::DescManager::getSingleton().getDefaultSheetDesc();
	sd->resetToDefault();
	sd->widget_dimensions.size = QuickGUI::Size(800,600);
	QuickGUI::Sheet* mySheet = QuickGUI::SheetManager::getSingleton().createSheet(sd);
	mGUIManager->setActiveSheet(mySheet);

	// Button
	QuickGUI::ButtonDesc* bd = QuickGUI::DescManager::getSingleton().getDefaultButtonDesc();
	bd->widget_name = "MyButton";
	bd->widget_dimensions.size = QuickGUI::Size(100,25);
	bd->widget_dimensions.position = QuickGUI::Point(50,50);
	QuickGUI::Button* myButton = mySheet->createButton(bd);
}

GameLogic::~GameLogic() {
	entities.clear();

	delete QuickGUI::Root::getSingletonPtr();

	delete inputManager;
	delete networkManager;
	delete physicsManager;
	delete audioManager;
	delete renderer;

	delete Ogre::Root::getSingletonPtr();
}

void GameLogic::initResourceLocations() {
	QuickGUI::registerScriptReader();
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(".", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\..\\..\\Assets", "FileSystem", "General", true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\..\\..\\Assets\\qgui.core.zip", "Zip", "General", true);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

SharedPtr<Entity> GameLogic::getEntityById(int id) {
	return entities[id];
}

void GameLogic::update() {
	if (isServer) {
		
		if(lastUpdate == 0) {
			lastUpdate = std::clock();
		}

		// if time since last end of frame goes beyond threshold. send end of fram.
		float stepTime = (float) (std::clock() - lastUpdate) / CLOCKS_PER_SEC;
		if (stepTime > .01) {
			// send eof
			SharedPtr<EndOfFrameMessage> eofMsg = SharedPtr<EndOfFrameMessage>(new EndOfFrameMessage(stepTime));
			networkManager->sendMessageToSelf(eofMsg);
			lastUpdate = std::clock();
		}
	}

	for(;;) {
		SharedPtr<Message> m = networkManager->getMessage(false);
		if (m.isNull()) {
			break;
		} else {
			// collect messages an propagate onto network
			if (this->isServer) {
				networkManager->sendMessage(m);
			}
			handleMessage(m);
		}
	}
	

		// collect keypresses and stuff
		// send them onto network.
	

	inputManager->update();
	
}

void GameLogic::render(){
	// Just delegate to the renderer
	renderer->render();
}

void GameLogic::sendMessage(SharedPtr<Message> message) {
	// Todo: Need to filter out collision messages on the client,
	// as they should not be sent to the server.

	// If server, send only message to self, since all incoming messages are sent to all clients before it is handled.
	if(this->isServer) {
		networkManager->sendMessageToSelf(message);
	} else {
		networkManager->sendMessage(message);
	}
}

void GameLogic::handleMessage(SharedPtr<Message> message) {
	switch (message->getType()) {
		case MsgDebugOut:
			handleDebugOutMessage(message);
			break;
		case MsgEntityAssigned:
			handleEntityAssignedMessage(message);
			break;
		case MsgEndOfFrame:
			handleEndOfFrameMessage(message);
			break;
		default:
			break;
	}
}

void GameLogic::handleDebugOutMessage(SharedPtr<Message> message) {
	GET_MSG(DebugOutMessage, message);
	std::cout << msg->getText();
}

void GameLogic::handleEntityAssignedMessage(SharedPtr<Message> message) {
	GET_MSG(EntityAssignedMessage, message);

	// Todo: check playerId 
	setPlayerEntity(getEntityById(msg->getEntityId()));
}

void GameLogic::handleEndOfFrameMessage(SharedPtr<Message> message) {
	GET_MSG(EndOfFrameMessage, message);
	physicsManager->update(msg->getStepTime());
}

void GameLogic::loadWorld(const std::string &name) {
	// Todo: Write a basic load function
	
}

SharedPtr<Entity> GameLogic::createEntity() {
	SharedPtr<Entity> entity = SharedPtr<Entity>(new Entity(nextEntityId));
	entities.insert(std::make_pair(nextEntityId, entity));
	
	Ogre::SceneNode *node = renderer->getDefaultSceneManager()->getRootSceneNode()->createChildSceneNode("Node" + entity->getId());
	entity->setSceneNode(node);

	nextEntityId++;

	return entity;
}

SharedPtr<Entity> GameLogic::createEntity(const Ogre::String &meshName) {
	SharedPtr<Entity> entity = createEntity();
	Ogre::Entity *mesh = renderer->getDefaultSceneManager()->createEntity("Mesh" + entity->getId(), meshName);

	entity->getSceneNode()->attachObject(mesh);

	return entity;
}

void GameLogic::setCameraEntity(SharedPtr<Entity> &entity) { 
	if (!cameraEntity.isNull()) {
		cameraEntity->getSceneNode()->detachObject(renderer->getDefaultCamera());
	}

	entity->getSceneNode()->attachObject(renderer->getDefaultCamera());
	
	cameraEntity = entity; 
}
