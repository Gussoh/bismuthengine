#pragma once

#include "AudioManager.h"
#include <iostream>

using namespace BismuthAudio;

class AudioTest : public Test {
public:

	virtual void run() {
		AudioManager audio;
		std::cout << audio.test();
	}
};