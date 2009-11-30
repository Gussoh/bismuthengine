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
		MsgEntityAssigned
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

	class DebugOutMessage : public Message {
	public:
		DebugOutMessage() : Message() {}
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
		EntityAssignedMessage() : Message() {}
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
				case MsgEntityAssigned:
					message = SharedPtr<Message>(new EntityAssignedMessage());
					break;
				default:
					throw std::runtime_error("unknown type id ");
					break;
			}

			message->deserialize(stream);
			return message;
		}
	};

#define GET_MSG(type) type* msg = dynamic_cast<type*>(message.getPointer()); if (msg == 0) return;

}