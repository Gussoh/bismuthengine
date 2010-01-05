#include "stdafx.h"
#include "Renderer.h"
#include "OgreSceneManager.h"
#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "Entity.h"
#include "DemoGameLogic.h"

using namespace Bismuth;

Ogre::String playerNames[] = { "Goran", "Maria", "Lars", "Thomas", "Leif", "Jimmie", "Fredrik", "Riddler" };

DemoGameLogic::DemoGameLogic(std::string host) : GameLogic(host) {

}

DemoGameLogic::DemoGameLogic(int numberOfPlayers) : GameLogic(numberOfPlayers) {

}

DemoGameLogic::~DemoGameLogic() {

}

void DemoGameLogic::initGui() {
	GameLogic::initGui();

	QuickGUI::SkinTypeManager::getSingletonPtr()->loadTypes();
	QuickGUI::GUIManagerDesc d;
	d.sceneManager = getRenderer()->getDefaultSceneManager();
	d.viewport = getRenderer()->getDefaultCamera()->getViewport();
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

void DemoGameLogic::update() {
	// Get the animations running
	for (EntityList::iterator iter = getEntities()->begin(); iter != getEntities()->end(); ++iter) {
		Ogre::AnimationStateSet *states = iter->second->getAnimationStates();
		if (states != 0 && states->hasEnabledAnimationState()) {
			Ogre::ConstEnabledAnimationStateIterator state = states->getEnabledAnimationStateIterator();
			while (state.hasMoreElements()) {
				state.peekNext()->addTime(1.0f / 50.0f);
				state.moveNext();
			}
		}

		if (states != 0) {
			float velocity = getPhysicsManager()->getVelocity(iter->second);
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

	GameLogic::update();
}

void DemoGameLogic::updateGui() {
	GameLogic::updateGui();

	// Remove waiting screen if started
	if (imgWaiting != NULL && isGameStarted() == true)
	{
		imgWaiting->setVisible(false);
		imgWaiting->destroy();
		imgWaiting = NULL;
	}

	// UPDATE AVATAR
	if (getMyPlayerId() > 6 || getMyPlayerId() < 0)
	{
		playerAvatar->setImage("riddler.jpg");
	}
	else if (playerAvatar->getImageName().compare(playerNames[getMyPlayerId()] + ".jpg") != 0)
	{
		playerAvatar->setImage(playerNames[getMyPlayerId()] + ".jpg");
	}

	// Update score text
	Ogre::String scoreNames = "";

	for (int i = 0; i < getNumberOfPlayers(); ++i)
	{
		if (i == getMyPlayerId())
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

		for (int i = 0; i < getNumberOfPlayers(); ++i)
		{
			if (i == getMyPlayerId())
				continue;

			scoreString += "\n" + Ogre::StringConverter::toString(scores[i]);
		}

		textaScore->setText(scoreString);
		textaPlayerScore->setText(Ogre::StringConverter::toString(scores[getMyPlayerId()]));
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
	if (nextShotAllowed - getFrameCounter() < 1)
	{
		imgReload->setVisible(false);
	}
	else
	{
		imgReload->setVisible(true);
		imgReload->setWidth((float)(nextShotAllowed - getFrameCounter()));
		imgReload->setPosition(QuickGUI::Point(690 + (100 - (nextShotAllowed - getFrameCounter())), 571));
	}

	// Update weapon
	imgWeapon->setImage("weapon" + Ogre::StringConverter::toString(weapon) + ".png");
}