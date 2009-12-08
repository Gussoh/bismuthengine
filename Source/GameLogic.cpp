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

GameLogic::GameLogic(std::string host) : 
		isServer(false), 
		gameStarted(false), 
		nextEntityId(0), 
		lastUpdate(0), 
		maxNumberOfPlayers(0),
		currentPlayerId(0),
		myPlayerId(0) {
	initialize();
	networkManager->connect(host);
}

GameLogic::GameLogic(int maxNumberOfPlayers) : 
		isServer(true), 
		gameStarted(false), 
		nextEntityId(0), 
		lastUpdate(0), 
		maxNumberOfPlayers(maxNumberOfPlayers),
		currentPlayerId(0),
		myPlayerId(0) {

	myPlayerId = currentPlayerId;
	currentPlayerId++;
	initialize();
	// Number of connections is one less since the server also is a player.
	networkManager->startServer(maxNumberOfPlayers - 1);
}

void GameLogic::initialize() {
	Ogre::Root *root = new Ogre::Root("", "", "OgreLog" + (long)std::clock());

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
		case MsgCollision:
			handleCollisionMessage(message);
			break;
		case MsgPlayerMove:
			handlePlayerMoveMessage(message);
			break;
		case MsgPlayerRotate:
			handlePlayerRotateMessage(message);
			break;
		case MsgPressButton:
			handlePressButtonMessage(message);
			break;
		case MsgCreateEntity:
			handleCreateEntityMessage(message);
			break;
		case MsgStartGame:
			handleStartGameMessage(message);
			break;
		case MsgIncomingConnection:
			handleIncomingConnectionMessage(message);
			break;
		case MsgPlayerIdAssigned:
			handlePlayerIdAssignedMessage(message);
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
	inputManager->update();
	audioManager->update();

	Ogre::Vector3 mousePosition = inputManager->getRelativeMousePosition();
	SharedPtr<Entity> playerEntity = getPlayerEntity();
	if (!playerEntity.isNull()) {
		
		renderer->getDefaultCamera()->pitch(Ogre::Radian(-mousePosition.y * 0.005f));

		SharedPtr<PlayerRotateMessage> rotateMsg = SharedPtr<PlayerRotateMessage>(new PlayerRotateMessage(this, Ogre::Radian(-mousePosition.x * 0.005f)));
		sendMessage(rotateMsg);
		//playerEntity->getSceneNode()->yaw(Ogre::Radian(-mousePosition.x * 0.005f));
		playerEntity->setPositionOrientationChanged(true);

		if (inputManager->isKeyDown(Input::KC_W)) {
			SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(this, Input::KC_W));
			sendMessage(moveMsg);
		}
		if (inputManager->isKeyDown(Input::KC_S)) {
			SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(this, Input::KC_S));
			sendMessage(moveMsg);
		} 
		if (inputManager->isKeyDown(Input::KC_A)) {
			SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(this, Input::KC_A));
			sendMessage(moveMsg);
		}
		if (inputManager->isKeyDown(Input::KC_D)) {
			SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(this, Input::KC_D));
			sendMessage(moveMsg);
		} 
		if (inputManager->isKeyDown(Input::KC_SPACE)) {
			SharedPtr<PlayerMoveMessage> moveMsg = SharedPtr<PlayerMoveMessage>(new PlayerMoveMessage(this, Input::KC_SPACE));
			sendMessage(moveMsg);
		}
	}

}

void GameLogic::handleCollisionMessage(SharedPtr<Message> message) {
	GET_MSG(CollisionMessage, message);

	SharedPtr<Entity> entity1 = getEntityById(msg->getEntityId1());
	SharedPtr<Entity> entity2 = getEntityById(msg->getEntityId2());

	if ((entity1->getType() == ET_player && entity2->getType() == ET_dynamic)) {
		entity1->getAudioPropertiesPtr()->soundType = Audio::SoundType_Collision;
		audioManager->playSound(entity1);
	}
}

void GameLogic::handlePlayerMoveMessage(SharedPtr<Message> message) {
	GET_MSG(PlayerMoveMessage, message);
	SharedPtr<Entity> entity = getEntityById(msg->getEntityId());
	if (!entity.isNull()) {
		Ogre::Vector3 impulseVector;
		switch(msg->getDirection()) {
			case Input::KC_W:
				impulseVector = entity->getOrientation() * -Ogre::Vector3::UNIT_Z;
				break;
			case Input::KC_S:
				impulseVector = entity->getOrientation() * Ogre::Vector3::UNIT_Z;
				break;
			case Input::KC_A:
				impulseVector = entity->getOrientation() * -Ogre::Vector3::UNIT_X;
				break;
			case Input::KC_D:
				impulseVector = entity->getOrientation() * Ogre::Vector3::UNIT_X;
				break;
			case Input::KC_SPACE:
				impulseVector = entity->getOrientation() * Ogre::Vector3::UNIT_Y;
				break;
			default:
				return;
		}
		physicsManager->addImpulse(entity, impulseVector * 0.2f);
	}
}

void GameLogic::handlePlayerRotateMessage(SharedPtr<Message> message) {
	GET_MSG(PlayerRotateMessage, message);
	SharedPtr<Entity> entity = getEntityById(msg->getEntityId());
	if (!entity.isNull()) {
		entity->getSceneNode()->yaw(msg->getRotation());
	}
}

void GameLogic::handlePressButtonMessage(SharedPtr<Message> message) {
	GET_MSG(PressButtonMessage, message);
	// TODO: implement!
}

void GameLogic::handleCreateEntityMessage(SharedPtr<Message> message) {
	GET_MSG(CreateEntityMessage, message);

	SharedPtr<Entity> entity;
	if (msg->getMeshName().empty()) {
		entity = createEntity();
	} else {
		entity = createEntity(msg->getMeshName());
	}

	entity->setPosition(msg->getPosition());
	entity->setOrientation(msg->getOrientation());
	entity->setType(msg->getEntityType());
	entity->setMaterial(msg->getEntityMaterial());
}

void GameLogic::handleIncomingConnectionMessage(SharedPtr<Message> message) {
	networkManager->sendMessage(SharedPtr<Message>(new PlayerIdAssignedMessage(currentPlayerId)));
	currentPlayerId++;
}

void GameLogic::handlePlayerIdAssignedMessage(SharedPtr<Message> message) {
	GET_MSG(PlayerIdAssignedMessage, message);
	if (myPlayerId == 0) {
		myPlayerId = msg->getPlayerId();
	}
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
	// Detach the camera from the old scene node
	if (!cameraEntity.isNull()) {
		cameraEntity->getSceneNode()->detachObject(renderer->getDefaultCamera());
	}

	// Attach the camera to the new sceneNode provided bu the caller.
	entity->getSceneNode()->attachObject(renderer->getDefaultCamera());
	
	cameraEntity = entity; 
}

bool GameLogic::isGameStarted() {
	if (!gameStarted) {
		SharedPtr<Message> message = networkManager->getMessage(false);
		if (message.isNull()) {
			return gameStarted;
		}
		handleMessage(message);
		
		if (isServer) {
			if (currentPlayerId == maxNumberOfPlayers) {
				sendMessage(SharedPtr<StartGameMessage>(new StartGameMessage()));
			}
		} 
	}

	return gameStarted;
}
