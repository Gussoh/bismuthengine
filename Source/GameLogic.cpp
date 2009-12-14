/**
 * @file GameLogic.cpp
 */

#include "stdafx.h"

//#include "Entity.h"
#include "GameLogic.h"
#include "QuickGUI.h"
#include <ctime>
#include <OgreEntity.h>
#include <OgreRay.h>
#include <OgreCamera.h>
#include <windows.h> // JOHAN please solve this!! :(

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
		myPlayerId(-1),
		frameCounter(0),
		nextShotAllowed(0) {
	initialize();
	if(!networkManager->connect(host)) {
		MessageBox(NULL, TEXT("Failed to connect to server."), TEXT("Internet fail"), MB_ICONERROR);
		exit(1);
	}
}

GameLogic::GameLogic(int numberOfPlayers) : 
		isServer(true), 
		gameStarted(false), 
		nextEntityId(0), 
		lastUpdate(0), 
		numberOfPlayers(numberOfPlayers),
		playerIdCounter(0),
		myPlayerId(-1),
		frameCounter(0),
		nextShotAllowed(0) {

	myPlayerId = playerIdCounter;
	playerIdCounter++;
	initialize();
	// Number of connections is one less since the server also is a player.
	networkManager->startServer(numberOfPlayers - 1);
}

void GameLogic::initialize() {
	Ogre::Root *root = new Ogre::Root("", "", "OgreLog" + Ogre::StringConverter::toString((long)std::clock()) + ".txt");

//	Ogre::LogManager::getSingleton().getDefaultLog()->setLogDetail(Ogre::LL_LOW);

	// Renderer must be created first since a valid instance is needed by the physics manager.
	// What about isServer??
	this->renderer = new Renderer();
	this->renderer->init(800, 600, false); // Todo: should not be hard coded

	this->audioManager = createAudioManager(this);
	this->physicsManager = createPhysicsManager(this);
	this->networkManager = createNetworkManager(this);
	this->inputManager = creatInputManager(this->renderer->getWindowHandle(), 800, 600);

	initResourceLocations();
	audioManager->preloadSounds();
	guiTest();

	renderer->setCompositorEnabled("Bloom", false);
	renderer->setCompositorEnabled("SeeThrough", false);
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

	// CREATE BOTTOM PANEL
	QuickGUI::ImageDesc *imgdBottomPanel = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdBottomPanel->widget_name = "BottomPanel";
	imgdBottomPanel->widget_dimensions.size = QuickGUI::Size(800, 98);
	imgdBottomPanel->widget_dimensions.position = QuickGUI::Point(0, 502);
	QuickGUI::Image* imgBottomPanel = mySheet->createImage(imgdBottomPanel);
	imgBottomPanel->setImage("texture1.jpg");
	imgBottomPanel->setTileImage(true);

	// CREATE GÖRAN PERSSON
	QuickGUI::ImageDesc *imgdFace = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdFace->widget_name = "Face";
	imgdFace->widget_dimensions.size = QuickGUI::Size(73, 98);
	imgdFace->widget_dimensions.position = QuickGUI::Point(363, 502);
	//QuickGUI::Image playerAvatar = mySheet->createImage(imgdFace);
	playerAvatar = mySheet->createImage(imgdFace);
	playerAvatar->setImage("riddler.jpg");
	playerAvatar->setTileImage(true);
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
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("..\\..\\..\\Assets\\hotpix.zip", "Zip", "General", true);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

SharedPtr<Entity> GameLogic::getEntityById(int id) {
	EntityList::iterator iter = entities.find(id);
	if (iter == entities.end()) {
		return SharedPtr<Entity>();
	} else {
		return iter->second;
	}
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
			if (this->isServer && !handleMessageSelf(m->getType())) {
				networkManager->sendMessage(m);
			}
			handleMessage(m);
		}
	}
		// collect keypresses and stuff
		// send them onto network.


	// UPDATE AVATAR

	Ogre::String avatarName;

	switch (myPlayerId)
	{
	case 0:
		avatarName = "goran.jpg";
		break;
	case 1:
		avatarName = "maria.jpg";
		break;
	case 2:
		avatarName = "lars.jpg";
		break;
	case 3:
		avatarName = "thomas.jpg";
		break;
	case 4:
		avatarName = "leif.jpg";
		break;
	case 5:
		avatarName = "jimmie.jpg";
		break;
	case 6:
		avatarName = "fredrik.jpg";
		break;
	default: // Including -1
		avatarName = "riddler.jpg";
		break;

	}

	if (playerAvatar->getImageName().compare(avatarName) != 0)
	{
		playerAvatar->setImage(avatarName);
	}
}

void GameLogic::render(){
	// Just delegate to the renderer
	renderer->render();
}

void GameLogic::sendMessage(SharedPtr<Message> message) {

	// If server, send only message to self, since all incoming messages are sent to all clients before it is handled.
	if(this->isServer) {
		networkManager->sendMessageToSelf(message);
	} else if (shouldSendMessage(message->getType())) {
		networkManager->sendMessage(message);
	} else if (handleMessageSelf(message->getType())) {
		// This is for clients only and must be checked after shouldSendMessage()
		networkManager->sendMessageToSelf(message); 
	}
}

void GameLogic::removeEntity(SharedPtr<Entity> entity) {
	entities.erase(entity->getId());
	physicsManager->removeEntity(entity);
	renderer->getDefaultSceneManager()->destroySceneNode(entity->getSceneNode());
}

bool GameLogic::handleMessageSelf(MessageType msgType) {
	return (msgType == MsgCollision);
}

bool GameLogic::shouldSendMessage(MessageType msgType) {
	return (msgType == MsgMoveEntity || 
			msgType == MsgRotateEntity || 
			msgType == MsgPressButton ||
			msgType == MsgFire);
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
	mesh->getMesh()->buildTangentVectors();

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

	
	}

	return gameStarted;
}

int GameLogic::getNumberOfPlayers() {
	return numberOfPlayers;
}

SharedPtr<Entity> GameLogic::GetEntityAtScreenPosition(float x, float y, float maxDistance) {
	Ogre::Ray ray = renderer->getDefaultCamera()->getCameraToViewportRay(x, y);

	Ogre::Vector3 startPosition = ray.getOrigin();
	Ogre::Vector3 endPosition = ray.getPoint(maxDistance);

	return physicsManager->getFirstEntityAlongRay(startPosition, endPosition);
}