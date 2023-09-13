#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "MyPlayerBase.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class CLIENT_API AMyPlayerBase : public APlayerBase
{
	GENERATED_BODY()

public:
	AMyPlayerBase();

public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
};
