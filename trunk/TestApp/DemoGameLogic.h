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

	/** Overrides GameLogic::handleEndOfFrameMessage.
	 * Performs game specific actions at end of frame.
	 */
	virtual void handleEndOfFrameMessage(Bismuth::SharedPtr<Bismuth::Message> message);

	
	void handleCollisionMessage(Bismuth::SharedPtr<Bismuth::Message> message);

	void handleCollision(Bismuth::SharedPtr<Bismuth::Entity> entity, float velocity);

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
};