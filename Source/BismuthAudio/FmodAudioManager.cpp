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
FmodAudioManager::FmodAudioManager(GameLogic *gameLogic) {
	this->gameLogic = gameLogic;

	FMOD_RESULT result;

	// Create the main system object.
	result = FMOD::System_Create(&fmodSystem);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		throw std::exception();
		// TO DO: 
			// change to the correct exception
	}

	// Initialize FMOD with a right handed coord. system.
	result = fmodSystem->init(100, FMOD_INIT_3D_RIGHTHANDED, 0);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));		
		throw std::exception();
	}	

}

FmodAudioManager::~FmodAudioManager() {

}

void FmodAudioManager::update() {
	//To do:
		// update listener properties
		// fmodSystem->update();
}

void FmodAudioManager::playSound(SharedPtr<Entity> &entity) {

	FMOD_RESULT result;
	FMOD::Sound *sound;
	FMOD::Channel *channel;
	FMOD::DSP *dsp;

	// retrieve the sound from the entity depending on the SoundType (e.g. default, collision)
	AudioProperties * audioPropertiesPtr = entity->getAudioPropertiesPtr();

	if (audioPropertiesPtr->soundType == SoundType_Default)
	{
		result = fmodSystem->createSound("Audio/jaguar.wav", FMOD_LOOP_NORMAL, 0, &sound);
		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));	
		}
		
		int loopCount = 0;
		if (audioPropertiesPtr->loop)
		{
			loopCount = -1; // infinite loop
		}
		else
		{
			loopCount = 0; // play once
		}

		result = sound->setLoopCount(loopCount);
		
		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		}
		// TO DO:
			// get the correct audio file for the default sound from the entity
	}

	// TO DO:
		// add check for other sound types, maybe a switch
		// error handling
			
	
	// TO DO:
		// get the position and velocity of the entity and use: 
			// channel->set3DAttributes(FMOD_VECTOR pos, FMOD_VECTOR vel)
	Ogre::Vector3 entityPos = entity->getPosition();

		// get directivity of the entity

	result = fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);

	// TO DO: apply a series of effects depending on the audio properties
		// reverb, 

	fmodSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
	channel->addDSP(dsp, 0);

	//Sleep(10000);

	//channel->stop();
}





void FmodAudioManager::updateListener() {
	
	// entity.getPosition();
	// entity.getOrientation();
	// fmodSystem->set3DListenerAttributes

	// TO DO:
		// directivity

}