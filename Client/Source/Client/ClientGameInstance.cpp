#include "ClientGameInstance.h"

#include "Client.h"
#include "ClientPacketHandler.h"
#include "PacketSession.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "ClientMyPlayer.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Kismet/GameplayStatics.h"

void UClientGameInstance::Init()
{
	Super::Init();

	RequestConnect();
}

void UClientGameInstance::Tick(float DeltaTime)
{
	HandleRecvPackets();
}

void UClientGameInstance::Shutdown()
{
	Super::Shutdown();

	RequestDisconnect();
}

void UClientGameInstance::RequestConnect()
{
	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(TEXT("Stream"), TEXT("Client Socket"));

	FIPv4Address IPv4Address;
	FIPv4Address::Parse(IPAddress, OUT IPv4Address);
	
	TSharedPtr<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	InternetAddr->SetIp(IPv4Address.Value);
	InternetAddr->SetPort(Port);

	LOG_SCREEN(TEXT("Connecting To Server..."));
	
	if (Socket->Connect(*InternetAddr))
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

void UClientGameInstance::HandleRecvPackets()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->HandleRecvPackets();
}

void UClientGameInstance::RequestDisconnect()
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	Protocol::C_LEAVE_GAME LeavePkt;
	SEND_PACKET(LeavePkt);
}

void UClientGameInstance::Disconnect()
{
	if (GameServerSession)
	{
		GameServerSession->Disconnect();
		GameServerSession = nullptr;
	}
	
	if (Socket)
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
		SocketSubsystem->DestroySocket(Socket);
		Socket = nullptr;
	}
}

void UClientGameInstance::SendPacket(SendBufferRef SendBuffer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	GameServerSession->SendPacket(SendBuffer);
}

void UClientGameInstance::HandleSpawn(const Protocol::PlayerInfo& PlayerInfo, bool IsMyPlayer)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	const uint64 ObjectID = PlayerInfo.objectid();
	if (Players.Contains(ObjectID))
		return;
	
	if (IsMyPlayer)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if (AClientMyPlayer* Player = Cast<AClientMyPlayer>(PlayerController->GetPawn()))
		{
			MyPlayer = Player;
			Player->Init(true, PlayerInfo);
			Players.Add(PlayerInfo.objectid(), Player);
		}
	}
	else
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		if (AClientPlayer* Player = Cast<AClientPlayer>(World->SpawnActor(OtherPlayerClass, nullptr, SpawnParameters)))
		{
			Player->Init(false, PlayerInfo);
			Players.Add(PlayerInfo.objectid(), Player);
		}
	}
}

void UClientGameInstance::HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt)
{
	HandleSpawn(EnterGamePkt.player(), true);
}

void UClientGameInstance::HandleSpawn(const Protocol::S_SPAWN& SpawnPkt)
{
	for (const auto& Player : SpawnPkt.players())
		HandleSpawn(Player, false);
}

void UClientGameInstance::HandleDespawn(uint64 ObjectID)
{
	if (Socket == nullptr || GameServerSession == nullptr)
		return;

	UWorld* World = GetWorld();
	if (World == nullptr)
		return;

	TObjectPtr<AClientPlayer>* FindPlayer = Players.Find(ObjectID);
	if (FindPlayer == nullptr)
		return;

	World->DestroyActor(*FindPlayer);
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

	const uint64 ObjectID = MovePkt.info().objectid();
	TObjectPtr<AClientPlayer>* FindPlayer = Players.Find(ObjectID);
	if (FindPlayer == nullptr)
		return;

	AClientPlayer* Player =	(*FindPlayer);
	if (Player->IsMyPlayer())
		return;
	
	Player->SetDesiredPlayerInfo(MovePkt.info());
}
