/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "FmodAudioManager.h"
#include "fmod_errors.h"
#include <OgreResourceGroupManager.h>
#include <OgreDataStream.h>

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

	initOccludingGeometry();
}

FmodAudioManager::~FmodAudioManager() {

}

void FmodAudioManager::update() {
	
	updateListener();
	updateOccludingGeometry();
	fmodSystem->update();
}

void FmodAudioManager::playSound(SharedPtr<Entity> &entity) {

	FMOD_RESULT result;
	FMOD::Sound *sound;
	FMOD::Channel *channel;
	FMOD::DSP *dsp;

	// get pointer to audio properties and retrieve the sound from the entity depending on the SoundType
		//(e.g. default, collision), also check if the channel is already playing the same sound, in that 
		//case return without playing the sound
	AudioProperties * audioPropertiesPtr = entity->getAudioPropertiesPtr();

	if (audioPropertiesPtr->sounds.find(audioPropertiesPtr->soundType) == audioPropertiesPtr->sounds.end()) {
		return;
	}

	if (activeSounds.find(entity->getId()) != activeSounds.end()) {
		ActiveSoundChannels &channels = activeSounds[entity->getId()];
		if (channels.find(audioPropertiesPtr->soundType) != channels.end()) {
			bool isPlaying;
			if (channels[audioPropertiesPtr->soundType]->isPlaying(&isPlaying) == FMOD_OK && isPlaying) {
				return;
			}
		}
	}

	std::string filename = audioPropertiesPtr->sounds[audioPropertiesPtr->soundType];
	int loopCount = audioPropertiesPtr->soundType == SoundType_Continuous ? -1 : 0;

	sound = createSound(filename, FMOD_3D, 0);

	result = sound->setLoopCount(loopCount);

	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
	}


	
	// get and set the directivity of the entity	
	if (entity->getAudioPropertiesPtr()->directivity == 0 )
	{
		// omni, use default values for set3DConeSettings
		result = sound->set3DConeSettings(360,360,1);
		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		}
	}
	else
	{
		// use set3DConeSettings, set3DConeOrientation
	}

	result = fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);

	if (activeSounds.find(entity->getId()) == activeSounds.end()) {
		ActiveSoundChannels as;
		activeSounds.insert(std::make_pair(entity->getId(), as));
	}

	activeSounds[entity->getId()][audioPropertiesPtr->soundType] = channel;


	// get the position and velocity of the entity and use: 

	Ogre::Vector3 entityPos = entity->getPosition();
	channel->set3DAttributes(&ogreToFmodVector(entityPos),0);
	// TO DO: apply a series of effects depending on the audio properties
		// reverb, 

	// fmodSystem->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
	// channel->addDSP(dsp, 0);

}






void FmodAudioManager::playSoundtrack() {

	FMOD_RESULT result;
	FMOD::Sound *sound;
	FMOD::Channel *channel;


	sound = createSound("Audio/silent_night.mp3", FMOD_LOOP_NORMAL, 0);

	result = sound->setLoopCount(-1); // infinite loop

	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
	}



	result = fmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);

}


void FmodAudioManager::preloadSounds() {
	// from Ogre resource group manager get a listing of *.wav files. Put all sounds in the cache
	Ogre::StringVectorPtr strVPtr = Ogre::ResourceGroupManager::getSingleton().findResourceNames("General","*.wav",false);
	float volume = getMasterVolume();
	setMasterVolume(0);
	// loop and cache all sounds

	setMasterVolume(volume);

}
void FmodAudioManager::setMasterVolume(float volume) {
	// TODO:
		// check if volume is between 0 and 1, otherwise raise an error
	FMOD::ChannelGroup  ** chGroup;
	FMOD_RESULT result = fmodSystem->getMasterChannelGroup(chGroup);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
	}
	// set the volume
	

}

float FmodAudioManager::getMasterVolume() {
	return 1.0;
}

void FmodAudioManager::updateListener() {
	SharedPtr<Entity> playerEntity = gameLogic->getPlayerEntity();
	if (playerEntity.isNull()) {
		return;
	}

	Ogre::Vector3 pos = playerEntity->getPosition();
	Ogre::Quaternion orientation_quaternion = playerEntity->getOrientation();
	Ogre::Vector3 upVector = orientation_quaternion * Ogre::Vector3::UNIT_Y;
	Ogre::Vector3 forwardVector = orientation_quaternion * -Ogre::Vector3::UNIT_Z;
	
	fmodSystem->set3DListenerAttributes(0,&ogreToFmodVector(pos),0,&ogreToFmodVector(forwardVector),&ogreToFmodVector(upVector));

	// TO DO:
		// directivity
		// velocity

}

void FmodAudioManager::initOccludingGeometry() {
	// TO DO:
		// determine the world size in some smart way
	FMOD_RESULT result;
	float maxWorldSize = 100;
	result = fmodSystem->setGeometrySettings(maxWorldSize);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
	}
}

void FmodAudioManager::updateOccludingGeometry() {

}

FMOD_VECTOR FmodAudioManager::ogreToFmodVector(Ogre::Vector3 ogreVector){
	FMOD_VECTOR returnVector;
	returnVector.x =  ogreVector.x;
	returnVector.y =  ogreVector.y;
	returnVector.z =  ogreVector.z;
	return returnVector;
}

FMOD::Sound *FmodAudioManager::createSound(const std::string &filename, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO *exInfo) {
	SoundCache::iterator iter = soundCache.find(filename);
	
	if (iter != soundCache.end()) {
		return iter->second;
	} else {
		Ogre::DataStreamPtr ds = Ogre::ResourceGroupManager::getSingleton().openResource(filename);
		char *data = new char[ds->size()];
		ds->read(data, ds->size());

		FMOD_CREATESOUNDEXINFO ex = { 0 };
		ex.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		ex.length = ds->size();

		FMOD::Sound *sound;
		FMOD_RESULT result = fmodSystem->createSound(data,  mode | FMOD_OPENMEMORY | FMOD_CREATESAMPLE, &ex, &sound);
		delete [] data;

		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));	
		}

		soundCache.insert(std::make_pair(filename, sound));
		
		return sound;
	}
}