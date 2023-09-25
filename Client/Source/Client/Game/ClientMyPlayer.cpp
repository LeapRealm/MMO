#include "ClientMyPlayer.h"

#include "Client.h"
#include "ClientGameInstance.h"
#include "ClientPacketHandler.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AClientMyPlayer::AClientMyPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetRootComponent(), USpringArmComponent::SocketName);
	CameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 73.f));
	CameraComponent->bUsePawnControlRotation = true;
	
	bUseControllerRotationYaw = true;
	GetMesh()->SetOwnerNoSee(true);
	
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->bRunPhysicsWithNoController = false;
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
	
	TickSendMovePacket();
}

void AClientMyPlayer::TickSendMovePacket()
{
	MovePacketSendTimer -= GetWorld()->DeltaTimeSeconds;

	if (bForceSendPacket || MovePacketSendTimer <= 0)
	{
		bForceSendPacket = false;
		MovePacketSendTimer = MOVE_PACKET_SEND_DELAY;

		Protocol::C_MOVE MovePkt;
		Protocol::PlayerInfo* Info = MovePkt.mutable_info();
		Info->CopyFrom(*CurrentPlayerInfo);
		SEND_PACKET(MovePkt);
	}
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
