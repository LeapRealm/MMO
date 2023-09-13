#include "ClientGameMode.h"

#include "ClientGameInstance.h"
#include "ClientPacketHandler.h"
#include "EmptyPawn.h"
#include "MyPlayerBase.h"
#include "Protocol.pb.h"
#include "Kismet/GameplayStatics.h"

AClientGameMode::AClientGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultPawnClass = AEmptyPawn::StaticClass();
}

void AClientGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	ClientGameInstance = Cast<UClientGameInstance>(GetGameInstance());
	ClientGameInstance->ConnectToGameServer();
	RequestEnterGame();
}

void AClientGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	ClientGameInstance->HandleRecvPackets();
}

void AClientGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ClientGameInstance->DisconnectFromGameServer();
}

void AClientGameMode::RequestEnterGame()
{
	Protocol::C_ENTER_GAME pkt;
	SendBufferRef SendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
	ClientGameInstance->SendPacket(SendBuffer);
};

void AClientGameMode::AddPlayer(const Protocol::PlayerInfo& PlayerInfo, bool bMyPlayer)
{
	if (bMyPlayer)
	{
		FVector Position;
		Position.Set(PlayerInfo.positon().x(), PlayerInfo.positon().y(), PlayerInfo.positon().z());
		FActorSpawnParameters SpawnInfo; 
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AMyPlayerBase* MyPlayerBase = GetWorld()->SpawnActor<AMyPlayerBase>(Position, FRotator::ZeroRotator, SpawnInfo);
		MyPlayerBase->ID = PlayerInfo.playerid();

		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		PlayerController->Possess(MyPlayerBase);

		Players.Add(PlayerInfo.playerid(), MyPlayerBase);
	}
	else
	{
		FVector Position;
		Position.Set(PlayerInfo.positon().x(), PlayerInfo.positon().y(), PlayerInfo.positon().z());
		FActorSpawnParameters SpawnInfo; 
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		APlayerBase* PlayerBase = GetWorld()->SpawnActor<APlayerBase>(Position, FRotator::ZeroRotator, SpawnInfo);
		PlayerBase->ID = PlayerInfo.playerid();

		Players.Add(PlayerInfo.playerid(), PlayerBase);
	}
}

void AClientGameMode::RemovePlayer(uint64 ID)
{
	TObjectPtr<APlayerBase>* Player = Players.Find(ID);
	if (Player == nullptr)
		return;
	
	Player->Get()->Destroy();
	Players.Remove(ID);
}

void AClientGameMode::RemoveMyPlayer()
{
	if (MyPlayer == nullptr)
		return;
	
	RemovePlayer(MyPlayer->ID);
}
