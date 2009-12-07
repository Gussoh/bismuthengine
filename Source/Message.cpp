/**
 * @file Template.cpp
 */

#include "stdafx.h"
#include "Message.h"
#include "GameLogic.h"

using namespace Bismuth;
 
PlayerMoveMessage::PlayerMoveMessage(GameLogic *gameLogic, char direction) : Message(MsgPlayerMove), direction(direction) {
	SharedPtr<Entity> playerEntity = gameLogic->getPlayerEntity();
	if (playerEntity.isNull()) {
		entityId = -1; // Set to no entity
	} else {
		entityId = playerEntity->getId();
	}
}