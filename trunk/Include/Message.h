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

namespace Bismuth {

	enum MessageType {
		MsgDebugOut = 0,
	};

	/**
	 * Message class
	 */
	class Message {
	public:
		Message(MessageType type) : type(type) {}
		virtual ~Message() {}

		MessageType getType() { return type; }

	protected:
		MessageType type;

	};

	class DebugOutMessage : public Message {
	public:
		DebugOutMessage(const std::string &text) : Message(MsgDebugOut), text(text) {}

		std::string getText() const { return text; };

	private:
		std::string text;

	};

}