#pragma once

#include "CoreMinimal.h"
#include "Protocol.pb.h"
#include "GameFramework/Character.h"
#include "MonsterBase.generated.h"

UCLASS()
class CLIENT_API AMonsterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AMonsterBase();
	virtual ~AMonsterBase() override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
protected:
	void UpdateCurrentMonsterInfo();

public:
	void Init(const Protocol::MonsterInfo& Info);
	void Attack();
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetCurrentLocation() { return FVector(CurrentMonsterInfo->transform().x(), CurrentMonsterInfo->transform().y(), CurrentMonsterInfo->transform().z()); };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetDesiredLocation() { return FVector(DesiredMonsterInfo->transform().x(), DesiredMonsterInfo->transform().y(), DesiredMonsterInfo->transform().z()); };

	
	Protocol::MonsterInfo* GetCurrentMonsterInfo() { return CurrentMonsterInfo; }
	void SetCurrentMonsterInfo(const Protocol::MonsterInfo& Info);

	Protocol::MonsterInfo* GetDesiredMonsterInfo() { return DesiredMonsterInfo; }
	void SetDesiredMonsterInfo(const Protocol::MonsterInfo& Info);

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> AttackMontage;
	
protected:
	Protocol::MonsterInfo* CurrentMonsterInfo = new Protocol::MonsterInfo();
	Protocol::MonsterInfo* DesiredMonsterInfo = new Protocol::MonsterInfo();

private:
	const float MoveThreshold = 100.f;
	float MoveThresholdSquared = MoveThreshold * MoveThreshold;
};
