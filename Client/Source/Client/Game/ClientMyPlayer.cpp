#include "ClientMyPlayer.h"

#include "Client.h"
#include "ClientGameInstance.h"
#include "ClientPacketHandler.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AClientMyPlayer::AClientMyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetMesh()->SetOwnerNoSee(true);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetRootComponent(), USpringArmComponent::SocketName);
	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 73.f));
	CameraComponent->bUsePawnControlRotation = true;
	
	bUseControllerRotationYaw = true;
}

void AClientMyPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AClientMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AClientMyPlayer::Look);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AClientMyPlayer::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AClientMyPlayer::Move);
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}

void AClientMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	TickSendMovePacket(DeltaTime);
}

void AClientMyPlayer::TickSendMovePacket(float DeltaTime)
{
	if (GetCharacterMovement()->Velocity.Z)
		MovePacketSendDelay = 0.04f;
	else
		MovePacketSendDelay = 0.08f;
	
	MovePacketSendTimer -= DeltaTime;

	if (bForceSendPacket || MovePacketSendTimer <= 0)
	{
		bForceSendPacket = false;
		MovePacketSendTimer = MovePacketSendDelay;

		UpdateDesiredPlayerInfo();
		
		Protocol::C_MOVE_PLAYER MovePkt;
		Protocol::PlayerInfo* Info = MovePkt.mutable_info();
		Info->CopyFrom(*DesiredPlayerInfo);
		
		SEND_PACKET(MovePkt);
	}
}

void AClientMyPlayer::UpdateDesiredPlayerInfo()
{
	DesiredPlayerInfo->CopyFrom(*CurrentPlayerInfo);

	FVector NewDesiredLocation = GetActorLocation();
	
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->SweepSingleByChannel(HitResult, GetActorLocation(), NewDesiredLocation, GetCapsuleComponent()->GetComponentRotation().Quaternion(),
		ECC_Pawn, FCollisionShape::MakeCapsule(GetCapsuleComponent()->GetScaledCapsuleRadius(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight()), QueryParams);
	
	if (HitResult.bBlockingHit)
		NewDesiredLocation = HitResult.Location;
	
	Protocol::Transform* DesiredTransform = DesiredPlayerInfo->mutable_transform();
	DesiredTransform->set_x(NewDesiredLocation.X);
	DesiredTransform->set_y(NewDesiredLocation.Y);
	DesiredTransform->set_z(NewDesiredLocation.Z);
}

void AClientMyPlayer::Move(const FInputActionValue& Value)
{
	FVector2D MoveInput = Value.Get<FVector2D>();

	if (Controller)
	{
		if (LastMoveInput != MoveInput)
		{
			bForceSendPacket = true;
			LastMoveInput = MoveInput;
		}
		
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MoveInput.Y);
		AddMovementInput(RightDirection, MoveInput.X);
	}
}

void AClientMyPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookInput.X);
		AddControllerPitchInput(LookInput.Y);
	}
}
