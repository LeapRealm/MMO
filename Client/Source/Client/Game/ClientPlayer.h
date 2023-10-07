#pragma once

#include "CoreMinimal.h"
#include "Protocol.pb.h"
#include "GameFramework/Character.h"
#include "ClientPlayer.generated.h"

UCLASS()
class CLIENT_API AClientPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AClientPlayer();
	virtual ~AClientPlayer() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	void UpdateCurrentPlayerInfo();

public:
	void Init(bool IsMyPlayer, const Protocol::PlayerInfo& Info);

public:
	bool IsMyPlayer() { return bIsMyPlayer; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetCurrentLocation() { return FVector(CurrentPlayerInfo->transform().x(), CurrentPlayerInfo->transform().y(), CurrentPlayerInfo->transform().z()); };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetDesiredLocation() { return FVector(DesiredPlayerInfo->transform().x(), DesiredPlayerInfo->transform().y(), DesiredPlayerInfo->transform().z()); };

	Protocol::PlayerInfo* GetCurrentPlayerInfo() { return CurrentPlayerInfo; }
	void SetCurrentPlayerInfo(const Protocol::PlayerInfo& Info);
	
	Protocol::PlayerInfo* GetDesiredPlayerInfo() { return DesiredPlayerInfo; }
	void SetDesiredPlayerInfo(const Protocol::PlayerInfo& Info);
	
protected:
	bool bIsMyPlayer = false;
	
	Protocol::PlayerInfo* CurrentPlayerInfo = new Protocol::PlayerInfo();
	Protocol::PlayerInfo* DesiredPlayerInfo = new Protocol::PlayerInfo();

private:
	const float MoveThreshold = 100.f;
	float MoveThresholdSquared = MoveThreshold * MoveThreshold;
};
