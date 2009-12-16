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

Ogre::String playerNames[] = { "Goran", "Maria", "Lars", "Thomas", "Leif", "Jimmie", "Fredrik", "Riddler" };

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
		health(100),
		gDown(false), 
		gogglesEnabled(false),
		dead(true),
		spawnOnFrame(100),
		scores(NULL) {

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
		gogglesEnabled(false),
		weapon(2),
		dead(true),
		spawnOnFrame(100),
		health(100),
		scores(NULL) {


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
	imgHealth->setMinSize(QuickGUI::Size(1, 1));
	imgHealth->setTileImage(true);

	// Create background for reload
	QuickGUI::ImageDesc *imgdReloadBack = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdReloadBack->widget_name = "ReloadBack";
	imgdReloadBack->widget_dimensions.size = QuickGUI::Size(100, 19);
	imgdReloadBack->widget_dimensions.position = QuickGUI::Point(690, 571);
	QuickGUI::Image* imgReloadBack = mySheet->createImage(imgdReloadBack);
	imgReloadBack->setImage("reload2.png");
	imgReloadBack->setTileImage(true);
	// Create reload bar
	QuickGUI::ImageDesc *imgdReload = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdReload->widget_name = "Reload";
	imgdReload->widget_dimensions.size = QuickGUI::Size(100, 19);
	imgdReload->widget_dimensions.position = QuickGUI::Point(690, 571);
	imgReload = mySheet->createImage(imgdReload);
	imgReload->setImage("reload1.png");
	imgReload->setMinSize(QuickGUI::Size(1, 1));
	imgReload->setTileImage(true);

	// Create weapon
	QuickGUI::ImageDesc *imgdWeapon = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdWeapon->widget_name = "Weapon";
	imgdWeapon->widget_dimensions.size = QuickGUI::Size(62, 67);
	imgdWeapon->widget_dimensions.position = QuickGUI::Point(107, 498);
	imgWeapon = mySheet->createImage(imgdWeapon);
	imgWeapon->setImage("weapon1.png");
	imgWeapon->setTileImage(true);

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

	// Create score text
	QuickGUI::TextAreaDesc *textadScoreText = QuickGUI::DescManager::getSingleton().getDefaultTextAreaDesc();
	textadScoreText->widget_dimensions.position = QuickGUI::Point(0, 0); //19
	textaScoreText = mySheet->createTextArea(textadScoreText);
	textaScoreText->setText("Maria\nLars\nPlayer4");
	textaScoreText->setFont("verdana.14");
	QuickGUI::TextAreaDesc *textadPlayerScoreText = QuickGUI::DescManager::getSingleton().getDefaultTextAreaDesc();
	textadPlayerScoreText->widget_dimensions.position = QuickGUI::Point(0, 0);
	textaPlayerScoreText = mySheet->createTextArea(textadPlayerScoreText);
	textaPlayerScoreText->setText("YOU");
	textaPlayerScoreText->setFont("verdana.14");
	textaPlayerScoreText->setTextColor(QuickGUI::ColourValue(1.0f, 1.0f, 0.0f, 1.0f));
	// Create score
	QuickGUI::TextAreaDesc *textadScore = QuickGUI::DescManager::getSingleton().getDefaultTextAreaDesc();
	textadScore->widget_dimensions.position = QuickGUI::Point(80, 0); //19
	textaScore = mySheet->createTextArea(textadScore);
	textaScore->setText("\n4\n3\n7");
	textaScore->setFont("verdana.14");
	QuickGUI::TextAreaDesc *textadPlayerScore = QuickGUI::DescManager::getSingleton().getDefaultTextAreaDesc();
	textadPlayerScore->widget_dimensions.position = QuickGUI::Point(80, 0);
	textaPlayerScore = mySheet->createTextArea(textadPlayerScore);
	textaPlayerScore->setText("42");
	textaPlayerScore->setFont("verdana.14");
	textaPlayerScore->setTextColor(QuickGUI::ColourValue(1.0f, 1.0f, 0.0f, 1.0f));

	// Create waiting background
	QuickGUI::ImageDesc *imgdWaiting = QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
	imgdWaiting->widget_name = "Waiting";
	imgdWaiting->widget_dimensions.size = QuickGUI::Size(800, 600);
	imgdWaiting->widget_dimensions.position = QuickGUI::Point(0, 0);
	imgWaiting = mySheet->createImage(imgdWaiting);
	imgWaiting->setImage("waiting.png");
	imgWaiting->setTileImage(true);
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
	if(scores != NULL) {
		delete[] scores;
	}

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

	for (EntityList::iterator iter = entities.begin(); iter != entities.end(); ++iter) {
		Ogre::AnimationStateSet *states = iter->second->getAnimationStates();
		if (states != 0 && states->hasEnabledAnimationState()) {
			Ogre::ConstEnabledAnimationStateIterator state = states->getEnabledAnimationStateIterator();
			while (state.hasMoreElements()) {
				state.peekNext()->addTime(1.0f / 50.0f);
				state.moveNext();
			}
		}

		if (states != 0) {
			float velocity = physicsManager->getVelocity(iter->second);
			if (velocity <= 0.1f && states->hasAnimationState("Idle1")) {
				states->getAnimationState("Idle1")->setEnabled(true);
				if (states->hasAnimationState("Walk")) {
					states->getAnimationState("Walk")->setEnabled(false);
				}
				if (states->hasAnimationState("JumpNoHeight")) {
					states->getAnimationState("JumpNoHeight")->setEnabled(false);
				}
			} else if (velocity > 0.1f && states->hasAnimationState("Walk")) {
				if (states->hasAnimationState("Idle1")) {
					states->getAnimationState("Idle1")->setEnabled(false);
				}

				if (!iter->second->hasContact() && states->hasAnimationState("JumpNoHeight")) {
					states->getAnimationState("JumpNoHeight")->setEnabled(true);
					states->getAnimationState("Walk")->setEnabled(false);
				} else {
					states->getAnimationState("Walk")->setEnabled(true);
					if (states->hasAnimationState("JumpNoHeight")) {
						states->getAnimationState("JumpNoHeight")->setEnabled(false);
					}
				}
			}
		}
	}

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

	// Remove waiting screen if started
	if (imgWaiting != NULL && gameStarted == true)
	{
		imgWaiting->setVisible(false);
		imgWaiting->destroy();
		imgWaiting = NULL;
	}

	// UPDATE AVATAR
	if (myPlayerId > 6 || myPlayerId < 0)
	{
		playerAvatar->setImage("riddler.jpg");
	}
	else if (playerAvatar->getImageName().compare(playerNames[myPlayerId] + ".jpg") != 0)
	{
		playerAvatar->setImage(playerNames[myPlayerId] + ".jpg");
	}

	// Update score text
	Ogre::String scoreNames = "";

	for (int i = 0; i < numberOfPlayers; ++i)
	{
		if (i == myPlayerId)
			continue;

		if (i > 6)
		{
			scoreNames += "\nRiddler";
		}
		else
		{
			scoreNames += "\n" + playerNames[i];
		}
	}

	textaScoreText->setText(scoreNames);

	// Update score
	if (scores != NULL)
	{
		Ogre::String scoreString;

		for (int i = 0; i < numberOfPlayers; ++i)
		{
			if (i == myPlayerId)
				continue;

			scoreString += "\n" + Ogre::StringConverter::toString(scores[i]);
		}

		textaScore->setText(scoreString);
		textaPlayerScore->setText(Ogre::StringConverter::toString(scores[myPlayerId]));
	}

	// Update health bar
	if (health < 1)
	{
		imgHealth->setVisible(false);
	}
	else
	{
		imgHealth->setVisible(true);
		imgHealth->setWidth((float)health);
	}

	// Update reload bar
	if (nextShotAllowed - frameCounter < 1)
	{
		imgReload->setVisible(false);
	}
	else
	{
		imgReload->setVisible(true);
		imgReload->setWidth((float)(nextShotAllowed - frameCounter));
		imgReload->setPosition(QuickGUI::Point(690 + (100 - (nextShotAllowed - frameCounter)), 571));
	}

	// Update weapon
	imgWeapon->setImage("weapon" + Ogre::StringConverter::toString(weapon) + ".png");
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
	//renderer->getDefaultSceneManager()->destroySceneNode(entity->getSceneNode());
}

bool GameLogic::shouldSendMessage(MessageType msgType) {
	return (msgType == MsgMoveEntity || 
			msgType == MsgRotateEntity || 
			msgType == MsgPressButton ||
			msgType == MsgFire ||
			msgType == MsgDeath ||
			msgType == MsgSpawn);
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

	entity->setAnimationStates(mesh->getAllAnimationStates());

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
