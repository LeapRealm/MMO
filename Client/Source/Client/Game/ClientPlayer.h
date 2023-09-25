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
	
	Protocol::PlayerInfo* GetCurrentPlayerInfo() { return CurrentPlayerInfo; }
	void SetCurrentPlayerInfo(const Protocol::PlayerInfo& Info);

	Protocol::PlayerInfo* GetDesiredPlayerInfo() { return DesiredPlayerInfo; }
	void SetDesiredPlayerInfo(const Protocol::PlayerInfo& Info);

	Protocol::MoveState GetMoveState() { return CurrentPlayerInfo->movestate(); }
	void SetMoveState(Protocol::MoveState State);
	
protected:
	bool bIsMyPlayer = false;
	Protocol::PlayerInfo* CurrentPlayerInfo;
	Protocol::PlayerInfo* DesiredPlayerInfo;

private:
	const float MoveThreshold = 20.f;
	float MoveThresholdSquared;
};
