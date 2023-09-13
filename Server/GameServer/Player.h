#pragma once

class GameRoom;

class Player
{
public:
	Protocol::PlayerInfo	_info;
	weak_ptr<GameRoom>		_room;
	weak_ptr<GameSession>	_session;
};
