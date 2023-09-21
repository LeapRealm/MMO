#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
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

	UFUNCTION(BlueprintCallable)
	void HandleJump(bool Input);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator TargetRotation;
	
	float MoveSpeed = 450.f;
	float RotSpeed = 720.f;
	
	float MovePktTime = 0.f;
	float TargetMovePktTime = 0.15f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D PrevInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity;
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UClientGameInstance> ClientGameInstance;

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsFalling;
};
