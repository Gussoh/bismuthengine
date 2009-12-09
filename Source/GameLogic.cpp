/**
 * @file GameLogic.cpp
 */

#include "stdafx.h"

//#include "Entity.h"
#include "GameLogic.h"
#include <QuickGUI.h>
#include <ctime>
#include <OgreEntity.h>

using namespace Bismuth;
using namespace Bismuth::Audio;
using namespace Bismuth::Physics;
using namespace Bismuth::Network;
using namespace Bismuth::Graphics;
using namespace Bismuth::Input;

GameLogic::GameLogic(std::string host) : 
		isServer(false), 
		gameStarted(false), 
		nextEntityId(0), 
		lastUpdate(0), 
		numberOfPlayers(0),
		playerIdCounter(0),
		myPlayerId(-1) {
	initialize();
	networkManager->connect(host);
}

GameLogic::GameLogic(int numberOfPlayers) : 
		isServer(true), 
		gameStarted(false), 
		nextEntityId(0), 
		lastUpdate(0), 
		numberOfPlayers(numberOfPlayers),
		playerIdCounter(0),
		myPlayerId(-1) {

	myPlayerId = playerIdCounter;
	playerIdCounter++;
	initialize();
	// Number of connections is one less since the server also is a player.
	networkManager->startServer(numberOfPlayers - 1);
}

void GameLogic::initialize() {
	Ogre::Root *root = new Ogre::Root("", "", "OgreLog" + Ogre::StringConverter::toString((long)std::clock()) + ".txt");

	Ogre::LogManager::getSingleton().getDefaultLog()->setLogDetail(Ogre::LL_LOW);

	// Renderer must be created first since a valid instance is needed by the physics manager.
	// What about isServer??
	this->renderer = new Renderer();
	this->renderer->init(800, 600, false); // Todo: should not be hard coded

	this->audioManager = createAudioManager(this);
	this->physicsManager = createPhysicsManager(this);
	this->networkManager = createNetworkManager(this);
	this->inputManager = creatInputManager(this->renderer->getWindowHandle(), 800, 600);

	initResourceLocations();
	//guiTest();
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

	QuickGUI::PanelDesc *pd = QuickGUI::DescManager::getSingleton().getDefaultPanelDesc();
	pd->widget_name = "MyPanel";
	pd->widget_dimensions.size = QuickGUI::Size(800, 40);
	QuickGUI::Panel* myPanel = mySheet->createPanel(pd);

	QuickGUI::ImageDesc *imageDescription = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imageDescription->widget_name = "MyImage";
	imageDescription->widget_dimensions.size = QuickGUI::Size(150, 25);
	imageDescription->widget_dimensions.position = QuickGUI::Point(0, 575);
	QuickGUI::Image* myImage = mySheet->createImage(imageDescription);
	myImage->setImage("lobby.textbox.png");
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
		if (stepTime > .016) {
			// send eof
			SharedPtr<EndOfFrameMessage> eofMsg = SharedPtr<EndOfFrameMessage>(new EndOfFrameMessage(stepTime));
			sendMessage(eofMsg);
			lastUpdate = std::clock();
		}
	}

	for(;;) {
		SharedPtr<Message> m = networkManager->getMessage(false);
		if (m.isNull()) {
			break;
		} else {
			// collect messages and propagate onto network
			if (this->isServer) {
				networkManager->sendMessage(m);
			}
			handleMessage(m);
		}
	}
		// collect keypresses and stuff
		// send them onto network.
	
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
	} else if (shouldSendMessage(message->getType())) {
		networkManager->sendMessage(message);
	}
}

bool GameLogic::shouldSendMessage(MessageType msg) {
	return (msg == MsgPlayerMove || msg == MsgPlayerRotate || msg == MsgPressButton);
}


void GameLogic::loadWorld(const std::string &name) {
	// Todo: Write a basic load function
	
}

SharedPtr<Entity> GameLogic::createEntity() {
	SharedPtr<Entity> entity = SharedPtr<Entity>(new Entity(nextEntityId));
	entities.insert(std::make_pair(nextEntityId, entity));
	
	Ogre::SceneNode *node = renderer->getDefaultSceneManager()->getRootSceneNode()->createChildSceneNode("Node" + Ogre::StringConverter::toString(entity->getId()));
	entity->setSceneNode(node);

	nextEntityId++;

	return entity;
}

SharedPtr<Entity> GameLogic::createEntity(const Ogre::String &meshName) {
	SharedPtr<Entity> entity = createEntity();
	Ogre::Entity *mesh = renderer->getDefaultSceneManager()->createEntity("Mesh" + Ogre::StringConverter::toString(entity->getId()), meshName);

	entity->getSceneNode()->attachObject(mesh);

	return entity;
}

void GameLogic::setCameraEntity(SharedPtr<Entity> &entity) { 
	// Detach the camera from the old scene node
	if (!cameraEntity.isNull()) {
		cameraEntity->getSceneNode()->detachObject(renderer->getDefaultCamera());
		cameraEntity->getSceneNode()->setVisible(true);
	}

	// Change the camera position so that it is at a good default position relative to the rendered object.	
	Ogre::Vector3 center = entity->getSceneNode()->getAttachedObject(0)->getBoundingBox().getCenter();
	renderer->getDefaultCamera()->setPosition(center);

	entity->getSceneNode()->attachObject(renderer->getDefaultCamera());
	entity->getSceneNode()->setVisible(false);
	cameraEntity = entity; 
}

bool GameLogic::isGameStarted() {
	if (!gameStarted) {

		if (isServer) {
			if (playerIdCounter == numberOfPlayers) {
				sendMessage(SharedPtr<StartGameMessage>(new StartGameMessage(numberOfPlayers)));
			}
		} 

		SharedPtr<Message> message = networkManager->getMessage(false);

		if (message.isNull()) {
			return false;
		}

		if(isServer) {
			networkManager->sendMessage(message);
		}
		handleMessage(message);

		/*if (message->getType() == MsgStartGame) {
			gameStarted = true;
			std::cout << "Game STARTED!!!!!!!!!!!!!" << std::endl;
			if (isServer) {
				networkManager->sendMessage(message);
			}
		} else if (message->getType() == MsgPlayerIdAssigned) {
			if(myPlayerId == -1) {
				PlayerIdAssignedMessage* msg = dynamic_cast<PlayerIdAssignedMessage*>(message.getPointer());
				myPlayerId = msg->getPlayerId();
				std::cout << "My player ID assigned: " << myPlayerId << std::endl;
			}
		} else if (message->getType() == MsgIncomingConnection) {
			networkManager->sendMessage(SharedPtr<Message>(new PlayerIdAssignedMessage(playerIdCounter)));
			std::cout << "Assigning new ID to client: " << playerIdCounter << std::endl;
			playerIdCounter++;
		}*/
	}

	return gameStarted;
}

int GameLogic::getNumberOfPlayers() {
	return numberOfPlayers;
}
