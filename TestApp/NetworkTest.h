#pragma once

#include "NetworkManager.h"
#include <iostream>

using namespace BismuthNetwork;

class NetworkTest : public Test {
public:

	virtual void run() {
		AudioManager audio;
		if (audio.test() == 0) {
			std::cout << "Audio test was successful!" << std::endl;
		} else {
			std::cout << "Audio test FAILED!" << std::endl;
		}
	}
};