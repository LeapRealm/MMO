#include "ClientPacketHandler.h"

#include "Client.h"
#include "ClientGameInstance.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	Protocol::C_ENTER_GAME EnterGamePkt;
	EnterGamePkt.set_playerindex(0);
	SEND_PACKET(EnterGamePkt);
	
	return true;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	if (UClientGameInstance* ClientGameInstance = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		ClientGameInstance->HandleSpawnPlayer(pkt);
	return true;
}

bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt)
{
	if (UClientGameInstance* ClientGameInstance = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		ClientGameInstance->Disconnect();
	return true;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	if (UClientGameInstance* ClientGameInstance = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		ClientGameInstance->HandleSpawn(pkt);
	return true;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	if (UClientGameInstance* ClientGameInstance = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		ClientGameInstance->HandleDespawn(pkt);
	return true;
}

bool Handle_S_MOVE_PLAYER(PacketSessionRef& session, Protocol::S_MOVE_PLAYER& pkt)
{
	if (UClientGameInstance* ClientGameInstance = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		ClientGameInstance->HandleMovePlayer(pkt);
	return true;
}

bool Handle_S_MOVE_MONSTER(PacketSessionRef& session, Protocol::S_MOVE_MONSTER& pkt)
{
	if (UClientGameInstance* ClientGameInstance = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		ClientGameInstance->HandleMoveMonster(pkt);
	return true;
}

bool Handle_S_ATTACK(PacketSessionRef& session, Protocol::S_ATTACK& pkt)
{
	if (UClientGameInstance* ClientGameInstance = Cast<UClientGameInstance>(GWorld->GetGameInstance()))
		ClientGameInstance->HandleAttack(pkt);
	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	std::string Msg = pkt.msg();
	LOG(TEXT("%s"), *FString(Msg.c_str()));
	return true;
}
