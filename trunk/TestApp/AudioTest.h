#pragma once

#include "BismuthEngine.h"
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
		BismuthEngine engine;
		SharedPtr<AudioManager> audio = engine.getAudioManager();
		audio->playSound(Entity(),AudioProperties());
		std::cout << "audio test" << std::endl;
	}
};