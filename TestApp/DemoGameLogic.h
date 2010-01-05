#pragma once

#include "GameLogic.h"

class DemoGameLogic : public Bismuth::GameLogic {
public:
	DemoGameLogic(int numberOfPlayers);
	DemoGameLogic(std::string host);

	virtual ~DemoGameLogic();

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