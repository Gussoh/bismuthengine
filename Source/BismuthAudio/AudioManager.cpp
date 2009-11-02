/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "Template.h"
#include "fmod.hpp"
#include "fmod_errors.h"
#include "stdio.h"
#include "AudioManager.h"
#include "windows.h"

using namespace BismuthAudio;

AudioManager::AudioManager() {

}

AudioManager::~AudioManager() {

}

int AudioManager::test () {
	FMOD_RESULT result;
	FMOD::System *system;

	result = FMOD::System_Create(&system);		// Create the main system object.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return -1;
	}

	result = system->init(100, FMOD_INIT_NORMAL, 0);	// Initialize FMOD.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return -1;
	}

	FMOD::Sound *sound;
	result = system->createSound("../../Media/Audio/jaguar.wav", FMOD_DEFAULT, 0, &sound);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return -1;
	}

	FMOD::Channel *channel;
	result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		return -1;
	}

	Sleep(1000);

}