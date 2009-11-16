#pragma once

#include "GameLogic.h"
#include <iostream>

using namespace Bismuth;
using namespace Bismuth::Audio;

class AudioTest : public Test {
public:

	virtual void run() {
		/*AudioManager audio;
		if (audio.test() == 0) {
			std::cout << "Audio test was successful!" << std::endl;
		} else {
			std::cout << "Audio test FAILED!" << std::endl;
		}*/
		GameLogic engine(false);
		SharedPtr<AudioManager> audio = engine.getAudioManager();
		audio->playSound(Entity(&engine),AudioProperties());
		std::cout << "audio test" << std::endl;
	}
};