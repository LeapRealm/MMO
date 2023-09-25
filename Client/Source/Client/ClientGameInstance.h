#pragma once

#include "CoreMinimal.h"
#include "Client.h"
#include "Protocol.pb.h"
#include "Engine/GameInstance.h"
#include "ClientGameInstance.generated.h"

class AClientMyPlayer;
class AClientPlayer;
class PacketSession;
class FSocket;

UCLASS()
class CLIENT_API UClientGameInstance : public UGameInstance, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Shutdown() override;
	
public:
	void RequestConnect();
	void HandleRecvPackets();
	void RequestDisconnect();
	void Disconnect();

	void SendPacket(SendBufferRef SendBuffer);

public:
	void HandleSpawn(const Protocol::PlayerInfo& PlayerInfo, bool IsMyPlayer);
	void HandleSpawn(const Protocol::S_ENTER_GAME& EnterGamePkt);
	void HandleSpawn(const Protocol::S_SPAWN& SpawnPkt);

	void HandleDespawn(uint64 ObjectID);
	void HandleDespawn(const Protocol::S_DESPAWN& DespawnPkt);

	void HandleMove(const Protocol::S_MOVE& MovePkt);

public:
	FORCEINLINE virtual TStatId GetStatId() const override { return Super::GetStatID(); }
	FORCEINLINE virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	FORCEINLINE virtual ETickableTickType GetTickableTickType() const override { return IsTemplate() ? ETickableTickType::Never : FTickableGameObject::GetTickableTickType(); }
	FORCEINLINE virtual bool IsAllowedToTick() const override { return IsTemplate() == false; }
	
public:
	FSocket* Socket;
	FString IPAddress = TEXT("127.0.0.1");
	int16 Port = 7777;
	TSharedPtr<PacketSession> GameServerSession;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AClientPlayer> OtherPlayerClass;

	UPROPERTY()
	TObjectPtr<AClientMyPlayer> MyPlayer;
	
	UPROPERTY()
	TMap<uint64, TObjectPtr<AClientPlayer>> Players;
};
