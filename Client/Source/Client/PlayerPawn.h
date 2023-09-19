#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class UClientGameInstance;

UCLASS()
class CLIENT_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	UFUNCTION(BlueprintCallable)
	void HandleMove(FVector2D Input);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetPosition;
	
	UPROPERTY(VisibleAnywhere)
	float Speed = 450.f;

	UPROPERTY(VisibleAnywhere)
	float MovePktTime = 0.f;
	float TargetMovePktTime = 0.15f;

	UPROPERTY(VisibleAnywhere)
	FVector2D PrevInput;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UClientGameInstance> ClientGameInstance;

};
