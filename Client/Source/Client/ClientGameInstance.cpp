#include "ClientGameInstance.h"

#include "Client.h"
#include "ClientPacketHandler.h"
#include "PacketSession.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"

void UClientGameInstance::ConnectToGameServer()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	FIPv4Address IPv4Address;
	FIPv4Address::Parse(IPAddress, OUT IPv4Address);
	
	TSharedPtr<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(IPv4Address.Value);
	InternetAddr->SetPort(Port);

	LOG_SCREEN(TEXT("Connecting To Server..."));

	bool bConnected = Socket->Connect(*InternetAddr);
	if (bConnected)
	{
		LOG_SCREEN(TEXT("Connection Success"));
		
		GameServerSession = MakeShared<PacketSession>(Socket);
		GameServerSession->Run();

		Protocol::C_LOGIN LoginPkt;
		SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(LoginPkt);
		SendPacket(SendBuffer);
	}
	else
	{
		LOG_SCREEN(TEXT("Connection Failed"));
	}
}

void UClientGameInstance::DisconnectFromGameServer()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	Protocol::C_LEAVE_GAME LeavePkt;
	SEND_PACKET(LeavePkt);
}

void UClientGameInstance::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->HandleRecvPackets();
}

void UClientGameInstance::SendPacket(SendBufferRef SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}

void UClientGameInstance::HandleSpawn(const Protocol::PlayerInfo& PlayerInfo)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectID = PlayerInfo.objectid();
	if (Players.Find(ObjectID) != nullptr)
		return;

	const Protocol::Vector3D& Position = PlayerInfo.transform().position();
	FVector SpawnLocation(Position.x(), Position.y(), Position.z());
	AActor* SpawnedActor = World->SpawnActor(PlayerClass, &SpawnLocation);
	
	Players.Add(PlayerInfo.objectid(), SpawnedActor);
}

void UClientGameInstance::HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
	HandleSpawn(EnterGamePkt.player());
}

void UClientGameInstance::HandleSpawn(const Protocol::S_SPAWN& SpawnPkt)
{
	for (const auto& Player : SpawnPkt.players())
		HandleSpawn(Player);
}

void UClientGameInstance::HandleDespawn(uint64 ObjectID)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	TObjectPtr<AActor>* FindActor = Players.Find(ObjectID);
	if (FindActor == nullptr)
		return;

	World->DestroyActor(*FindActor);
}

void UClientGameInstance::HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt)
{
	for (uint64 ObjectID : DespawnPkt.objectids())
		HandleDespawn(ObjectID);
}
