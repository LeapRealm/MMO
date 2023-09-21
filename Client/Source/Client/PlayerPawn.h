#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class UClientGameInstance;

UCLASS()
class CLIENT_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;

public:
	UFUNCTION(BlueprintCallable)
	void HandleMove(FVector2D Input);

private:
	void SendMovePacket();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector SimulatedPlayerTargetPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector ControlledPlayerVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bPlayerControlled = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MoveSpeed = 450.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RotationSpeed = 720.f;

public:
	float MovePktTime = 0.f;
	float TargetMovePktTime = 0.15f;
	
	FVector2D PrevInput;
	FRotator TargetRotation;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UClientGameInstance> ClientGameInstance;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
};
