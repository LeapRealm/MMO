#include "PlayerPawn.h"

#include "ClientGameInstance.h"
#include "ClientPacketHandler.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->TargetArmLength = 600.f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bDoCollisionTest = false;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(GetRootComponent());
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	ClientGameInstance = Cast<UClientGameInstance>(GetGameInstance());

	TargetPosition = GetActorLocation();
	TargetRotation = GetActorRotation();
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetActorLocation(FMath::VInterpConstantTo(GetActorLocation(), TargetPosition, DeltaTime, MoveSpeed));
	SetActorRotation(FMath::RInterpConstantTo(GetActorRotation(), TargetRotation, DeltaTime, RotSpeed));
	
	MovePktTime += DeltaTime;
}

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void APlayerPawn::HandleMove(FVector2D Input)
{
	FRotator Rotation = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	FVector Forward = UKismetMathLibrary::GetForwardVector(Rotation);
	FVector Right = UKismetMathLibrary::GetRightVector(Rotation);
	
	FVector Direction = (Input.X * Forward + Input.Y * Right).GetSafeNormal();
	AddActorWorldOffset(Direction * GetWorld()->DeltaTimeSeconds * MoveSpeed);
	TargetPosition = GetActorLocation();

	if (Input != FVector2D::ZeroVector)
		TargetRotation = FRotator(0.f, UKismetMathLibrary::Conv_VectorToRotator(Direction).Yaw, 0.f);

	if (MovePktTime >= TargetMovePktTime || PrevInput != Input)
	{
		MovePktTime = 0.f;

		Protocol::C_MOVE MovePkt;
		Protocol::Vector3D* Position = MovePkt.mutable_transform()->mutable_position();
		MovePkt.mutable_transform()->set_yaw(TargetRotation.Yaw);
		
		FVector Pos = GetActorLocation();
		if (Input != FVector2D::ZeroVector)
			Pos += Direction * 75.f;
		
		Position->set_x(Pos.X);
		Position->set_y(Pos.Y);
		Position->set_z(Pos.Z);
		SEND_PACKET(MovePkt);
	}

	PrevInput = Input;
}

void APlayerPawn::HandleJump(bool Input)
{
	if (IsFalling || Input == false)
		return;

	IsFalling = true;
}
