/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file GUI.h
 */

#pragma once

#include <QuickGUI.h>

namespace Bismuth {
namespace GUI {

	/**
	 * GUI class
	 */
	class GUI {
	private:
		/** 
		* Returns an image description needed for an image.
		*/
		virtual QuickGUI::ImageDesc* createImageDesc();
	public:
		GUI();
		virtual ~GUI();

		/** 
		* Returns an image rendered on the screen
		*
		* @param mySheet Sheet that the image belongs to.
		* @param desc	ImageDescription.
		* @param name	Filename for the picture.
		*/
		virtual QuickGUI::Image* renderImage(QuickGUI::Sheet* mySheet, QuickGUI::ImageDesc* desc, Ogre::String name);
	};
}
}