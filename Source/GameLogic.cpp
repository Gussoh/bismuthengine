/**
 * @file GameLogic.cpp
 */

#include "stdafx.h"

//#include "Entity.h"
#include "GameLogic.h"
#include "QuickGUI.h"
#include "ShotEntity.h"
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
		nextShotAllowed(0),
		weapon(2),
		health(100) {
		gDown(false), 
		gogglesEnabled(false) {
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
		nextShotAllowed(0),
		gDown(false), 
		gogglesEnabled(false) {
		weapon(2),
		health(100) {

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

	renderer->addCompositor("Bloom");
	renderer->addCompositor("SeeThrough");
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
	/*
	QuickGUI::ImageDesc *imgdBottomPanel1 = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdBottomPanel1->widget_name = "BottomPanel1";
	imgdBottomPanel1->widget_dimensions.size = QuickGUI::Size(95, 121);
	imgdBottomPanel1->widget_dimensions.position = QuickGUI::Point(0, 600-121);
	QuickGUI::Image* imgBottomPanel1 = mySheet->createImage(imgdBottomPanel1);
	imgBottomPanel1->setImage("panel1.png");
	imgBottomPanel1->setTileImage(true);
	//
	QuickGUI::ImageDesc *imgdBottomPanel2 = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdBottomPanel2->widget_name = "BottomPanel2";
	imgdBottomPanel2->widget_dimensions.size = QuickGUI::Size(128, 61);
	imgdBottomPanel2->widget_dimensions.position = QuickGUI::Point(95, 600-61);
	QuickGUI::Image* imgBottomPanel2 = mySheet->createImage(imgdBottomPanel2);
	imgBottomPanel2->setImage("panel2.png");
	imgBottomPanel2->setTileImage(true);
	*/

	// Create background for health
	QuickGUI::ImageDesc *imgdHealthBack = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdHealthBack->widget_name = "HealthBack";
	imgdHealthBack->widget_dimensions.size = QuickGUI::Size(100, 19);
	imgdHealthBack->widget_dimensions.position = QuickGUI::Point(88, 571);
	QuickGUI::Image* imgHealthBack = mySheet->createImage(imgdHealthBack);
	imgHealthBack->setImage("health2.png");
	imgHealthBack->setTileImage(true);
	// Create health
	QuickGUI::ImageDesc *imgdHealth = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdHealth->widget_name = "Health";
	imgdHealth->widget_dimensions.size = QuickGUI::Size(100, 19);
	imgdHealth->widget_dimensions.position = QuickGUI::Point(88, 571);
	imgHealth = mySheet->createImage(imgdHealth);
	imgHealth->setImage("health1.png");
	imgHealth->setTileImage(true);

	// Create cross
	QuickGUI::ImageDesc *imgdCross = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdCross->widget_name = "Cross";
	imgdCross->widget_dimensions.size = QuickGUI::Size(41, 41);
	imgdCross->widget_dimensions.position = QuickGUI::Point(400 - 21, 300 - 21);
	QuickGUI::Image* imgCross = mySheet->createImage(imgdCross);
	imgCross->setImage("cross1.png");
	imgCross->setTileImage(false);

	// Create character avatar
	QuickGUI::ImageDesc *imgdFace = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdFace->widget_name = "Face";
	imgdFace->widget_dimensions.size = QuickGUI::Size(73, 98);
	imgdFace->widget_dimensions.position = QuickGUI::Point(10, 492);
	playerAvatar = mySheet->createImage(imgdFace);
	playerAvatar->setImage("riddler.jpg");
	playerAvatar->setTileImage(true);

	// Create health
	QuickGUI::TextAreaDesc *textadHealth = QuickGUI::DescManager::getSingleton().getDefaultTextAreaDesc();
	textadHealth->widget_dimensions.position = QuickGUI::Point(40, 40);
	textaHealth = mySheet->createTextArea(textadHealth);
	textaHealth->setText("100%");
}

GameLogic::~GameLogic() {
	entities.clear();
	timedAnimations.clear();

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
			if (this->isServer) {
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

	// Update health text
	textaHealth->setText(Ogre::StringConverter::toString(health) + "%");
	// Update health bar
	if (health < 5)
	{
		imgHealth->setWidth(5.0f);
	}
	else
	{
		imgHealth->setWidth((float)health);
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
	} 
}

void GameLogic::addSpecialMessage(SharedPtr<Message> message) {
	specialMessageQueue.push(message);
}

void GameLogic::removeEntity(SharedPtr<Entity> entity) {
	entities.erase(entity->getId());
	physicsManager->removeEntity(entity);
	renderer->getDefaultSceneManager()->destroySceneNode(entity->getSceneNode());
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

SharedPtr<Entity> GameLogic::createEntity(EntityType type) {

	SharedPtr<Entity> entity;
	switch(type) {
		case ET_shot:
			entity = SharedPtr<Entity>(new ShotEntity(nextEntityId));
			break;
		default:
			entity = SharedPtr<Entity>(new Entity(nextEntityId));
			break;
	}
	
	entities.insert(std::make_pair(nextEntityId, entity));
	
	Ogre::SceneNode *node = renderer->getDefaultSceneManager()->getRootSceneNode()->createChildSceneNode("Node" + Ogre::StringConverter::toString(entity->getId()));
	entity->setSceneNode(node);

	nextEntityId++;

	return entity;
}


SharedPtr<Entity> GameLogic::createEntity(EntityType type, const Ogre::String &meshName) {
	SharedPtr<Entity> entity = createEntity(type);
	Ogre::Entity *mesh = renderer->getDefaultSceneManager()->createEntity("Mesh" + Ogre::StringConverter::toString(entity->getId()), meshName);
	mesh->getMesh()->buildTangentVectors();

	entity->getSceneNode()->attachObject(mesh);

	return entity;
}

void GameLogic::addTimedAnimation(int time, Ogre::SceneNode *node) {
	SharedPtr<TimedAnimation> anim(new TimedAnimation());
	anim->time = time;
	anim->sceneNode = node;
	timedAnimations.push_back(anim);
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
