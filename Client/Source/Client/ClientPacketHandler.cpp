#include "ClientPacketHandler.h"

#include "ClientGameMode.h"
#include "Kismet/GameplayStatics.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return false;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	UWorld* World = GEngine->GameViewport->GetWorld();
	AClientGameMode* ClientGameMode = Cast<AClientGameMode>(UGameplayStatics::GetGameMode(World));
	ClientGameMode->AddPlayer(pkt.playerinfo(), true);
	return true;
}

bool Handle_S_LEAVE_GAME(PacketSessionRef& session, Protocol::S_LEAVE_GAME& pkt)
{
	UWorld* World = GEngine->GameViewport->GetWorld();
	AClientGameMode* ClientGameMode = Cast<AClientGameMode>(UGameplayStatics::GetGameMode(World));
	ClientGameMode->RemoveMyPlayer();
	return true;
}

bool Handle_S_SPAWN(PacketSessionRef& session, Protocol::S_SPAWN& pkt)
{
	UWorld* World = GEngine->GameViewport->GetWorld();
	AClientGameMode* ClientGameMode = Cast<AClientGameMode>(UGameplayStatics::GetGameMode(World));
	
	for (const Protocol::PlayerInfo& Player : pkt.players())
	{
		ClientGameMode->AddPlayer(Player, false);
	}
	return true;
}

bool Handle_S_DESPAWN(PacketSessionRef& session, Protocol::S_DESPAWN& pkt)
{
	UWorld* World = GEngine->GameViewport->GetWorld();
	AClientGameMode* ClientGameMode = Cast<AClientGameMode>(UGameplayStatics::GetGameMode(World));
	
	for (const uint64 ID : pkt.playerids())
	{
		ClientGameMode->RemovePlayer(ID);
	}
	return true;
}

bool Handle_S_MOVE(PacketSessionRef& session, Protocol::S_MOVE& pkt)
{
	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	std::string Msg = pkt.msg();
	LOG(TEXT("%s"), *FString(Msg.c_str()));
	return true;
}
