/**
 *    ___  _                __  __                 _
 *   / _ )(_)__ __ _  __ __/ /_/ /  ___ ___  ___ _(_)__  ___
 *  / _  / (_-</  ' \/ // / __/ _ \/ -_) _ \/ _ `/ / _ \/ -_)
 * /____/_/___/_/_/_/\_,_/\__/_//_/\__/_//_/\_, /_/_//_/\__/
 *                                         /___/
 *
 * @file Message.h
 */

#pragma once

#include "IStream.h"
#include "Entity.h"
#include <string>
#include <OgreStringConverter.h>

namespace Bismuth {

	enum MessageType {
		MsgDebugOut = 0,
		MsgEndOfFrame,
		MsgEntityAssigned,
		MsgCollision,
		MsgMoveEntity,
		MsgRotateEntity,
		MsgPressButton,
		MsgCreateEntity,
		MsgStartGame,
		MsgIncomingConnection,
		MsgPlayerIdAssigned,
		MsgFire
	};

	/**
	 * Message class
	 */
	class Message {
	public:
		Message() {};
		Message(MessageType type) : type(type) {}
		virtual ~Message() {}

		MessageType getType() { return type; }

		virtual void serialize(IStream *stream) { stream->write((int)type); };
		virtual void deserialize(IStream *stream) { };

	protected:
		MessageType type;
		friend class MessageFactory;

	};

	class EndOfFrameMessage : public Message {
	public:
		EndOfFrameMessage() : Message(MsgEndOfFrame) {}
		EndOfFrameMessage(float stepTime) : Message(MsgEndOfFrame) {
			this->stepTime = stepTime;
		}

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(stepTime);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			
			stepTime = stream->readFloat();
		}

		/**
		 * Get the entity which is the button.
		 */
		float getStepTime() const { return stepTime; }

	private:
		float stepTime;
	};

	class PressButtonMessage : public Message {
	public:
		PressButtonMessage() : Message(MsgPressButton) {}
		PressButtonMessage(int entityId) : Message(MsgPressButton) {
			this->entityId = entityId;
		}

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(entityId);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			
			entityId = stream->readInt();
		}

		/**
		 * Get the entity which is the button.
		 */
		int getEntityId() const { return entityId; }

	private:
		int entityId;
	};

	class RotateEntityMessage : public Message {
	public:
		RotateEntityMessage() : Message(MsgRotateEntity) {}
		RotateEntityMessage(int entityId, Ogre::Radian rotation) : Message(MsgRotateEntity), entityId(entityId), rotation(rotation) { }

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(entityId);
			stream->write(rotation.valueRadians());
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			entityId = stream->readInt();
			rotation = Ogre::Radian(stream->readFloat());
		}

		/**
		 * Get the orientation.
		 */
		Ogre::Radian getRotation() const { return rotation; }
		int getEntityId() const { return entityId; }

	private:
		int entityId;
		Ogre::Radian rotation;
		
	};


	class MoveEntityMessage : public Message {
	public:
		MoveEntityMessage() : Message(MsgMoveEntity) {}
		MoveEntityMessage(int entityId, char direction) : Message(MsgMoveEntity), entityId(entityId), direction(direction) { }

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(entityId);
			stream->write(direction);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			entityId = stream->readInt();
			direction = stream->readChar();
		}

		/**
		 * Get the direction in either w, a, s or d.
		 */
		char getDirection() const { return direction; }
		int getEntityId() const { return entityId; }

	private:
		int entityId;
		char direction;
	};

	class CollisionMessage : public Message {
	public:
		CollisionMessage() : Message(MsgCollision) {}
		CollisionMessage(int entityId1, int entityId2, float velocity) : Message(MsgCollision) {
			this->entityId1 = entityId1;
			this->entityId2 = entityId2;
			this->velocity = velocity;
		}

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(entityId1);
			stream->write(entityId2);
			stream->write(velocity);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			entityId1 = stream->readInt();
			entityId2 = stream->readInt();
			velocity = stream->readFloat();
		}

		/**
		 * Get the id of one of the entities.
		 */
		int getEntityId1() const { return entityId1; }

		/**
		 * Get the id of one of the entities.
		 */
		int getEntityId2() const { return entityId2; }

		/**
		 * Get the relative velocity.
		 */
		float getVelocity() const { return velocity; }

	private:
		int entityId1, entityId2;
		float velocity;

	};

	class DebugOutMessage : public Message {
	public:
		DebugOutMessage() : Message(MsgDebugOut) {}
		DebugOutMessage(const std::string &text) : Message(MsgDebugOut), text(text) {}

		std::string getText() const { return text; };

	private:
		std::string text;

	};

	/**
	 * Sent when a player is assigned an entity
	 */
	class EntityAssignedMessage : public Message {
	public:
		EntityAssignedMessage() : Message(MsgEntityAssigned) {}
		/**
		 * Create a new EntityAssignedMessage
		 * \param entityId Id of the assigned entity
		 * \param playerId Id of the player that was assigned the entity
		 */
		EntityAssignedMessage(int entityId, int playerId) : Message(MsgEntityAssigned), entityId(entityId), playerId(playerId) {}

		/**
		 * Get the id of the assigned entity
		 */
		int getEntityId() const { return entityId; }

		/**
		 * Get the id of the player that was assigned the entity
		 */
		int getPlayerId() const { return playerId; }

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(entityId);
			stream->write(playerId);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			entityId = stream->readInt();
			playerId = stream->readInt();
		}

	private:
		int entityId;
		int playerId;

	};
	
	class CreateEntityMessage : public Message {
	public:
		CreateEntityMessage() : Message(MsgCreateEntity), 
			meshName(""), 
			position(Ogre::Vector3(0, 0, 0)),
			orientation(Ogre::Quaternion()),
			entityType(ET_static),
			entityMaterial(EMT_wood),
			audioProperties(),
			scale(1.0f) { }

		void setMeshName(std::string meshName) { this->meshName = meshName; }
		std::string getMeshName() { return this->meshName; }

		void setPosition(Ogre::Vector3 position) { this->position = position; }
		Ogre::Vector3 getPosition() { return this->position; }

		void setOrientation(Ogre::Quaternion orientation) { this->orientation = orientation; }
		Ogre::Quaternion getOrientation() { return this->orientation; }

		void setEntityType(EntityType entityType) { this->entityType = entityType; }
		EntityType getEntityType() { return this->entityType; }

		void setEntityMaterial(EntityMaterial entityMaterial) { this->entityMaterial = entityMaterial; }
		EntityMaterial getEntityMaterial() { return this->entityMaterial; }

		void setAudioProperties(Audio::AudioProperties audioProperties) { this->audioProperties = audioProperties; }
		Audio::AudioProperties getAudioProperties() const { return audioProperties; }

		void setScale(float scale) { this->scale = scale; }
		float getScale() { return scale; }

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(meshName);
			stream->write(position);
			stream->write(orientation);
			stream->write((int)entityType);
			stream->write((int)entityMaterial);

			stream->write(audioProperties.directivity);
			stream->write(audioProperties.directivityOrientation);
			stream->write((int)audioProperties.loop);
			stream->write((int)audioProperties.soundType);
			stream->write((int)audioProperties.sounds.size());
			for (Audio::SoundMap::iterator iter = audioProperties.sounds.begin(); iter != audioProperties.sounds.end(); ++iter) {
				stream->write(iter->first)->write(iter->second);
			}
			stream->write(scale);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			meshName = stream->readString();
			position = stream->readVector3();
			orientation = stream->readQuaternion();
			entityType = (EntityType)stream->readInt();
			entityMaterial = (EntityMaterial)stream->readInt();

			audioProperties.directivity = stream->readFloat();
			audioProperties.directivityOrientation = stream->readVector3();
			audioProperties.loop = stream->readInt() == 1 ? 1 : 0;
			audioProperties.soundType = (Audio::SoundType)stream->readInt();
			int count = stream->readInt();
			for (int i = 0; i < count; i++) {
				int t = stream->readInt();
				std::string s = stream->readString();
				audioProperties.sounds.insert(std::make_pair((Audio::SoundType)t, s));
			}
			scale = stream->readFloat();
		}

	private:
		std::string meshName;
		Ogre::Vector3 position;
		Ogre::Quaternion orientation;
		EntityType entityType;
		EntityMaterial entityMaterial;
		Audio::AudioProperties audioProperties;
		float scale;
	};

	class StartGameMessage : public Message {
	public:
		StartGameMessage() : Message(MsgStartGame), numberOfPlayers(0) { };
		StartGameMessage(int numberOfPlayers) : Message(MsgStartGame), numberOfPlayers(numberOfPlayers) { };

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(numberOfPlayers);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			numberOfPlayers = stream->readInt();
		}

		int getNumberOfPlayers() { return numberOfPlayers; }
	private:
		int numberOfPlayers;
	};

	class IncomingConnectionMessage : public Message {
	public:
		IncomingConnectionMessage() : Message(MsgIncomingConnection) { };
	};

	class PlayerIdAssignedMessage : public Message {
	public:
		PlayerIdAssignedMessage() : Message(MsgPlayerIdAssigned), playerId(0) { };
		PlayerIdAssignedMessage(int playerId) : Message(MsgPlayerIdAssigned), playerId(playerId) { };

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(playerId);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			playerId = stream->readInt();
		}

		int getPlayerId() const {
			return playerId;
		}
	private:
		int playerId;
	};

	class FireMessage : public Message {
	public:
		FireMessage() : Message(MsgFire) { };
		FireMessage(int weapon, SharedPtr<CreateEntityMessage> createMessage) : Message(MsgFire), weapon(weapon), createMessage(createMessage) { };

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(weapon);
			createMessage->serialize(stream);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			weapon = stream->readInt();
			int createMessageId = stream->readInt(); // remove first 4 bytes, not used
			createMessage = SharedPtr<CreateEntityMessage>(new CreateEntityMessage());
			createMessage->deserialize(stream);

		}

		int getWeaponId() const {
			return weapon;
		}

		SharedPtr<CreateEntityMessage> getCreateEntityMessage() {
			return createMessage;
		}
	private:
		int weapon;
		SharedPtr<CreateEntityMessage> createMessage;
	};

	class MessageFactory {
	public:
		MessageFactory() {}
		~MessageFactory() {}

		static SharedPtr<Message> createFromStream(IStream *stream) {
			MessageType type = (MessageType)stream->readInt();
			SharedPtr<Message> message;

			switch (type) {
				case MsgDebugOut:
					message = SharedPtr<Message>(new DebugOutMessage());
					break;
				case MsgEndOfFrame:
					message = SharedPtr<Message>(new EndOfFrameMessage());
					break;
				case MsgEntityAssigned:
					message = SharedPtr<Message>(new EntityAssignedMessage());
					break;
				case MsgCollision:
					message = SharedPtr<Message>(new CollisionMessage());
					break;
				case MsgMoveEntity:
					message = SharedPtr<Message>(new MoveEntityMessage());
					break;
				case MsgRotateEntity:
					message = SharedPtr<Message>(new RotateEntityMessage());
					break;
				case MsgPressButton:
					message = SharedPtr<Message>(new PressButtonMessage());
					break;
				case MsgCreateEntity:
					message = SharedPtr<Message>(new CreateEntityMessage());
					break;
				case MsgStartGame:
					message = SharedPtr<Message>(new StartGameMessage());
					break;
				case MsgIncomingConnection:
					message = SharedPtr<Message>(new IncomingConnectionMessage());
					break;
				case MsgPlayerIdAssigned:
					message = SharedPtr<Message>(new PlayerIdAssignedMessage());
					break;
				default:
					std::cout << "Message.h: unknown type id: " << (int) type << std::endl;
					throw std::runtime_error("Message.h: unknown type id: " + Ogre::StringConverter::toString((int)type));
					break;
			}
			message->deserialize(stream);

			return message;
		}
	};

#define GET_MSG(type, message) type* msg = dynamic_cast<type*>(message.getPointer()); if (msg == 0) return;

}