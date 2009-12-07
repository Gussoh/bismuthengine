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

namespace Bismuth {

	enum MessageType {
		MsgDebugOut = 0,
		MsgEndOfFrame,
		MsgEntityAssigned,
		MsgCollision,
		MsgPlayerMove,
		MsgPlayerRotate,
		MsgPressButton
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

		virtual void serialize(IStream *stream) const { stream->write((int)type); };
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

	class PlayerRotateMessage : public Message {
	public:
		PlayerRotateMessage() : Message(MsgPlayerRotate) {}
		PlayerRotateMessage(Ogre::Quaternion rotation) : Message(MsgPlayerRotate) {
			this->rotation = rotation;
		}

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(rotation);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			
			rotation = stream->readQuaternion();
		}

		/**
		 * Get the orientation.
		 */
		Ogre::Quaternion getRotation() const { return rotation; }

	private:
		Ogre::Quaternion rotation;
	};


	class PlayerMoveMessage : public Message {
	public:
		PlayerMoveMessage() : Message(MsgPlayerMove) {}
		PlayerMoveMessage(char direction) : Message(MsgPlayerMove) {
			this->direction = direction;
		}

		virtual void serialize(IStream *stream) {
			Message::serialize(stream);
			stream->write(direction);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			direction = stream->readChar();
		}

		/**
		 * Get the direction in either w, a, s or d.
		 */
		char getDirection() const { return direction; }

	private:
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
			stream->write(entityId1)->write(entityId2)->write(velocity);
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
			stream->write(entityId)->write(playerId);
		}

		virtual void deserialize(IStream *stream) {
			Message::deserialize(stream);
			playerId = stream->readInt();
		}

	private:
		int entityId;
		int playerId;

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
				case MsgPlayerMove:
					message = SharedPtr<Message>(new PlayerMoveMessage());
					break;
				case MsgPlayerRotate:
					message = SharedPtr<Message>(new PlayerRotateMessage());
					break;
				case MsgPressButton:
					message = SharedPtr<Message>(new PressButtonMessage());
					break;
				default:
					std::cout << "Message.h: unknown type id: " << (int) type << std::endl;
					throw std::runtime_error("Message.h: unknown type id: (CANNOT CONCATENATE!)");
					break;
			}
			message->deserialize(stream);

			return message;
		}
	};

#define GET_MSG(type, message) type* msg = dynamic_cast<type*>(message.getPointer()); if (msg == 0) return;

}