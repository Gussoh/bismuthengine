#pragma once

#include "GameLogic.h"

class DemoGameLogic : public Bismuth::GameLogic {
public:
	DemoGameLogic(int numberOfPlayers);
	DemoGameLogic(std::string host);

	virtual ~DemoGameLogic();

	/**
	 * Handle a message
	 * \param message A pointer to the message to handle
	 */
	virtual void handleMessage(Bismuth::SharedPtr<Bismuth::Message> message);

	/** 
	 * Overrides GameLogic::handleEndOfFrameMessage.
	 * Performs game specific actions at end of frame.
	 */
	virtual void handleEndOfFrameMessage(Bismuth::SharedPtr<Bismuth::Message> message);
	
	/** 
	 * Overrides GameLogic::handleCollisionMessage.
	 * Handles explosions and damage.
	 */
	virtual void handleCollisionMessage(Bismuth::SharedPtr<Bismuth::Message> message);

	/** 
	 * Overrides GameLogic::handleStartGameMessage.
	 * Initialises the score board.
	 */
	virtual void handleStartGameMessage(Bismuth::SharedPtr<Bismuth::Message> message);

	/**
	 * Handles explosions and gives damage to players if they are hit by shots or explosions.
	 */
	void handleFireMessage(Bismuth::SharedPtr<Bismuth::Message> message);

	void handleDeathMessage(Bismuth::SharedPtr<Bismuth::Message> message);
	void handleSpawnMessage(Bismuth::SharedPtr<Bismuth::Message> message);

	void handleCollision(Bismuth::SharedPtr<Bismuth::Entity> entity, float velocity);
	void handleShotHitPlayer(Bismuth::SharedPtr<Bismuth::Entity> player, Bismuth::SharedPtr<Bismuth::Entity> shot, float velocity);

	virtual void update();
	virtual void updateGui();

protected:
	virtual void initGui();

private:
	QuickGUI::Image* imgWaiting;
	QuickGUI::Image* playerAvatar;
	QuickGUI::TextArea *textaScore;
	QuickGUI::TextArea *textaPlayerScore;
	QuickGUI::TextArea *textaScoreText;
	QuickGUI::TextArea *textaPlayerScoreText;
	QuickGUI::Image* imgHealth;
	QuickGUI::Image* imgReload;
	QuickGUI::Image* imgWeapon;

	
	bool dead;
	int health;
	int weapon;
	int nextShotAllowed;
	bool gogglesEnabled;
	bool gDown;
	int spawnOnFrame;
	int *scores;
};