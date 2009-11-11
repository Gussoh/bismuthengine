/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "FmodAudioManager.h"
#include "fmod_errors.h"

using namespace Bismuth;
using namespace Bismuth::Audio;

/** 
* Initialise the Fmod system
*/
FmodAudioManager::FmodAudioManager() {
	FMOD_RESULT result;

	result = FMOD::System_Create(&fmodSystem);		// Create the main system object.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		
		throw std::exception(); // TODO: change to the correct exception
	}

	result = fmodSystem->init(100, FMOD_INIT_NORMAL, 0);	// Initialize FMOD.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		
		throw std::exception();
	}
	

}

/**
* Close the Fmod system.
*/
FmodAudioManager::~FmodAudioManager() {

}

void FmodAudioManager::playSound(const Entity &entity, const AudioProperties &properties) {

	FMOD_RESULT result;
	FMOD::Sound *sound;

	result = fmodSystem->createSound("Audio/jaguar.wav", FMOD_LOOP_NORMAL, 0, &sound);		// FMOD_DEFAULT uses the defaults.  These are the same as FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		
	}

	FMOD::Channel *channel;
	result = fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		
	}
	fmodSystem->update();
	FMOD::DSP *dsp, *dsp2;
	fmodSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
	fmodSystem->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp2);
	dsp2->setParameter(0, 100);
	channel->addDSP(dsp, 0);
	channel->addDSP(dsp2, 0);

	//Sleep(10000);

	//channel->stop();

	


}



