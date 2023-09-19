#include "ClientGameInstance.h"

#include "Client.h"
#include "ClientPacketHandler.h"
#include "PacketSession.h"
#include "PlayerPawn.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Kismet/GameplayStatics.h"

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

APawn* UClientGameInstance::HandleSpawn(const Protocol::PlayerInfo& PlayerInfo)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return nullptr;

	UWorld* World = GetWorld();
	if (World == nullptr)
		return nullptr;

	const uint64 ObjectID = PlayerInfo.objectid();
	if (Players.Find(ObjectID) != nullptr)
		return Players.Find(ObjectID)->Get();

	const Protocol::Vector3D& Position = PlayerInfo.transform().position();
	FVector SpawnLocation(Position.x(), Position.y(), Position.z());
	APawn* SpawnedPawn = Cast<APawn>(World->SpawnActor(PlayerClass, &SpawnLocation));
	
	Players.Add(PlayerInfo.objectid(), SpawnedPawn);
	return SpawnedPawn;
}

void UClientGameInstance::HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
	APawn* PlayerPawn = HandleSpawn(EnterGamePkt.player());

	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0))
		PlayerController->Possess(PlayerPawn);
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

	TObjectPtr<APawn>* FindPawn = Players.Find(ObjectID);
	if (FindPawn == nullptr)
		return;

	World->DestroyActor(*FindPawn);
}

void UClientGameInstance::HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt)
{
	for (uint64 ObjectID : DespawnPkt.objectids())
		HandleDespawn(ObjectID);
}

void UClientGameInstance::HandleMove(const Protocol::S_MOVE& MovePkt)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	const uint64 ObjectID = MovePkt.objectid();
	if (Players.Contains(ObjectID) == false)
		return;

	const Protocol::Vector3D& Position = MovePkt.position();
	Cast<APlayerPawn>(Players[ObjectID])->TargetPosition = FVector(Position.x(), Position.y(), Position.z());
}
