/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file Entity.h
 */

#pragma once

#include "IStream.h"
#include "OgreSceneNode.h"
#include "AudioProperties.h"

namespace Ogre {
	class AnimationStateSet;
}

namespace Bismuth {
	class GameLogic;

	enum EntityType {
		ET_dynamic = 0, // normal objects like a crate on the floor
		ET_player,
		ET_static,
		ET_button,		// a button is always static
		ET_light,
		ET_spawnarea,
		ET_shot,
		ET_COUNT, // Use ET_COUNT + n for your own entity types
	};

	enum EntityMaterial {
		EMT_wood = 0,
		EMT_steel,
		EMT_rubber,
		EMT_stone,
		EMT_styrofoam,
		EMT_player,
		EMT_COUNT, // Use EMT_COUNT + n for your own entity materials
	};

	/**
	 * The basic entity class, represent an object in the game world.
	 */
	class Entity {
	public:
		Entity(int id);
		Entity();
		virtual ~Entity();

		/**
		 * Tick the entity once
		 * @param elapsedTime Time in seconds since last update
		 */
		void update(float elapsedTime);

		/**
		 * Set the position
		 * @param position The new position
		 */
		void setPosition(const Ogre::Vector3 &position) {
			sceneNode->setPosition(position);
			positionOrientationChanged = true;
		}

		/**
		 * Get the current position
		 */
		Ogre::Vector3 getPosition() const {
			return sceneNode->getPosition();
		}

		/**
		 * Set the orientation
		 * @param orientation The new orientation
		 */
		void setOrientation(const Ogre::Quaternion &orientation) { 
			sceneNode->setOrientation(orientation); 
			positionOrientationChanged = true;
		}
		
		/**
		 * Get the current orientation
		 */
		Ogre::Quaternion getOrientation() const { 
			return sceneNode->getOrientation();
		}

		/**
		 * Set the id of this entity
		 */
		void setId(int id) { this->id = id; }

		/**
		 * Get the entitys id
		 */
		int getId() const { return id; }

		/**
		 * Serialize the entity into a stream
		 * @param stream A stream pointer
		 */
		void serialize(IStream *stream) const;

		/**
		 *  Deserialzie the entity from a stream
		 * @param stream A stream pointer
		 */
		void deserialize(IStream *stream);

		/**
		 * Get the type name of the entity
		 */
		virtual std::string typeName() const { return "Entity"; }

		/**
		 * Set the scene node of the entity
		 */
		void setSceneNode(Ogre::SceneNode *sceneNode) { 
			this->sceneNode = sceneNode; 
		}

		/**
		 * Get the entitys scene node
		 */
		Ogre::SceneNode* getSceneNode() { 
			return this->sceneNode; 
		}

		/**
		 * Set the type of entity
		 */
		void setType(EntityType type) { 
			this->type = type; 
		}

		/**
		 * Get the type of entity
		 */
		EntityType getType() const { 
			return type; 
		}

		/**
		 * Set the entity material
		 */
		void setMaterial(EntityMaterial material) { 
			this->material = material; 
		}

		/**
		 * Get the entity material
		 */
		EntityMaterial getMaterial() const { 
			return material; 
		}

		/**
		 * Get the audio properties
		 * @returns A pointer to the entities audio properties
		 */
		Bismuth::Audio::AudioProperties* getAudioPropertiesPtr() {
			return &audioProperties;
		}

		/**
		 * Set the audio properties
		 * @param audioProperties The new audio properties
		 */
		void setAudioProperties(Audio::AudioProperties audioProperties) { this->audioProperties = audioProperties; }
		
		/**
		 * Get a value indicating wheter the position or orientation has changed since the last frame
		 * @returns True if the position or orientation has been changed, false otherwise
		 */
		bool hasPositionOrientationChanged() const {
			return positionOrientationChanged;
		}

		/**
		 * Mark if the position or orientation has changed
		 */
		void setPositionOrientationChanged(bool value) {
			this->positionOrientationChanged = value;
		}

		void debugPrint() {
			std::cout << "id: " << id << ", type: " << type << ", material: " << material << std::endl;
		}

		/**
		 * Set the scale of the entity
		 */
		void setScale(float scale) {
			this->scale = scale;
		}

		/**
		 * Get the scale of the entity
		 */
		float getScale() const {
			return scale;
		}

		/**
		 * Get a value indicating wheter the entity has contact with another object
		 */
		bool hasContact() const {
			return contact;
		}

		/**
		 * Mark if the entity has contact with another object
		 */
		void setContact(bool contact) {
			this->contact = contact;
		}

		/**
		 * Set the animation states
		 */
		void setAnimationStates(Ogre::AnimationStateSet *set) {
			animationStates = set;
		}

		/**
		 * Get the animation state collection
		 */
		Ogre::AnimationStateSet *getAnimationStates() {
			return animationStates;
		}

	private:
		int id;
		float scale;
		Ogre::SceneNode *sceneNode;
		Bismuth::Audio::AudioProperties audioProperties;
		enum EntityType type;
		enum EntityMaterial material;
		bool positionOrientationChanged;
		bool contact;
		Ogre::AnimationStateSet *animationStates;
	};

#define GET_ENT(type, entity) Bismuth::type* ent = static_cast<Bismuth::type*>(entity.getPointer())
}