#pragma once

#include "CoreMinimal.h"
#include "Client.h"
#include "Protocol.pb.h"
#include "Engine/GameInstance.h"
#include "ClientGameInstance.generated.h"

class PacketSession;
class FSocket;

UCLASS()
class CLIENT_API UClientGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ConnectToGameServer();

	UFUNCTION(BlueprintCallable)
	void DisconnectFromGameServer();

	UFUNCTION(BlueprintCallable)
	void HandleRecvPackets();

	void SendPacket(SendBufferRef SendBuffer);

public:
	void HandleSpawn(const Protocol::PlayerInfo& PlayerInfo);
	void HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt);
	void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);

	void HandleDespawn(uint64 ObjectID);
	void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);
	
public:
	FSocket* Socket;
	FString IPAddress = TEXT("127.0.0.1");
	int16 Port = 7777;
	TSharedPtr<PacketSession> GameServerSession;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<APawn> PlayerClass;

	UPROPERTY(VisibleAnywhere)
	TMap<uint64, TObjectPtr<AActor>> Players;
};
