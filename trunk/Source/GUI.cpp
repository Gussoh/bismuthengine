/**
 * @file GUI.cpp
 */

#include "stdafx.h"
#include "GUI.h"
#include "QuickGUI.h"

using namespace Bismuth;
using namespace Bismuth::GUI;


Bismuth::GUI::GUI::GUI() {

}

Bismuth::GUI::GUI::~GUI() {

}

QuickGUI::ImageDesc* createImageDesc() {
	return QuickGUI::DescManager::getSingleton().getDefaultImageDesc();
}

QuickGUI::Image* renderImage(QuickGUI::Sheet* mySheet, QuickGUI::ImageDesc* desc, Ogre::String name) {
	QuickGUI::Image *temp = mySheet->createImage(desc);
	temp->setImage(name);
	temp->setTileImage(true);
	return temp;
}
