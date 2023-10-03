#pragma once

#include "CoreMinimal.h"
#include "Game/ClientPlayer.h"
#include "ClientMyPlayer.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class CLIENT_API AClientMyPlayer : public AClientPlayer
{
	GENERATED_BODY()

public:
	AClientMyPlayer();

public:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

private:
	void TickSendMovePacket(float DeltaTime);
	void UpdateDesiredPlayerInfo();
	
protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> JumpAction;

protected:
	float MovePacketSendDelay = 0.1f;
	float MovePacketSendTimer = MovePacketSendDelay;
	
	bool bForceSendPacket = false;
	FVector2D LastMoveInput = FVector2D::ZeroVector;
};
