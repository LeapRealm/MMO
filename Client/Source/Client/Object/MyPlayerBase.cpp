#include "MyPlayerBase.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AMyPlayerBase::AMyPlayerBase()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetRelativeRotation(FRotator(-75.f, 0.f, 0.f));
	SpringArmComponent->TargetArmLength = 1500.f;
	SpringArmComponent->bDoCollisionTest = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);	
}

void AMyPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
