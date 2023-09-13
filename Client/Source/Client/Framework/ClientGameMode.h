#pragma once

#include "CoreMinimal.h"
#include "Struct.pb.h"
#include "GameFramework/GameModeBase.h"
#include "ClientGameMode.generated.h"

class APlayerBase;
class AMyPlayerBase;
class UClientGameInstance;

UCLASS()
class CLIENT_API AClientGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AClientGameMode();
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void RequestEnterGame();
	
	void AddPlayer(const Protocol::PlayerInfo& PlayerInfo, bool bMyPlayer = false);
	void RemovePlayer(uint64 ID);
	void RemoveMyPlayer();
	
public:
	UPROPERTY(VisibleAnywhere)
	TMap<uint64, TObjectPtr<APlayerBase>> Players;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AMyPlayerBase> MyPlayer;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UClientGameInstance> ClientGameInstance;
};
